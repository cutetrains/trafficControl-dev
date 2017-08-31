// THIS FILE WILL BE REPLACED WITH GOOGLE TEST.
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
  //   Station - Initiating
  void station_noPassengersAfterCreated(); /* Verify that the station has no
                                       passengers after created */
  void station_noTrainsAfterCreated(); /* Verify that the station has no trains
                                       after created */
  void station_noCoordinatesAtCreation(); /* Will station report no coordinates
                                       if station is created without
                                       coordinates*/
  void staion_validCoordinatesAtCreation(); /* Will station add coordinates and
                                       set "hasCoordinates=TRUE" */
  void staion_validNegativeCoordinatesAtCreation(); /* Verify that station will
                                       add negative coordinates and set
                                       "hasCoordinates=TRUE"*/
  void station_nonNumericalCoordinatesAtCreation(); /* Verify that station will
                                       ignore invalid coordinates at creation*/
  void station_invalidFloatCoordinatesAtCreation(); /* Verify that station will
                                       ignore invalid coordinates at creation,
                                       for example 95 N, 100W*/
/*void station_nameAlreadyTaken(); // Verify that the station will rename new
                                      station with a random suffix if the
                                      proposed name is already taken */
/*void station_invalidName(); // Verify that the station names will contain onl
                                 [a-ö,A-Ö,0-9,-,_] */
/*void station_verifyName();// Verify that the name is copied correctly */
  void station_leavingTrackListEmptyAtCreation(); /* Verify that station has no
                                       defined leaving tracks at creation */
// Station - Performance tests
/*void station_timeForAdding100Stations(); // Observe the time needed for
                                       adding 100 stations */
  // Station - Simple operations
  void station_junctionHasNoPassengersAfterAddingTen(); /* Verify that a
                                       Junction cannot accept any waiting
                                       passengers *//*void station_verifyTrainListAfterAddingRemoving(); // Verify that the
                                       trainList is populated and depopulated
                                       properly */
/*void station_noMoreTrainsThanPlatforms();*/
  void station_tenPassengersAfterAddingTen(); /* Verify that the station can
                                       add passengers properly*/
  
  void station_noPassengersAfterAddingTenRemovingTwenty(); /* Verify that the
                                       station can add and remove passengers
                                       properly*/

									   
									   
									   
									   
  // Train Tests
  //   Train -  initiating
/*void train_noCurrentStationAfterCreation();// Verify that the train has no
                                       current station after creation */
/*void train_noCurrentTrackAfterCreation();// Verify that the train has no
                                       current track after creation */
/*void train_travelPlanEmptyAfterCreation();// Verify that the travel plan is
                                       empty after creation */
/*void train_travelPlanIndexZeroAfterCreation(); // Verify that the travel plan
                                       index is zero after creation */
/*void train_currentSpeedIsZeroAfterCreation(); // Verify that the speed of the
                                       train is zero after creation */
/*void train_noNextTrackDefinedAfterCreation(); // Verify that the next track
                                       isn't defined after creation */
/*void train_noCoordinatesDefinedAfterCreation(); // Verify that the train has
                                       no coordinates bool set after creation*/
/*void train_waitingStateAfterCreation();// Verify that the train is WAITING
                                       after creation */
/*void train_noCoordinatesAfterCreation();// Verify that the train has no
                                       coordinates after creation */
  //   Train - Setting train type. NEEDS A DATA SET FOR DIFFERENT TRAIN TYPES
  //                               CONSIDER STATIC TABLES!


  //    Train - Setting current station
/*void train_setNonExistingStation();// Verify that an error code is sent
                                       when an invalid station is sent.
                                       HOW TO HANDLE INVALID STATIONS HERE? */
/*void train_setFullStation(); // Verify that an error code is sent when a full
                                       station is sent.
                                       HOW TO HANDLE INVALID STATIONS HERE? */
/*void train_setValidStation(); // Verify that the train can be assigned to an
                                   existing available station */
  //    Train - Setting model
  //    Train - Modifying travel plans
