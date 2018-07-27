//
//  AuthenticationService.cc
//  SurgeCore
//
//  Created by Paul Shields on 27/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "AuthenticationService.h"

Surge::AuthenticationService::AuthenticationService() { }

Surge::AuthenticationService::~AuthenticationService() {
    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [](auto authenticator) {
                      delete authenticator;
                  });
}

void Surge::AuthenticationService::OnConnect(const std::string &username, const std::string &password) {
    currentAuthHeaders.clear();

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this, username, password](auto authenticator) {
                      auto newHeaders = authenticator->OnConnect(username, password);

                      std::for_each(newHeaders.begin(),
                                    newHeaders.end(),
                                    [this](auto header) {
                                        currentAuthHeaders += header.first;
                                        currentAuthHeaders += ": ";
                                        currentAuthHeaders += header.second;
                                        currentAuthHeaders += "\r\n";
                                    });
                  });
}

bool Surge::AuthenticationService::UnauthorizedError(const Response *response) {
    bool result = false;
    currentAuthHeaders.clear();

    std::for_each(authenticators.begin(),
                  authenticators.end(),
                  [this, &result, response](auto authenticator) {
                      auto newHeaders = authenticator->UnauthorizedError(response);

                      if (newHeaders.size() > 0) {
                          result = true;

                          std::for_each(newHeaders.begin(),
                                        newHeaders.end(),
                                        [this](auto header) {
                                            currentAuthHeaders += header.first;
                                            currentAuthHeaders += ": ";
                                            currentAuthHeaders += header.second;
                                            currentAuthHeaders += "\r\n";
                                        });
                      }
                  });

    return false;
}
