#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QObject>
#include <QString>
#include <QUrl>

class ViewModel : public QObject {
public:
    ViewModel(QString qmlLocation) : qmlLocation(QUrl("qrc:/" + qmlLocation)) { }

public:
    const QUrl qmlLocation;
};

#endif // VIEWMODEL_H
