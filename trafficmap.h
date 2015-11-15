#ifndef TRAFFICMAP_H
#define TRAFFICMAP_H

#include <QObject>
#include <QMutexLocker>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class TrafficMap : public QObject
{
    Q_OBJECT

public:
    explicit TrafficMap(QObject *parent = 0);
    void setGraphicsScene(QGraphicsScene &mapScene);
    ~TrafficMap();
    void moveTraindXdY(int dx, int dy);

signals:

public slots:

private:
    //QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;

};

#endif // TRAFFICMAP_H
