#include "SocketHandler.h"
#include "BasicFDEvent.h"
#include "WaitableEvent.h"
#include "Logging.h"
#include "Helpers.h"

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

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
    m_rtspSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_rtspSocketFD == -1) {
        ERROR("failed to create socket: error code = " << errno);
        return -1;
    }

    bool non_blocking_failed = SurgeUtil::SetNonBlocking(m_rtspSocketFD) == -1;
    if (non_blocking_failed) {
        ERROR("Failed to put socket into non-blocking mode: error code = " << errno);
        close(m_rtspSocketFD);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) == -1) {
        ERROR("inet_pton");
        close(m_rtspSocketFD);
        return -1;
    } 

    if (connect(m_rtspSocketFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        
        SurgeUtil::BasicFDEvent attemptCompleted{m_rtspSocketFD,
                SurgeUtil::WaitableEvents::WatchForType::writable
                };
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&attemptCompleted}, m_timeoutMs);

        if (firedEvents.empty()) {
            ERROR("socket connection attempt timed out");
            close(m_rtspSocketFD);
            return -1;
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, attemptCompleted))
        {
            // Connection attempt completed -  Check whether successful or not.
            int error_code;
            socklen_t optionLength = sizeof(error_code);
            if (getsockopt(m_rtspSocketFD, SOL_SOCKET, SO_ERROR, &error_code, &optionLength) != 0) {
                ERROR("failed to determine result of socket connection attempt: error code = " << errno);
                close(m_rtspSocketFD);
                return -1;
            }
            if (error_code != 0)
            {
                ERROR("socket connection attempt failed: error code =  " << error_code);
                close(m_rtspSocketFD);
                return -1;
            }
        }
    }
    INFO("Connection to: " << host << ":" << port << " - Open");
    
    return 0;
}

void Surge::SocketHandler::Run() {

    SurgeUtil::BasicFDEvent socketDataReceivable {
        m_rtspSocketFD, SurgeUtil::WaitableEvents::WatchForType::readable };
    
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
