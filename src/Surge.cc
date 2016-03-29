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
