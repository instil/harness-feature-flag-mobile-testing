#ifndef DATETIMECONVERTER_H
#define DATETIMECONVERTER_H

#include <QDateTime>
#include <SurgeCoreDeclarations.h>

class DateTimeConverter
{
private:
    DateTimeConverter();

public:
    static SurgeUtil::DateTime * ToSurgeCoreDateTime(QDateTime *dateTime);
};

#endif // DATETIMECONVERTER_H
