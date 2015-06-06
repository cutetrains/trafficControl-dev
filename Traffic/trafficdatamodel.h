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
     TrafficDataModel(int type, QObject *parent);
     int rowCount(const QModelIndex &parent = QModelIndex()) const ;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;
     QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;//ADD SETDATA
     QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
     bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());//rename
     bool setData(const QModelIndex &index, const QVariant &value, 
		int role=Qt::EditRole);
     Qt::ItemFlags flags(const QModelIndex &index) const;
     ~TrafficDataModel();

 public slots:
	void onDataChanged(int , const QVariant &); 

 private:
	int modelType;//TRAIN, TRACK or STATION, defined in trafficcontrol.cpp
	int nbrOfRows;
	int nbrOfColumns;
	QList<QStringList>trafficDataMatrix; 
 };

#endif
