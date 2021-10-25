#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QtTest>
#include <typeinfo>

#define QTHING(T)(statement) \
do {\
    return;\
} while (false)

template <typename TInstance, typename TRequired>
void IsInstanceOf(TInstance object) {
    if (dynamic_cast<TRequired>(object) == nullptr) {
        QFAIL("Object was not of type.");
    }
}

#endif // TESTUTILS_H
