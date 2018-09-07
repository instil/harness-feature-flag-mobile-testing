// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "RtspClient.h"

#import "Url.h"
#import "InterleavedRtspTransport.h"
#import "UdpTransport.h"
#import "SecureInterleavedTCPTransport.h"
#import "TLSFactory.h"
#import "BasicDigestAuthenticator.h"
#import "H264Depacketizer.h"
#import "MJPEGDepacketizer.h"
#import "MP4VDepacketizer.h"
#import "ErrorDispatcher.h"

using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS;
using SurgeUtil::Constants::DEFAULT_PACKET_BUFFER_DELAY_MS;
using SurgeUtil::Constants::DEFAULT_NO_PACKET_TIMEOUT_MS;

Surge::RtspClient::RtspClient(IRtspClientDelegate * delegate, bool forceInterleavedTcp) : delegate(delegate), url(""), isPlaying(false), startTime(nullptr), endTime(nullptr), timeout(DEFAULT_NO_PACKET_TIMEOUT_MS), transport(nullptr), tlsClient(nullptr), useInterleavedTcpTransport(forceInterleavedTcp), tlsCertificateValidationEnabled(true), tlsSelfSignedCertsAllowed(true), depacketizer(nullptr), timeLastPacketWasProcessed(0), lastKeepAliveMs(0), keepAliveIntervalInSeconds(0), processedFirstPayload(false), lastPacketSequenceNum(MAX_SEQ_NUM), dropFrame(false) {

    StartErrorDispatcher();

    packetBuffer = new RtpPacketBuffer(DEFAULT_PACKET_BUFFER_DELAY_MS);
    packetBuffer->SetPacketAvailableCallback([this](RtpPacket *packet) {
        this->ProcessRtpPacket(packet);
        delete packet;
    });

    frameBuffer = new std::vector<unsigned char>();

    dispatchQueue = new DispatchQueue();
    dispatchQueue->Start();

    authService = new AuthenticationService();
    authService->Add(new BasicDigestAuthenticator());

    rtspService = new RtspService();
    rtspService->SetAuthService(authService);
}

Surge::RtspClient::~RtspClient() {
    if (transport != nullptr && transport->IsRunning()) {
        Disconnect();
    }

    if (housekeepingThread.IsRunning()) {
        housekeepingThread.Stop();
    }

    CloseErrorDispatcher();

    delete rtspService;
    delete frameBuffer;
    delete transport;

    if (depacketizer != nullptr) {
        delete depacketizer;
    }

    dispatchQueue->Stop();
    delete dispatchQueue;

    if (authService != nullptr) {
        delete authService;
    }

    if (tlsClient != nullptr) {
        delete tlsClient;
    }
}

void Surge::RtspClient::Connect(const std::string& url, std::function<void(bool)> callback) {
    // TODO: What to do if stream is connected?
    
    this->url = url;
    isPlaying = false;

    SetTransport(this->url);

    SurgeUtil::Url url_model(url);

    transport->RtspTcpOpen(url_model, [this, callback](int result) {
        if (result != 0) {
            ERROR("Could not connect to the supplied URL to initiate streaming. Incorrect URL?");
        }

        dispatchQueue->Dispatch([callback, result]() { callback(result == 0); });
    });
}

void Surge::RtspClient::SetTransport(std::string& url) {
    if (transport != nullptr) {
        delete transport;
    }
    if (tlsClient != nullptr) {
        delete tlsClient;
    }

    SurgeUtil::Url parsedUrl = SurgeUtil::Url(url);

    if (parsedUrl.GetProtocol() == "rtsp") {
        if (useInterleavedTcpTransport) {
            INFO("Using Interleaved TCP Transport");
            transport = new InterleavedRtspTransport(nullptr, this);

            INFO("Disabling packet buffer");
            packetBuffer->SetBufferDelay(0);
        } else {
            INFO("Using UDP Transport");
            transport = new UdpTransport(nullptr);
        }
    } else {
        INFO("Using TLS TCP Transport");
        tlsClient = TLSFactory::GenerateTLSClient(tlsCertificateValidationEnabled,
                                                  tlsSelfSignedCertsAllowed);

        transport = new SecureInterleavedTCPTransport(tlsClient,
                                                      nullptr,
                                                      this);
        packetBuffer->SetBufferDelay(0);

        url = parsedUrl.WithRtspProtocol();
    }

    transport->SetDelegate(this);
    rtspService->SetStreamTransport(transport, url);
    authService->SetTransport(transport);
}

