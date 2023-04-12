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

#ifndef NETWORKDESIGNER_H
#define NETWORKDESIGNER_H
#include <QDebug>
#include <QObject>
#include <QString>
#include <QProcess>
#include <QFile>
#include <QGeoCoordinate>
#include <QDomDocument>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QtMath>

class NetworkDesigner : public QObject
{
    Q_OBJECT
public:
  explicit NetworkDesigner(QObject *parent = nullptr);
  bool convertKmlToTnm(QString kmlFileName);
  bool importTno(QString tnoFileName);
  bool importTnmOrTnf(QString tnmOrTnfFileName);
  QString TraverseXmlNode(const QDomNode& node);

signals:
  void kmlToTnmConversionDone(QString tnmOutput);
  void tnoFileOpened(QString tnoOutput);

public slots:

private:
  struct TrackListItem{
     QString name;
     QList<QGeoCoordinate> coordinates;
     int length;
  };

  struct StationListItem{
     QString name;
     QGeoCoordinate coordinates;
     bool isJunction;
  };

};

#endif // NETWORKDESIGNER_H
