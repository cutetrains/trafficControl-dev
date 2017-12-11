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

/*! @file trafficdatamodel.cpp
 * @class TrafficDataModel
 *
 * @brief Implementation of Model/View aspects of TrafficControl.
 *
 * This class defines the model/view aspects for the TrafficControl application.
 *
 * @author Gustaf Brännberg
 *
 * Contact: gustaf_brannberg@hotmail.com
 */
#include <iostream>
#include <QDebug>
#include <QListIterator>
#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QCoreApplication>
#include "../inc/trafficdatamodel.h"
#define TRACK 1/*! @todo: Use const instead for define */
#define TRAIN 2
#define STATION 3
using namespace std;

/*!
 * The constructor for TrafficDataModel defines dataModel objects according to the type of data they will handle.
 */
TrafficDataModel::TrafficDataModel(int type, QObject *parent)
     :QAbstractTableModel(parent)
{
  modelType=type;
  nbrOfRows=0;
  switch (type)
  {
    case TRACK: nbrOfColumns=4; break;//Name, length, start, end,
    case TRAIN: nbrOfColumns=5; break;//Name, Track, Position, Speed, State
    case STATION: nbrOfColumns=2; break;//Name, NbrOfPassengers
  }
}

/*!
 * The method returns the number of rows for the data model. 
 *
 * @param parent Index to dataModel, currently not used
 *
 * @return trafficDataMatrix.size The number of rows for the datamodel.
 */
int TrafficDataModel::rowCount(const QModelIndex & /*parent*/) const
{
  return trafficDataMatrix.size();
}

/*!
 * The method returns the number of columns for the data model. 
 *
 * @param parent Index to dataModel, currently not used
 *
 * @return nbrOfColumns Returns the number of columns for the datamodel.
 */
 int TrafficDataModel::columnCount(const QModelIndex & /*parent*/) const
 {
     return nbrOfColumns;
 }

/*!
 * The method is called when any Train/Track/Station object is modified. It sets the data of the corresponding datamodel.
 *
 * @param row The row that will be modified.
 * @param var The incoming data.
 * @TODO adjust the table width automatically
 */
void TrafficDataModel::onDataChanged(int row, const QVariant &var)
{
	QModelIndex index = this->index(row, 0, QModelIndex());
    this->setData(index, var.toStringList());
}

/*!
 * The method returns data for the dataModel. It is used primarily for the tables themselves.
 *
 * @param index Index to dataModel
 * @param role Item role for the class
 *
 * @return QVariant The data in the model index.
 */
QVariant TrafficDataModel::data(const QModelIndex &index, int role) const
{
  int row = index.row();
  int col = index.column();
  if (!index.isValid()) return QVariant();
  if(row>=trafficDataMatrix.size()) return QVariant();
  if(col>=nbrOfColumns) return QVariant();
  switch(role){
    case Qt::DisplayRole:
    return trafficDataMatrix[row][col];
    break;
  }
  return QVariant();
}

/*!
 * The method returns the flags for the dataModel. It specifies that the models are enabled.
 *
 * @param index Index to dataModel
 *
 * @return flags The flags for the model.
 */
Qt::ItemFlags TrafficDataModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return Qt::ItemIsEnabled;

  return QAbstractTableModel::flags(index) ;
}

/*!
 * The method sets the headers for the dataModel. It specifies that the models are enabled.
 *
 * @param section The column header
 * @param orientation Whether the model is horizontal or vertical
 * @param role Item role for the class
 *
 * @return QVariant The headers for the model.
 */
QVariant TrafficDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    {
    if (TRACK == modelType)
    {
      if (Qt::Horizontal == orientation)
      {
        switch (section)
        {
          case 0:
            return QString("Track name");
          case 1:
            return QString("Length");
          case 2:
            return QString("Start station");
          case 3:
            return QString("End station");
        }
      }
    }
    if (STATION == modelType)
    {
      if (Qt::Horizontal == orientation)
      {
        switch (section)
        {
          case 0:
            return QString("Station Name");
          case 1:
            return QString("Waiting");
        }
      }
    }
    if (TRAIN == modelType)
    {
      if (Qt::Horizontal == orientation)
      {
        switch (section)
        {
          case 0:
            return QString("Train Name");
          case 1:
            return QString("Location");
          case 2:
            return QString("Position");
          case 3:
            return QString("Speed");
          case 4:
            return QString("Status");
        }
      }
    }
  }
  return QVariant();
}

/*!
 * The method inserts new entries into the data model at a specific position.
 * It populates the data model with dots, ".".
 *
 * @param position The position where the rows shall be added.
 * @param rows The number of rows that shall be added.
 * @todo Change the parameter rows to numberOfNewEntries.
 * @param index Index to dataModel.
 *
 * @return result TRUE indicates success, FALSE indicates failure. 
 * @todo Change the boolean variable to int so that more error codes can be returned.
 */
bool TrafficDataModel::insertRows(int position,
                                  int rows,
                                  const QModelIndex &/*index=QModelIndex()*/)
{
  beginInsertRows(QModelIndex(),
                  position,
                  position+rows-1);//CHECK IF ROWS ARE VALID
  QStringList list; //Here, insert cols depending on number of cols
  for(int iii=0;iii<nbrOfColumns; iii++) {
    list << ".";
  }
  for (int row = 0; row < rows; ++row)
  {
    trafficDataMatrix.insert(position, list);
  }
  endInsertRows();
  return true;
} 

/*!
 * The method sets data in the corresponding data matrix and emits a signal
 * when that is done so that the views are updated.
 *
 * @param index Index to dataModel entry that shall be updated
 * @param value The value that shall be updated
 * @param role Item role for the class
 *
 * @return result TRUE indicates success, FALSE indicates failure. 
 * @todo Change the boolean variable to int so that more error codes can be returned.
 * @todo The data inserted is a stringlist (value) for the entire row. Check if
 *       this will create problems in future. In that case,  replace with the
 *       index (row, column).
 */
bool TrafficDataModel::setData(const QModelIndex &index,
                               const QVariant &value,
                               int role)
{
  if (index.isValid() && role == Qt::EditRole)
  {
    trafficDataMatrix.replace(index.row(), value.toStringList());
    emit dataChanged(index, index);
    emit layoutChanged();
    return true;
  }
  return false;
}

TrafficDataModel::~TrafficDataModel()
{
}
