// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspLoggingDelegateWrapper.h"
#include <android/log.h>


void SurgeJni::RtspLoggingDelegateWrapper::trace(const char *message) const {
    __android_log_print(ANDROID_LOG_VERBOSE, "Surge", message);
}

void SurgeJni::RtspLoggingDelegateWrapper::debug(const char *message) const {
    __android_log_print(ANDROID_LOG_DEBUG, "Surge", message);
}

void SurgeJni::RtspLoggingDelegateWrapper::info(const char *message) const {
    __android_log_print(ANDROID_LOG_INFO, "Surge", message);
}

void SurgeJni::RtspLoggingDelegateWrapper::warn(const char *message) const {
    __android_log_print(ANDROID_LOG_WARN, "Surge", message);
}

void SurgeJni::RtspLoggingDelegateWrapper::error(const char *message) const {
    __android_log_print(ANDROID_LOG_ERROR, "Surge", message);
}

void SurgeJni::RtspLoggingDelegateWrapper::fatal(const char *message) const {
    __android_log_print(ANDROID_LOG_FATAL, "Surge", message);
}