void Surge::RtspClient::Disconnect() {
    // TODO: Issue teardown if needed

    StopHousekeepingThread();

    if (transport->IsRunning()) {
        transport->StopRunning();
    }
}

void Surge::RtspClient::SetCredentials(const std::string& user, const std::string& password) {
    authService->SetStreamCredentials(url, user, password);
}

void Surge::RtspClient::Describe(std::function<void(Surge::DescribeResponse*)> callback) {
    // TODO: Check for connection already established

    DEBUG("Executing Auth calls");

    authService->ExecuteFirstBytesOnTheWireAuthentication();

    DEBUG("Sending DESCRIBE request");

    auto runCallback = [this, callback](Surge::DescribeResponse *response) {
        if (response == nullptr || !response->Ok()) {
            ERROR("DESCRIBE command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Describe(startTime, [this, runCallback] (Surge::DescribeResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Describe(startTime, runCallback);
            return;
        }

        runCallback(response);
    });
}

void Surge::RtspClient::Setup(const SessionDescription& sessionDescription, std::function<void(Surge::SetupResponse*)> callback) {
    DEBUG("Sending SETUP request");

    this->sessionDescription = sessionDescription;

    CreateDepacketizer();

    // New session = no processed payloads
    processedFirstPayload = false;

    auto runCallback = [this, callback](Surge::SetupResponse *response) {
        if (response != nullptr && response->Ok()) {
            lastKeepAliveMs = SurgeUtil::DateTime::CurrentTimeInMilliseconds();
            keepAliveIntervalInSeconds = response->GetTimeoutSeconds();

            DEBUG("RtspClient KeepAlive Interval set to: " << keepAliveIntervalInSeconds);

            transport->SetRtpPortsAndChannels(response);
        } else {
            ERROR("SETUP command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Setup(sessionDescription, [this, &sessionDescription, runCallback] (Surge::SetupResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Setup(sessionDescription, runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::CreateDepacketizer() {
    switch (sessionDescription.GetType()) {
        case H264:
            INFO("H264 detected");
            depacketizer = new H264Depacketizer(sessionDescription, frameBuffer);
            break;
        case MP4V:
            INFO("MP4V detected");
            depacketizer = new MP4VDepacketizer(sessionDescription, frameBuffer);
            break;
        case MJPEG:
            INFO("MJPEG detected");
            depacketizer = new MJPEGDepacketizer(sessionDescription, frameBuffer);
            break;
        default:
            ERROR("Content type " << sessionDescription.GetType() << " not currently supported");
            return;
    }
}

void Surge::RtspClient::Play(std::function<void(Surge::RtspResponse*)> callback) {
    DEBUG("Sending PLAY request");

    auto runCallback = [this, callback](Surge::RtspResponse *response) {
        isPlaying = response != nullptr && response->Ok();

        if (!isPlaying) {
            ERROR("PLAY command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        StartHousekeepingThread();

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Play(startTime, endTime, [this, runCallback] (Surge::RtspResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Play(startTime, endTime, runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::Pause(std::function<void(Surge::RtspResponse*)> callback) {
    DEBUG("Sending PAUSE request");

    auto runCallback = [this, callback](Surge::RtspResponse *response) {
        isPlaying = false;

        if (response == nullptr || !response->Ok()) {
            ERROR("PAUSE command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Pause([this, runCallback] (Surge::RtspResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Pause(runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::Options(std::function<void(Surge::RtspResponse*)> callback) {
    DEBUG("Sending OPTIONS request");

    auto runCallback = [this, callback](Surge::RtspResponse *response) {
        if (response == nullptr || !response->Ok()) {
            ERROR("OPTIONS command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Options([this, runCallback] (Surge::RtspResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Options(runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::Teardown(std::function<void(Surge::RtspResponse*)> callback) {
    DEBUG("Sending TEARDOWN request");

    auto runCallback = [this, callback](Surge::RtspResponse *response) {
        StopHousekeepingThread();

        if (response == nullptr || !response->Ok()) {
            ERROR("TEARDOWN command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->Teardown([this, runCallback] (Surge::RtspResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->Teardown(runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::KeepAlive(std::function<void(Surge::RtspResponse*)> callback) {
    DEBUG("Sending Keep-Alive request");

    auto runCallback = [this, callback](Surge::RtspResponse *response) {
        if (response == nullptr || !response->Ok()) {
            ERROR("Keep-Alive command failed");

            if (response != nullptr) {
                ERROR(response->GetRawResponse());
            }
        }

        dispatchQueue->Dispatch([callback, response]() { callback(response); });
    };

    rtspService->KeepAlive([this, runCallback] (Surge::RtspResponse *response) {
        if (response != nullptr && !response->Ok() && authService->UpdateAuthForUnauthorizedError(response)) {
            rtspService->KeepAlive(runCallback);
        }

        runCallback(response);
    });
}

void Surge::RtspClient::StartHousekeepingThread() {
    if (housekeepingThread.IsRunning()) {
        return;
    }

    housekeepingThread.Execute(*this);
}

void Surge::RtspClient::StopHousekeepingThread() {
    if (housekeepingThread.IsRunning()) {
        housekeepingThread.Stop();
    }
}

void Surge::RtspClient::RtpPacketReceived(RtpPacket *packet) {
    if (packet != nullptr) {
        packetBuffer->AddPacketToBuffer(packet);

        timeLastPacketWasProcessed = SurgeUtil::DateTime::CurrentTimeInMilliseconds();
    }
}

void Surge::RtspClient::Run() {
    timeLastPacketWasProcessed = SurgeUtil::DateTime::CurrentTimeInMilliseconds();

    transport->SetRtpCallback([&](RtpPacket* packet) {
        if (packet != nullptr) {
            packetBuffer->AddPacketToBuffer(packet);

            long long int test = SurgeUtil::DateTime::CurrentTimeInMilliseconds();
            timeLastPacketWasProcessed = test;
        }
    });

    while (true) {
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({housekeepingThread.StopRequested()}, 1000);

        // STOP
        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, *(housekeepingThread.StopRequested()))) {
            DEBUG("RtspClient - Stop Requested.");
            break;
        }

        // TIMEOUT
        int64_t timeout_delta_ms = SurgeUtil::DateTime::CurrentTimeInMilliseconds() - timeLastPacketWasProcessed;
        int maxTimeBetweenFrames = timeout + sessionDescription.GetTimeoutOffset();
        bool clientDidTimeout = isPlaying && timeout_delta_ms >= maxTimeBetweenFrames;
        if (clientDidTimeout) {
            ERROR("No processed packets in the last " << maxTimeBetweenFrames << "(ms). Issueing timeout signal.");
            NotifyDelegateTimeout();
            break;
        }

        // KEEP ALIVE
        int64_t delta = SurgeUtil::DateTime::CurrentTimeInMilliseconds() - lastKeepAliveMs;
        int64_t deltaSeconds = delta / 1000;
        bool needKeepAlive = deltaSeconds >= static_cast<int64_t>(keepAliveIntervalInSeconds * 0.9);
        if (needKeepAlive) {
            DEBUG("Sending Keep-Alive for session: " << url);

            KeepAlive([&](RtspResponse *resp) {
                if (resp == nullptr) {
                    // notify delegate via error dispatcher
                    ERROR("Failed to get response to keep alive");
                    NotifyDelegateTimeout();
                    Disconnect();
                } else if (!resp->Ok()) {
                    ERROR("Failed Keep-Alive request: " << resp->GetCode());
                    delete resp;
                    NotifyDelegateTimeout();
                    Disconnect();
                } else {
                    DEBUG("Keep Alive response received.");
                    lastKeepAliveMs = SurgeUtil::DateTime::CurrentTimeInMilliseconds();
                    delete resp;
                }
            });
        }
    }

    transport->SetRtpCallback([&](RtpPacket* packet) { delete packet; });
    INFO("Rtsp Client is Finished");
}

void Surge::RtspClient::ProcessRtpPacket(const RtpPacket* packet) {
    if (depacketizer == nullptr) {
        WARNING("Received RTP packet but no depacketizer available");
        return;
    }

    depacketizer->ProcessPacket(packet, !processedFirstPayload);
    processedFirstPayload = true;

    if (packet->HasExtension()) {
        NotifyDelegateOfExtendedRtpHeader(packet->GetExtensionData(),
                                          packet->GetExtensionLength());
    }

    CheckIfFrameShouldBeDropped(packet);

    if (!packet->IsMarked()) {
        return;
    }

    std::vector<unsigned char> *frame = new std::vector<unsigned char>();
    frameBuffer->swap(*frame);
    dispatchQueue->Dispatch([this, frame]() {
        NotifyDelegateOfAvailableFrame(*frame);
        delete frame;
    });
}

void Surge::RtspClient::CheckIfFrameShouldBeDropped(const Surge::RtpPacket* packet) {
    if (sessionDescription.GetType() != MJPEG) {
        return;
    }

    if (packet->GetSequenceNumber() != (++lastPacketSequenceNum % MAX_SEQ_NUM)) {
        DropNextFrame();
        lastPacketSequenceNum = packet->GetSequenceNumber();
    }
}
