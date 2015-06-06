#include<vector>
#include<QObject>
#ifndef _TCSTATION_H_
#define _TCSTATION_H_
using namespace std;
class Track;
class Train;
class Station: public QObject
{
	Q_OBJECT

signals:
	void dataChangedSignal(int , const QVariant & ); 

private:
	static int totalNbrOfStations;
  	int waitingPassengers;
  	QString name;
	int stationID;
	vector<int> leavingTrackVector;
    QList<Track*> *thisTrackList;//inserted, REMOVE?
    QList<Train*> *thisTrainList;//inserted
    QList<Station*> *thisStationList;//inserted
public:// to do later: Add vector with pointers to the trains on each station.
  	/*! The constructor for a Station object.*/
  	Station(QString cn,QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList);
    ~Station();
	bool checkIfTrackLeavesStation(int trackID);
    int findLeavingTrackIndexToStation(int targetStationID);
	void addTrack(int trackID); //To be handled in trafficcontrol
  	void changeNbrOfPassengers(int n);
  	int getID();
	QString getName();
  	int getNbrOfWaitingPassengers();
	void sendDataChangedSignal(int stationID);
  	void showInfo();
};

#endif
