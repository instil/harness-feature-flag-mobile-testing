#include "Dispatcher.h"

#include <QApplication>
#include <QThread>
#include <QTimer>

Dispatcher::Dispatcher() { }
Dispatcher::~Dispatcher() { }

void Dispatcher::DispatchToThread(QObject *qObject, const std::function<void()> task) {
    if (AlreadyOnRequestedThread(qObject)) {
        task();
        return;
    }

    QMetaObject::invokeMethod(qObject, task, Qt::BlockingQueuedConnection);
}

void Dispatcher::DispatchToMainThread(const std::function<void()> task) {
    DispatchToThread(qApp, task);
}

bool Dispatcher::AlreadyOnRequestedThread(const QObject *qObject) {
    return QThread::currentThread() == qObject->thread();
}
