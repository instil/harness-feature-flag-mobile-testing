//
//  RtspService.cc
//  SurgeCore
//
//  Created by Paul Shields on 16/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "RtspService.h"

Surge::RtspService::RtspService() : transport(nullptr), authService(nullptr), sequenceNumber(1), sessionDescriptionFactory(new SessionDescriptionFactory()), session() { }

Surge::RtspService::~RtspService() {
    if (sessionDescriptionFactory != nullptr) {
        delete  sessionDescriptionFactory;
    }
}

void Surge::RtspService::Describe(const SurgeUtil::DateTime *startTime, std::function<void(Surge::DescribeResponse*)> callback) {

    RtspCommand* describe;
    if (startTime != nullptr) {
        describe = RtspCommandFactory::DescribeRequest(streamUrl, NextSequenceNumber(), *startTime);
    }
    else {
        describe = RtspCommandFactory::DescribeRequest(streamUrl, NextSequenceNumber());
    }

    transport->RtspTransaction(describe, [=](Response *raw_resp) {
        INFO("Received DESCRIBE response");

        if (raw_resp == nullptr) {
            ERROR("Failed to get response to DESCRIBE!");
            callback(nullptr);
            return;
        }

        DescribeResponse *resp = nullptr;
        try {
            resp = new DescribeResponse(raw_resp, sessionDescriptionFactory);
        }
        catch (const std::exception& e) {
            ERROR("Invalid DescribeResponse: " << e.what());
            resp = nullptr;
        }

        delete raw_resp;

        callback(resp);
    });

    delete describe;
}

void Surge::RtspService::Setup(const SessionDescription& sessionDescription, std::function<void(Surge::SetupResponse*)> callback) {
    std::string setupUrl = (sessionDescription.IsControlUrlComplete()) ?
        sessionDescription.GetControl():
        streamUrl + "/" + sessionDescription.GetControl();

    RtspCommand* setup = RtspCommandFactory::SetupRequest(setupUrl, NextSequenceNumber(), transport);

    transport->RtspTransaction(setup, [=](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to SETUP!");
            callback(nullptr);
            return;
        }

        SetupResponse* resp = nullptr;
        try {
            resp = new SetupResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid SetupResponse: " << e.what());
            resp = nullptr;
        }

        delete raw_resp;

        if (resp != nullptr && resp->Ok()) {
            DEBUG("RtspClient Session set to: " << resp->GetSession());
            session = resp->GetSession();
        }

        callback(resp);
    });

    delete setup;
}

void Surge::RtspService::Play(const SurgeUtil::DateTime *startTime, const SurgeUtil::DateTime *endTime, std::function<void(Surge::RtspResponse*)> callback) {
    // TODO: No session set error handling

    RtspCommand* play;

    if (endTime != nullptr) {
        play = RtspCommandFactory::PlayRequest(streamUrl, session, NextSequenceNumber(), *startTime, *endTime);
    }
    else if (startTime != nullptr) {
        play = RtspCommandFactory::PlayRequest(streamUrl, session, NextSequenceNumber(), *startTime);
    }
    else {
        play = RtspCommandFactory::PlayRequest(streamUrl, session, NextSequenceNumber());
    }

    transport->RtspTransaction(play, [=](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to PLAY!");
            callback(nullptr);
            return;
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid PlayResponse: " << e.what());
            resp = nullptr;
        }

        delete raw_resp;

        callback(resp);
    });

    delete play;
}

void Surge::RtspService::Pause(std::function<void(Surge::RtspResponse*)> callback) {
    // TODO: No session set error handling

    RtspCommand* pause = RtspCommandFactory::PauseRequest(streamUrl, session, NextSequenceNumber());
    transport->RtspTransaction(pause, [=](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to PAUSE!");
            callback(nullptr);
            return;
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid PauseResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;

        if (callback != NULL) {
            callback(resp);
        }
    });

    delete pause;
}

void Surge::RtspService::Options(std::function<void(Surge::RtspResponse*)> callback) {
    // TODO: No session set error handling

    RtspCommand* options = RtspCommandFactory::OptionsRequest(streamUrl, session, NextSequenceNumber());
    transport->RtspTransaction(options, [callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to OPTIONS!");
            callback(nullptr);
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid OptionsResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;

        callback(resp);
    });

    delete options;
}

void Surge::RtspService::Teardown(std::function<void(Surge::RtspResponse*)> callback) {
    // TODO: No session set error handling

    RtspCommand* teardown = RtspCommandFactory::TeardownRequest(streamUrl, session, NextSequenceNumber());
    transport->RtspTransaction(teardown, [callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to TEARDOWN!");
            if (callback) {
                callback(nullptr);
            }
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid TeardownResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;

        callback(resp);
    });

    delete teardown;
}

void Surge::RtspService::KeepAlive(std::function<void(Surge::RtspResponse*)> callback) {
    // TODO: No session set error handling

    RtspCommand* keep_alive = RtspCommandFactory::KeepAliveRequest(streamUrl, session, NextSequenceNumber());
    transport->RtspTransaction(keep_alive, [callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to Keep-Alive!");
            callback(nullptr);
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        } catch (const std::exception& e) {
            ERROR("Invalid KeepaliveResponse: " << e.what());
            resp = nullptr;
        }

        delete raw_resp;

        callback(resp);
    });

    delete keep_alive;
}
