#include "DateTimeConverter.h"

#include <DateTime.h>

SurgeUtil::DateTime * DateTimeConverter::ToSurgeCoreDateTime(QDateTime *dateTime) {
    if (dateTime == nullptr) {
        return nullptr;
    }

    auto utcDateTime = dateTime->toUTC();
    auto date = utcDateTime.date();
    auto time = utcDateTime.time();

    return new SurgeUtil::DateTime(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), 0);
}
