// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef Queue_h
#define Queue_h

#include <stdio.h>

#define DEFAULT_QUEUE_SIZE 1000

namespace SurgeUtil {

    /**
     * Simple FIFO circular buffer queue, similar tto std::queue, but written to be fast and thread-safe.
     */
    template <class T>
    class Queue
    {
    public:
        T* stack;
        int allocationSize;
        unsigned int head;
        unsigned int tail;

    public:
        Queue(int allocationSize) {
            this->allocationSize = allocationSize;
            stack = new T[allocationSize];
            head = 0;
            tail = 0;
        };

        Queue() : Queue(DEFAULT_QUEUE_SIZE) { }

        ~Queue() {
            delete[] stack;
        }

    public:
        void push(T item) {
            stack[head] = item;
            head = (head + 1) % allocationSize;
        }

        T pop() {
            T result = stack[tail];
            tail = (tail + 1) % allocationSize;
            return result;
        }

        int size() {
            return head - tail;
        }

        int empty() {
            return size() == 0;
        }
    };
}

#endif /* Queue_h */
