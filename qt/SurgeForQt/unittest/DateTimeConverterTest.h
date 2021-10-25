#ifndef DATETIMECONVERTERTEST_H
#define DATETIMECONVERTERTEST_H

#include <QtTest>
#include <TestSuite.h>
#include <DateTimeConverter.h>

class DateTimeConverterTest : public TestSuite {
    Q_OBJECT

private slots:
    void ShouldConvertToSurgeCoreDateTime();
    void ShouldConvertNonUtcToSurgeCoreDateTime();
    void ShouldHandleInputDateIsNull();

private:
    void AssertSurgeDateTimeIsCorrect(SurgeUtil::DateTime *surgeDateTime, int year, int month, int day, int hour, int minute, int second);
};

static DateTimeConverterTest TEST_DATETIMECONVERTER;

#endif // DATETIMECONVERTERTEST_H
