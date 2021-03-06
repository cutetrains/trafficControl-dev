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

#ifndef TRAFFICDATAMODEL_H
#define TRAFFICDATAMODEL_H
#include <QAbstractTableModel>
#include <QVariant> 
#include <QObject>
#include <QStringList>
class TrafficDataModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const;
  bool insertRows(int position,
                  int rows,
                  const QModelIndex &index=QModelIndex());//rename
  bool setData(const QModelIndex &index,
               const QVariant &value,
               int role=Qt::EditRole);
  TrafficDataModel(int type, QObject *parent);

  ~TrafficDataModel();

public slots:
  void onDataChanged(int,
                     const QVariant &);

private:
  int modelType;//TRAIN, TRACK or STATION, defined in trafficcontrol.cpp
  int nbrOfRows;
  int nbrOfColumns;
  QList<QStringList>trafficDataMatrix;
};

#endif
