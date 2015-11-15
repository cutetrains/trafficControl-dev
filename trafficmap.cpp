/*************************************************************************
 * This file is part of TrafficControl.
 *
 * TrafficControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TrafficControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TrafficControl.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#include "trafficmap.h"
#include <QDebug>

TrafficMap::TrafficMap(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"Created TrafficMap object!";

}

//Should this be in constructor instead?
void TrafficMap::setGraphicsScene(QGraphicsScene &mapScene)
{
    qDebug()<<"Set Graphics Scene";
    QBrush greenBrush(QColor(0,153,0));
    QBrush blueBrush(Qt::blue);
    QBrush redBrush(Qt::red);
    QBrush blackBrush(Qt::black);
    QBrush yellowBrush(Qt::yellow);

    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    QPen thinPen(QColor(64,64,64));
    thinPen.setWidth(1);

    QPen stationPen(Qt::blue);
    stationPen.setWidth(2);


    QGraphicsEllipseItem *stationLundC = mapScene.addEllipse(200-20/2,100-20/2,20,20, stationPen);
    QGraphicsEllipseItem *stationStangby = mapScene.addEllipse(500-20/2,-200-20/2,20,20, stationPen);
    QGraphicsEllipseItem *stationGunnesbo = mapScene.addEllipse(100-20/2,-100-20/2,20,20, stationPen);
    QGraphicsEllipseItem *stationHjarup = mapScene.addEllipse(100-20/2,200-20/2,20,20, stationPen);
    outlinePen.setBrush(redBrush);
    QGraphicsLineItem *trackLunStaN = mapScene.addLine(200,100,500,-200, thinPen);
    QGraphicsLineItem *trackLunGunN = mapScene.addLine(200,100,100,-100, outlinePen);
    thinPen.setBrush(greenBrush);
    QGraphicsLineItem *trackHjaLunNN = mapScene.addLine(200,100,100,200, thinPen);
    // addEllipse(x,y,w,h,pen,brush)
    outlinePen.setBrush(blackBrush);
    thinPen.setBrush(blackBrush);
    QGraphicsEllipseItem *trainHedyLamarr = mapScene.addEllipse(200-10/2,100-10/2,10,10,thinPen,yellowBrush);

    trainHedyLamarr->moveBy(-50,-100);


    //ellipse = mapScene.addEllipse(0, -100, 300, 60, outlinePen, greenBrush);

    //rectangle = mapScene.addRect(100, 0, 80, 100, outlinePen, blueBrush);
    //text = mapScene.addText("bogotobogo.com", QFont("Arial", 20) );
    //rotateEllipse(-45);


}

void TrafficMap::moveTraindXdY(int dx, int dy)
{

}

TrafficMap::~TrafficMap()
{
    qDebug()<<"Destroyed TrafficMap object!";
}
