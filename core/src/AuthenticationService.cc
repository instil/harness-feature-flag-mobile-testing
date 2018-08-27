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

void Surge::AuthenticationService::GenerateAuthHeaders() {
    currentAuthHeaders.clear();

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this](auto authenticator) {
                      auto newHeaders = authenticator->AuthenticationHeaders(username, password);

                      std::for_each(newHeaders.begin(),
                                    newHeaders.end(),
                                    [this](auto header) {
                                        currentAuthHeaders += header;
                                        currentAuthHeaders += "\r\n";
                                    });
                  });
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
