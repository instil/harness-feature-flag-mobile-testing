// -*-c++-*-
#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "SocketHandlerDelegate.h"

namespace Surge {

    class Transport {
    public:
        Transport(SocketHandlerDelegate *delegate) : m_delegate(delegate) { }

        virtual ~Transport() { }
        
    protected:
        void NotifyDelegateOfReadFailure() {
            if (m_delegate != nullptr) {
                m_delegate->SocketReadFailed();
            }
        }

        void NotifyDelegateOfAnnounce() {
            if (m_delegate != nullptr) {
                m_delegate->AnnounceReceived();
            }
        }

        void NotifyDelegateOfRedirect() {
            if (m_delegate != nullptr) {
                m_delegate->RedirectReceived();
            }
        }

        SocketHandlerDelegate *m_delegate;
    };
    
}

#endif //__TRANSPORT_H__
