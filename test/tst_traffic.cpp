#include <QtTest>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "inc/trafficcontrol.h"

class TstTraffic : public QMainWindow
{
  Q_OBJECT

public:
  TstTraffic();
  ~TstTraffic();

private slots:
  //void initTestCase();
  // Station-only Tests
  //   Initiating Station
  void station_noPassengersAfterCreated(); /* Verify that the station has no
                                       passengers after created */
  void station_noTrainsAfterCreated(); /* Verify that the station has no trains
                                       after created */
  void station_noCoordinatesAtCreation(); /* Will station report no coordinates
                                       if station is created without
                                       coordinates*/
  void staion_validCoordinatesAtCreation(); /* Will station add coordinates and
                                       set "hasCoordinates=TRUE"*/
  void staion_validNegativeCoordinatesAtCreation(); /* Verify that station will
                                       add negative coordinates and set
                                       "hasCoordinates=TRUE"*/
  void station_nonNumericalCoordinatesAtCreation(); /* Verify that station will
                                       ignore invalid coordinates at creation*/
  void station_invalidFloatCoordinatesAtCreation(); /* Verify that station will
                                       ignore invalid coordinates at creation,
                                       for example 95 N, 100W*/
/*void station_nameAlreadyTaken(); // Verify that the station will not create if
                                      the station name has already been used */
/*void station_invalidName();*/
/*void station_validName();*/
/*void station_leavingTrackListEmptyAtCreation(); // Verify that station has no
                                       defined leaving tracks at creation */
  // Performance tests
/*void station_timeForAdding100Stations(); // Observe the time needed for
                                       adding 100 stations */
  // Simple operations
  void station_junctionHasNoPassengersAfterAddingTen(); /* Verify that a
                                       Junction cannot accept any waiting
                                       passengers */
/*void station_verifyTrainListAfterAddingRemoving(); // Verify that the
                                       trainList is populated and depopulated
                                       properly */
/*void station_noMoreTrainsThanPlatforms();*/
  void station_tenPassengersAfterAddingTen(); /* Verify that the station can
                                       add passengers properly*/
  void station_noPassengersAfterAddingTenRemovingTwenty(); /* Verify that the
                                       station can add and remove passengers
                                       properly*/


  // Train Tests
//void oneTrainInTrainListModelAfterAddingATrain();
  //void cleanupTestCase();
};

TstTraffic::TstTraffic() {}

TstTraffic::~TstTraffic() {}

//void TstTraffic::initTestCase() {}

//void TstTraffic::cleanupTestCase() {}

void TstTraffic::station_noPassengersAfterCreated()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY(0 == newStation->getNbrOfWaitingPassengers());
  delete newStation;
}

void TstTraffic::station_noTrainsAfterCreated()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY(0 == newStation->getTrainList().length());
  delete newStation;
}

void TstTraffic::station_noCoordinatesAtCreation()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY(false == newStation->hasCoordinates());
  delete newStation;
}

void TstTraffic::staion_validCoordinatesAtCreation()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QString latitude = "55.72729652532263";
  QString longitude = "13.16723731495294";
  Station* newStation = new Station("testStation",
                                  false,
                                  latitude,
                                  longitude,
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY(true == newStation->hasCoordinates() &&
          latitude.toFloat() == newStation->getLatitude() &&
          longitude.toFloat() == newStation->getLongitude() );
  delete newStation;
}

void TstTraffic::staion_validNegativeCoordinatesAtCreation()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QString latitude = "-55.72729652532263";
  QString longitude = "-13.16723731495294";
  Station* newStation = new Station("testStation",
                                  false,
                                  latitude,
                                  longitude,
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY(true == newStation->hasCoordinates() &&
          latitude.toFloat() == newStation->getLatitude() &&
          longitude.toFloat() == newStation->getLongitude() );
  delete newStation;
}

void TstTraffic::station_nonNumericalCoordinatesAtCreation()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "55.727hej52532263",
                                  "13.16723731495294",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY2(false == newStation->hasCoordinates(),
          "Added invalid coordinates: First coordinate is not a numerical value.");
  delete newStation;
}


void TstTraffic::station_invalidFloatCoordinatesAtCreation()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "105.7272962532263",
                                  "13.16723731495294",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY2(false == newStation->hasCoordinates(),
          "Added invalid coordinates: First coordinate is more than 90.");
  delete newStation;
}

void TstTraffic::station_junctionHasNoPassengersAfterAddingTen()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  true,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  newStation->changeNbrOfPassengers(10);
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 0);
  delete newStation;
}

void TstTraffic::station_tenPassengersAfterAddingTen()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QVERIFY(0 == trackList.length());
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  newStation->changeNbrOfPassengers(10);
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 10);
}

void TstTraffic::station_noPassengersAfterAddingTenRemovingTwenty()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QVERIFY(0 == trackList.length());
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);

  newStation->changeNbrOfPassengers(10);
  newStation->changeNbrOfPassengers(-20);
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 0);
}

/*void TstTraffic::oneTrainInTrainListModelAfterAddingATrain()
{
  TrafficControl tc;
  tc.addTrainToNetwork("Train1");
  QVERIFY(tc.getNumberOfTrains() ==1);
    tc.addTrainToNetwork("Train2");
  QVERIFY(tc.getNumberOfTrains() ==2);
}*/

QTEST_MAIN(TstTraffic)

#include "tst_traffic.moc"
