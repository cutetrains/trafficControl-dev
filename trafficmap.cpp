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
    QBrush greenBrush(Qt::green);
    QBrush blueBrush(Qt::blue);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);
    rectangle = mapScene.addRect(100, 0, 80, 100, outlinePen, blueBrush);

    // addEllipse(x,y,w,h,pen,brush)
    ellipse = mapScene.addEllipse(0, -100, 300, 60, outlinePen, greenBrush);

    text = mapScene.addText("bogotobogo.com", QFont("Arial", 20) );
}


TrafficMap::~TrafficMap()
{
    qDebug()<<"Destroyed TrafficMap object!";
}
