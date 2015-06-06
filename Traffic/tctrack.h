#include<vector>
#include<QObject>
#ifndef _TCTRACK_H_
#define _TCTRACK_H_
using namespace std;
class Train;
class Station;
class Track: public QObject// to do later: Add vector with indices to the trains on each track.
{
	Q_OBJECT

signals:
	void dataChangedSignal(int , const QVariant & ); 

private:
	static int totalNbrOfTracks;
  	int length;
  	int trainsOnTrack;
  	QString name;
  	int startStation;
  	int endStation;
	int maxAllowedSpeed;
	int trackID;
public:
  	Track(QString cn, int nn,QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList);//ID and length
	Track(const Track& sTrack  );
	Track& operator=( const Track& sTrack );
    ~Track();
  	int getID();
	int getLength();
	int getMaxAllowedSpeed();
  	int getEndStation();
	QString getName();
  	int getStartStation();
	void sendDataChangedSignal(int trackID);
  	void setEndStation( int stationID);//To be handled in trafficcontrol
	void setMaxAllowedSpeed(int n);//to implement
  	void setStartStation( int stationID);//To be handled in trafficcontrol
  	void showInfo();//modify
    QList<Track*> *thisTrackList;//inserted, REMOVE?
    QList<Train*> *thisTrainList;//inserted
    QList<Station*> *thisStationList;//inserted
};
#endif
