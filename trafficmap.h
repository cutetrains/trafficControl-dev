#ifndef TRAFFICMAP_H
#define TRAFFICMAP_H

#include <QObject>

class TrafficMap : public QObject
{
    Q_OBJECT

public:
    explicit TrafficMap(QObject *parent = 0);
    ~TrafficMap();
signals:

public slots:

};

#endif // TRAFFICMAP_H
