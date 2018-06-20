// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "DispatchQueue.h"
#include "Logging.h"

using SurgeUtil::WaitableEvents::WaitFor;       // From "FireableEvent.h".
using SurgeUtil::WaitableEvents::IsContainedIn; // From "FireableEvent.h".

Surge::DispatchQueue::DispatchQueue() { }

Surge::DispatchQueue::~DispatchQueue() { }

void Surge::DispatchQueue::Dispatch(const std::function<void()> task) {
    pushTaskToQueue(task);
    if (!taskAvailableEvent.IsFired()) {
        taskAvailableEvent.Fire();
    }
}

void Surge::DispatchQueue::Run() {
    while (true) {
        auto firedEvents = WaitFor({
            dispatchThread.StopRequested(),
            &taskAvailableEvent
        }, 1000);

        if (IsContainedIn(firedEvents, *(dispatchThread.StopRequested()))) {
            INFO("Closing DispatchQueue");
            break;
        }

        while (taskQueueIsNotEmpty()) {
            auto task = popTaskFromQueue();
            task();
            taskAvailableEvent.Reset();
        }
    }
}