/*void train_travelPlanAddValidStations();// Verify that the train can add
                                       stations to a travelplan and that the
                                       travelplan is correct. */
/*void train_travelPlanAddNonAdjacentStations();// Verify that the train can't
                                       add stations to a travelplan and that
                                       travelplan is correct. (LATER TO BE
                                       UPDATED)*/
  // Network - Station/Track tests
/*void network_stationAddingValidLeavingTracks(); // Verify that list of
                                       leavingTracks is consistent after adding
                                       some valid (adjacent) tracks.*/
/*void network_stationNotAddingNonexistentLeavingTracks(); // Verify that station
                                       won't add non-existing leaving tracks and
                                       returns an error message.*/
/*void network_stationNotAddingNonAdjacentLeavingTracks(); // Verify that station
                                       won't add non-adjacent leaving tracks and
                                       returns an error message.*/
/*void network_stationPlatformAssumption(); //Verify that the station will
                                       assume numberOfPlatforms = "number of
                                       leaving tracks" */
/*void network_station_checkIfTrackLeavesStation_badID(); // Verify that
                                       station will return false if a non-
                                       existing track is specified */
  //   Traffic -
/*void traffic_coordinatesForTrainAfterSettingStation(); // Verify that train
                                       has same coordinates as station after
                                       setting current station which has
                                       coordinates*/
/*void traffic_noCoordinatesForTrainAfterSettingNoCoordinateStation();
                                       // Verify that train has no coordinates
                                       after setting current station which has
                                       no coordinates. */
/*void traffic_station_trainDeparture();// */
/*void traffic_station_trainDeparture_trainNotAtStation();// */
/*void traffic_station_trainDeparture_trainNotReady();// */
/*void traffic_station_trainDeparture_trainNotExist();// */
//   Traffic - Train operation tests
/**/

//void oneTrainInTrainListModelAfterAddingATrain();
  //void cleanupTestCase();
};

TstTraffic::TstTraffic() {}

TstTraffic::~TstTraffic() {}

//void TstTraffic::initTestCase() {}

//void TstTraffic::cleanupTestCase() {}

void TstTraffic::station_noPassengersAfterCreated()// Ported to Google Test
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

void TstTraffic::station_noTrainsAfterCreated()// Ported to Google Test
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

void TstTraffic::station_noCoordinatesAtCreation()// Ported to Google Test
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

void TstTraffic::staion_validCoordinatesAtCreation()// Ported to Google Test
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

void TstTraffic::staion_validNegativeCoordinatesAtCreation()// Ported to Google Test
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

void TstTraffic::station_nonNumericalCoordinatesAtCreation() // Ported to Google Test
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


void TstTraffic::station_invalidFloatCoordinatesAtCreation() //Ported to Google Test
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

void TstTraffic::station_leavingTrackListEmptyAtCreation() //Ported to Google Test
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  Station* newStation = new Station("testStation",
                                  false,
                                  "55.7272962532263",
                                  "13.16723731495294",
                                  trackList,
                                  trainList,
                                  stationList);
  QVERIFY( 0 == newStation->getLeavingTrackList().length());
  delete newStation;
}

void TstTraffic::station_junctionHasNoPassengersAfterAddingTen()//Ported to Google Test
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

void TstTraffic::station_tenPassengersAfterAddingTen()//Ported to Google Test
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

void TstTraffic::station_noPassengersAfterAddingTenRemovingTwenty()//Ported to Google Test
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

/*void TstTraffic::oneTrainInTrainListModelAfterAddingATrain() //Ported to Google Test
{
  TrafficControl tc;
  tc.addTrainToNetwork("Train1");
  QVERIFY(tc.getNumberOfTrains() ==1);
    tc.addTrainToNetwork("Train2");
  QVERIFY(tc.getNumberOfTrains() ==2);
}*/

QTEST_MAIN(TstTraffic)

#include "tst_traffic.moc"
