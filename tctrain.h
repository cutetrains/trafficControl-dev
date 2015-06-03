#include<vector>
#include<QObject>
#include<QHash>
#include <QMutexLocker>
#ifndef _TCTRAIN_H_
#define _TCTRAIN_H_
using namespace std;
class Track;
class Station;

class Train: public QObject
{
	Q_OBJECT

signals:
	void dataChangedSignal(int , const QVariant & ); 

private:
    QString trainName;
	static int totalNbrOfTrains;
  	int nbrOfSeats;
  	int nbrOfPassengers;
  	int currentTrack;//-1 indicates that train is not on a track
	int currentStation;//-1 indicates that train is not on a station
  	int positionOnTrack;//[m]
	int currentSpeed;//[m/s]
	int desiredSpeed;//[m/s]
	int maxSpeed;
	int trainID;
	int nextTrack;
    vector<int> travelPlanByStationID;
    int nextIndexTravelPlanByStationID;
    //int lastIndexTravelPlanByStationID;
    QList<Track*> *thisTrackList;
    QList<Train*> *thisTrainList;
    QList<Station*> *thisStationList;
    QMutex mutex;
    QHash<QString, int> stateTable;
    int state;
    int doorOpenProgress;//Doors needs 5 seconds to open and close. 0=closed, 100=open
    bool passing;

public:
    Train(QString name, QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList);
    ~Train();
	void addStationToTrainRoute(int trackID);//To be removed and replaced by addStationToTrainRoute
    int addStationToTrainRoute(QString stationName, bool stopAtStation);//Future step: add requested arrival and departure time to the entry
	int getCurrentSpeed();
	int getCurrentStation();
  	int getCurrentTrack();
	int getDesiredSpeed();
  	int getID();
    QString getName();
  	int getTrackPosition();
    int getIndexTravelPlanByStationID();
	vector<int> getRouteVectorStations();
	vector<int> getRouteVectorTracks();//To delete
  	void load(int n); 
	int move(int n);//if train shall wait for other elements, return int>0.
	void sendDataChangedSignal(int trainID);
	void setCurrentStation(int StationID);
  	void setCurrentTrack(int trackID);
    void setName(QString const& tn);
    void setModel(QString const& modelID);
    void setDesiredSpeed(int n);
  	void setTrackPosition(int n);
  	void showInfo();

};

#endif
