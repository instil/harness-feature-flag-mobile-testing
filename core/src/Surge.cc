// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "Surge.h"

namespace Surge {

    void StartErrorDispatcher() {
        ErrorDispatcher::GetInstance().StartRunning();
    }

    void CloseErrorDispatcher() {
        ErrorDispatcher::GetInstance().StopRunning();
    }

    ErrorDispatcher& GetDispatcher() {
        return ErrorDispatcher::GetInstance();
    }
    
}
