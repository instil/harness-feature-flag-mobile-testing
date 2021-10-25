#ifndef DISPATCHERTEST_H
#define DISPATCHERTEST_H

#include <Dispatcher.h>
#include <TestSuite.h>

class DispatcherTest : public TestSuite {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void ShouldRunOnThread();
    void ShouldAvoidDeadlockWhenRunningOnThread();
    void ShouldRunOnMainThread();
    void ShouldAvoidDeadlockWhenRunningOnMainThread();

private:
    void AssertCurrentlyOnThread(const QThread *thread);
    void AssertCurrentlyOnThread(const QObject *threadObject);

private:
    Dispatcher target;
    QThread *backgroundThread;
    QObject *backgroundObject;
};

static DispatcherTest TEST_DISPATCHER;

#endif // DISPATCHERTEST_H
