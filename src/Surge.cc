#include "Surge.h"

namespace Surge {

    void StartErrorDispatcher() {
        __dispatcher = new ErrorDispatcher();
        __dispatcher->StartRunning();
    }

    void CloseErrorDispatcher() {
        if (__dispatcher != nullptr) {
            __dispatcher->StopRunning();
        }
    }

    ErrorDispatcher* GetDispatcher() {
        return __dispatcher;
    }
    
}
