#include "SocketHandler.h"
#include "BasicFDEvent.h"
#include "WaitableEvent.h"
#include "Logging.h"

#define DEFAULT_SOCKET_HANDLER_TIMEOUT_MS 5000

Surge::SocketHandler::SocketHandler():
    m_rtspSocketFD(-1),
    m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS)
{
    
}

Surge::SocketHandler::~SocketHandler() {
    
}

void Surge::SocketHandler::StopRunning() {
    if (!IsRunning()) {
        return;
    }

    m_thread.Stop();
}

int Surge::SocketHandler::RtspTcpOpen(const std::string host, int port) {
    
    return -1;
}

void Surge::SocketHandler::Run() {

    SurgeUtil::BasicFDEvent socketDataReceivable{m_rtspSocketFD,
            SurgeUtil::WaitableEvents::WatchForType::readable
            };
    
    while (true) {
        
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                      &m_thread.StopRequested()
                    },
            m_timeoutMs);

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_thread.StopRequested())) {
            break;
        }

        // ....
        
    }
    INFO("SocketHandler thread finished...");
}
