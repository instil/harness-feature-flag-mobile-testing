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
        DataEventQueue(): m_maxSize(-1), m_elementDeallocator(0) { }

        DataEventQueue(int maxSize, std::function<void (T)> deallocator):
            m_maxSize(maxSize),
            m_elementDeallocator(deallocator)
            {}

        ~DataEventQueue() {
            if (m_elementDeallocator != 0) {
                Flush(m_elementDeallocator);
            }
            m_Queue.clear();
        }

        int Size() const { return m_Queue.size(); }
    
        bool IsEmpty() const { return Size() <= 0; }
    
        const FireableEvent& GetNonEmptyEvent() const { return m_NonEmpty; };

        void AddItem(T item) {
            MutexLocker lock(m_Mutex);

            if (IsAtSizeLimit()) {
                T item = m_Queue[0];
                m_Queue.pop_front();

                if (m_elementDeallocator != 0) {
                    m_elementDeallocator(item);
                }
            }
            
            bool wasEmpty = IsEmpty();
            m_Queue.push_back(item);

            if (wasEmpty) {
                m_NonEmpty.Fire();
            }
        }

        T RemoveItem() {
            MutexLocker lock(m_Mutex);

            T item = m_Queue[0];
            m_Queue.pop_front();

            if (IsEmpty()) {
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

        void Flush() {
            if (m_elementDeallocator != 0) {
                Flush(m_elementDeallocator);
            }
        }

    private:

        bool IsAtSizeLimit() const {
            if (m_maxSize <= 0) {
                return false;
            }
            return Size() >= m_maxSize;
        }
        
        int m_maxSize;
        std::function<void (T)> m_elementDeallocator;
        std::deque<T> m_Queue;
        Mutex m_Mutex;
        FireableEvent m_NonEmpty;
    };
}

#endif //__DATA_EVENT_QUEUE_H__
