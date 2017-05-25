// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "Mutex.h"

#include <sstream>


void SurgeUtil::Mutex::Lock()
{
    try
    {
        m_mutex.lock();
    }
    catch (std::system_error e)
    {
        std::ostringstream message;
        message << "Mutex::Lock(): failed to lock: "
                << "error code = " << e.code();
        throw std::runtime_error{message.str()};
    }
}

void SurgeUtil::Mutex::Unlock()
{
    try
    {
        m_mutex.unlock();
    }
    catch (std::system_error e)
    {
        std::ostringstream message;
        message << "Mutex::Unlock(): failed to unlock: "
                << "error code = " << e.code();
        throw std::runtime_error{message.str()};
    }
}

