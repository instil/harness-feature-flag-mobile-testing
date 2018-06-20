// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef DispatchQueue_h
#define DispatchQueue_h

#include <stdio.h>
#include <queue>

#include "FireableEvent.h"
#include "Thread.h"
#include "StoppableThread.h"
#include "Queue.h"


namespace Surge {
    class DispatchQueue : private SurgeUtil::Runnable {
    public:
        DispatchQueue();
        ~DispatchQueue();

        void Dispatch(const std::function<void()>);

        void Start() {
            if (!dispatchThread.IsRunning()) {
                dispatchThread.Execute(*this);
            }
        }

        void Stop() {
            if (dispatchThread.IsRunning()) {
                dispatchThread.Stop();
            }
        }

    private:
        void Run() override;

        std::function<void()> popTaskFromQueue() {
            return taskQueue.pop();
        }

        void pushTaskToQueue(std::function<void()> task) {
            taskQueue.push(task);
        }

        bool taskQueueIsNotEmpty() {
            return !taskQueue.empty();
        }

    private:
        SurgeUtil::Queue<std::function<void()>> taskQueue;

        SurgeUtil::FireableEvent taskAvailableEvent;
        SurgeUtil::StoppableThread dispatchThread;    };
}

#endif /* DispatchQueue_h */
