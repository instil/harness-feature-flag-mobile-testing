//
//  AuthenticationService.cc
//  SurgeCore
//
//  Created by Paul Shields on 27/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "AuthenticationService.h"

Surge::AuthenticationService::AuthenticationService() : transport(nullptr), username(""), password("") { }

Surge::AuthenticationService::~AuthenticationService() {
    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [](auto authenticator) {
                      delete authenticator;
                  });
}

std::string Surge::AuthenticationService::GenerateAuthHeadersFor(const std::string &method) {
    std::string result;

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this, method, &result](auto authenticator) {
                    auto newHeaders = authenticator->GenerateAuthHeadersFor(url, method, username, password);

                    std::for_each(newHeaders.begin(),
                                    newHeaders.end(),
                                    [this, &result](auto header) {
                                        result += header;
                                        result += "\r\n";
                                    });
                    });

    return result;
}


void Surge::AuthenticationService::ExecuteFirstBytesOnTheWireAuthentication() {
    if (!transport) {
        ERROR("Authenticator error: No transport is set, so could not send any bytes down the wire.");
        return;
    }

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this](auto authenticator) {
                      auto bytesToSend = authenticator->FirstBytesOnTheWireAuthentication(username, password);

                      if (bytesToSend.size() > 0) {
                          transport->ArbitraryDataTransaction(bytesToSend.data(), bytesToSend.size());
                      }
                  });
}

bool Surge::AuthenticationService::UpdateAuthForUnauthorizedError(const RtspResponse *response) {
    bool result = false;

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this, &result, response](auto authenticator) {
                      result |= authenticator->UpdateAuthForUnauthorizedError(response);
                  });

    return result;
}
