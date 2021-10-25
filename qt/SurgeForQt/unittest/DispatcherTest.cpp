#include "DispatcherTest.h"

#include <qthread.h>
#include <QDebug>
#include <QtTest>

void DispatcherTest::initTestCase() {
    backgroundThread = new QThread();
    backgroundObject = new QObject();
    backgroundObject->moveToThread(backgroundThread);
    backgroundThread->start();
}

void DispatcherTest::cleanupTestCase() {
    backgroundThread->quit();

    delete backgroundObject;
    connect(backgroundThread, &QThread::finished, backgroundThread, &QTimer::deleteLater);
}


void DispatcherTest::ShouldRunOnThread() {
    target.DispatchToThread(backgroundObject, [this] {
        AssertCurrentlyOnThread(backgroundThread);
    });
}

void DispatcherTest::ShouldAvoidDeadlockWhenRunningOnThread() {
    QMetaObject::invokeMethod(backgroundObject, [this] {
        target.DispatchToThread(backgroundObject, [this] {
            AssertCurrentlyOnThread(backgroundThread);
        });
    }, Qt::BlockingQueuedConnection);
}

void DispatcherTest::ShouldRunOnMainThread() {
    QMetaObject::invokeMethod(backgroundObject, [this] {
        target.DispatchToMainThread([this] {
            AssertCurrentlyOnThread(qApp);
        });
    }, Qt::QueuedConnection);
}

void DispatcherTest::ShouldAvoidDeadlockWhenRunningOnMainThread() {
    target.DispatchToMainThread([this] {
        AssertCurrentlyOnThread(qApp);
    });
}

void DispatcherTest::AssertCurrentlyOnThread(const QThread *thread) {
    QCOMPARE(thread, QThread::currentThread());
}

void DispatcherTest::AssertCurrentlyOnThread(const QObject *threadObject) {
    QCOMPARE(threadObject->thread(), QThread::currentThread());
}
