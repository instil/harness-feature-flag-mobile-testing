#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>

class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher();

    void DispatchToThread(QObject *qObject, const std::function<void()> task);
    void DispatchToMainThread(const std::function<void()> task);

private:
    bool AlreadyOnRequestedThread(const QObject *qObject);
};

#endif // DISPATCHER_H
