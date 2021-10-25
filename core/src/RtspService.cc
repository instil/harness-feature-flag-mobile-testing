//
//  RtspService.cc
//  SurgeCore
//
//  Created by Paul Shields on 16/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "RtspService.h"

#include "StringUtils.h"

#define NO_SESSION_AVAILABLE_ERROR "Session is not set, you must first execute a successful SETUP command before running this method."

Surge::RtspService::RtspService() : transport(nullptr), authService(nullptr), sequenceNumber(1), sessionDescriptionFactory(new SessionDescriptionFactory()), session() { }

Surge::RtspService::~RtspService() {
    if (sessionDescriptionFactory != nullptr) {
        delete sessionDescriptionFactory;
    }
}

void Surge::RtspService::Describe(const SurgeUtil::DateTime *startTime, std::function<void(Surge::DescribeResponse*)> callback) {

    RtspCommand* describe;
    if (startTime != nullptr) {
        describe = RtspCommandFactory::DescribeRequest(streamUrl, NextSequenceNumber(), *startTime, authService);
    }
    else {
        describe = RtspCommandFactory::DescribeRequest(streamUrl, NextSequenceNumber(), authService);
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
            
            setContentBaseUrlIfRequired(resp);
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
    std::string setupUrl = GenerateControlUrl(sessionDescription);

    RtspCommand* setup = RtspCommandFactory::SetupRequest(setupUrl, NextSequenceNumber(), transport, authService);

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

std::string Surge::RtspService::GenerateControlUrl(const SessionDescription& sessionDescription) {
    if (sessionDescription.IsControlUrlComplete()) {
        return sessionDescription.GetControl();
    }

    std::string controlUrl = streamContentBaseUrl;
    if (!SurgeUtil::String::EndsWith(controlUrl, "/")) controlUrl += "/";
    controlUrl += sessionDescription.GetControl();

    return controlUrl;
}

void Surge::RtspService::Play(const SurgeUtil::DateTime *startTime, const SurgeUtil::DateTime *endTime, std::function<void(Surge::RtspResponse*)> callback) {
    if (session.empty()) {
        ERROR(NO_SESSION_AVAILABLE_ERROR);
        callback(nullptr);
        return;
    }

    RtspCommand* play;

    if (endTime != nullptr) {
        play = RtspCommandFactory::PlayRequest(streamContentBaseUrl, session, NextSequenceNumber(), *startTime, *endTime, authService);
    }
    else if (startTime != nullptr) {
        play = RtspCommandFactory::PlayRequest(streamContentBaseUrl, session, NextSequenceNumber(), *startTime, authService);
    }
    else {
        play = RtspCommandFactory::PlayRequest(streamContentBaseUrl, session, NextSequenceNumber(), authService);
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
    if (session.empty()) {
        ERROR(NO_SESSION_AVAILABLE_ERROR);
        callback(nullptr);
        return;
    }

    RtspCommand* pause = RtspCommandFactory::PauseRequest(streamContentBaseUrl, session, NextSequenceNumber(), authService);
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
    if (session.empty()) {
        ERROR(NO_SESSION_AVAILABLE_ERROR);
        callback(nullptr);
        return;
    }

    RtspCommand* options = RtspCommandFactory::OptionsRequest(streamContentBaseUrl, session, NextSequenceNumber(), authService);
    transport->RtspTransaction(options, [callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to OPTIONS!");
            callback(nullptr);
            return;
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
    if (session.empty()) {
        ERROR(NO_SESSION_AVAILABLE_ERROR);
        callback(nullptr);
        return;
    }

    RtspCommand* teardown = RtspCommandFactory::TeardownRequest(streamContentBaseUrl, session, NextSequenceNumber(), authService);
    transport->RtspTransaction(teardown, [this, callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to TEARDOWN!");
            if (callback) {
                callback(nullptr);
            }
            return;
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

        ResetRtspSession();
        callback(resp);
    });

    delete teardown;
}

void Surge::RtspService::KeepAlive(std::function<void(Surge::RtspResponse*)> callback) {
    if (session.empty()) {
        ERROR(NO_SESSION_AVAILABLE_ERROR);
        callback(nullptr);
        return;
    }

    RtspCommand* keep_alive = RtspCommandFactory::KeepAliveRequest(streamContentBaseUrl, session, NextSequenceNumber(), authService);
    transport->RtspTransaction(keep_alive, [callback](Response *raw_resp) {
        if (raw_resp == nullptr) {
            ERROR("Failed to get response to Keep-Alive!");
            callback(nullptr);
            return;
        }

        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        } catch (const std::exception& e) {
            ERROR("Invalid Keep-alive Response: " << e.what());
            resp = nullptr;
        }

        delete raw_resp;

        callback(resp);
    });

    delete keep_alive;
}
