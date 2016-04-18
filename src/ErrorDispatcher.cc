// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ErrorDispatcher.h"
#include "RtspClient.h"
#include "DelegateInterface.h"
#include "Logging.h"

#include "FireableEvent.h"
  using SurgeUtil::WaitableEvents::WaitFor;       // From included "FireableEvent.h".
  using SurgeUtil::WaitableEvents::IsContainedIn; // From included "FireableEvent.h".

#define ERROR_DISPATCHER_CLIENT_RECEIVED_TIMEOUT_MILLISECONDS 1000

namespace Surge {

    ErrorDispatcher::ErrorDispatcher(): m_timeout(ERROR_DISPATCHER_CLIENT_RECEIVED_TIMEOUT_MILLISECONDS) { }

    void ErrorDispatcher::FailureForClient(const RtspClient * client, ERROR_TYPE type)
    {
        if (!IsRunning())
            return;
    
        m_firedEventLock.Lock();

        // reset received event
        m_receivedStopEvent.Reset();
    
        // fire stop event with associated data
        m_clientStopEvent.SetAssociated((void *)client);
        m_clientStopEvent.SetInt(type);
        m_clientStopEvent.Fire();
    
        // wait for it to be picked up
        WaitForErrorReceived(ERROR_DISPATCHER_CLIENT_RECEIVED_TIMEOUT_MILLISECONDS);
    
        m_firedEventLock.Unlock();
    }

    void ErrorDispatcher::WaitForErrorReceived(const long timeout_milliseconds)
    {
        WaitFor({&m_receivedStopEvent}, timeout_milliseconds);
    }

    void ErrorDispatcher::Run()
    {
        INFO("Starting error dispatcher..");
        while (true)
        {
            auto firedEvents = WaitFor({
                    m_thread.StopRequested(),
                    &m_clientStopEvent
                }, m_timeout);
        
            if (IsContainedIn(firedEvents, *(m_thread.StopRequested())))
            {
                break;
            }

            if (IsContainedIn(firedEvents, m_clientStopEvent)) {
                const Surge::RtspClient * client = (Surge::RtspClient*)m_clientStopEvent.GetAssociated();
                ERROR_TYPE type = (ERROR_TYPE)m_clientStopEvent.GetInt();
                m_clientStopEvent.Reset();
                m_receivedStopEvent.Fire();

                Surge::RtspClientDelegate *delegate = client->GetDelegate();

                switch(type) {
                case LOST_CONNECTION:
                    delegate->ClientDidTimeout();
                    break;

                case ANNOUNCE:
                    delegate->StreamConfigChanged(false);
                    break;

                case REDIRECT:
                    delegate->StreamConfigChanged(true);
                    break;

                default:
                    FATAL("Unhandled error event for client");
                    break;
                }
            }
        }
        INFO("Error dispatcher stopped..");
    }

}
