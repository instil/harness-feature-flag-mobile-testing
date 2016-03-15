// -*-c++-*-
#ifndef __SURGE_H__
#define __SURGE_H__

#include "ErrorDispatcher.h"

namespace Surge {

    void StartErrorDispatcher();

    void CloseErrorDispatcher();

    ErrorDispatcher* GetDispatcher();
    
}

#include "RtspClient.h"

#endif //__SURGE_H__
