// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __SURGE_H__
#define __SURGE_H__

#include "ErrorDispatcher.h"

namespace Surge {

    void StartErrorDispatcher();

    void CloseErrorDispatcher();

    ErrorDispatcher& GetDispatcher();
    
}

#include "RtspClient.h"

#endif //__SURGE_H__
