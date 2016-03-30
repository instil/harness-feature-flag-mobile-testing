// -*-c++-*-
#ifndef __DATA_EVENT_QUEUE_H__
#define __DATA_EVENT_QUEUE_H__

#include <functional>
#include <deque>

#include "FireableEvent.h"
#include "Mutex.h"
#include "MutexLocker.h"

namespace SurgeUtil {
    
    template<class T> class DataEventQueue {
    public:
        DataEventQueue() { }
    
        const FireableEvent& GetNonEmptyEvent() const { return m_NonEmpty; };

        void AddItem(T item) {
            MutexLocker lock(m_Mutex);

            bool wasEmpty = m_Queue.empty();
            m_Queue.push_back(item);

            if (wasEmpty) {
                m_NonEmpty.Fire();
            }
        }

        T RemoveItem() {
            MutexLocker lock(m_Mutex);

            T item = m_Queue[0];
            m_Queue.pop_front();

            if (m_Queue.empty()) {
                m_NonEmpty.Reset();
            }

            return item;
        }

        void Flush(std::function<void (T)> func) {
            MutexLocker lock(m_Mutex);

            for (auto it = m_Queue.begin(); it != m_Queue.end(); ++it) {
                func(*it);
            }
        
            m_Queue.clear();
        }

    private:
        std::deque<T> m_Queue;
        Mutex m_Mutex;
        FireableEvent m_NonEmpty;
    };
}

#endif //__DATA_EVENT_QUEUE_H__
