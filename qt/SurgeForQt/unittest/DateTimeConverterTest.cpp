#include "DateTimeConverterTest.h"

#include <QDateTime>

#include <DateTime.h>

void DateTimeConverterTest::ShouldConvertToSurgeCoreDateTime() {
    auto inputDateTime = QDateTime::fromString("2021-07-05 13:14:59Z", "yyyy-MM-dd HH:mm:ssZ");
    inputDateTime.setTimeSpec(Qt::UTC);

    auto result = DateTimeConverter::ToSurgeCoreDateTime(&inputDateTime);

    AssertSurgeDateTimeIsCorrect(result, 2021, 7, 5, 13, 14, 59);
}

void DateTimeConverterTest::ShouldConvertNonUtcToSurgeCoreDateTime() {
    auto utcPlusOne = QTimeZone(3600);
    auto inputDateTime = QDateTime::fromString("2021-07-05 13:14:59Z", "yyyy-MM-dd HH:mm:ssZ");
    inputDateTime.setTimeZone(QTimeZone(3600));

    auto result = DateTimeConverter::ToSurgeCoreDateTime(&inputDateTime);

    AssertSurgeDateTimeIsCorrect(result, 2021, 7, 5, 12, 14, 59);
}

void DateTimeConverterTest::ShouldHandleInputDateIsNull() {
    auto result = DateTimeConverter::ToSurgeCoreDateTime(nullptr);

    QCOMPARE(result, nullptr);
}

void DateTimeConverterTest::AssertSurgeDateTimeIsCorrect(SurgeUtil::DateTime *surgeDateTime, int year, int month, int day, int hour, int minute, int second) {
    QCOMPARE(surgeDateTime->Year, year);
    QCOMPARE(surgeDateTime->Month, month);
    QCOMPARE(surgeDateTime->Day, day);
    QCOMPARE(surgeDateTime->Hour, hour);
    QCOMPARE(surgeDateTime->Minute, minute);
    QCOMPARE(surgeDateTime->Second, second);
}
