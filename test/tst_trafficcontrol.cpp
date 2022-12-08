// THIS FILE WILL BE REPLACED WITH GOOGLE TEST.
#include <QTest>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "inc/trafficcontrol.h"
#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

//class TstTrafficControl : public QMainWindow
class TstTrafficControl : public QObject
{
  Q_OBJECT

  private:
    TrafficDataModel *trackListModel;
    TrafficDataModel *trainListModel;
    TrafficDataModel *stationListModel;
    QObject* handleQMLObject;
    NetworkControl *nc;

  private slots:

    void init();
    void cleanup();

    void network_findReversedTrack();
    void network_addOneTrainToStation();
    void network_connectThreeTracksToTwoStations();
    /*void station_invalidName(); // Verify that the station names will contain onl
                                     [a-ö,A-Ö,0-9,-,_] */
    void station_addingRemovingPassengers();
    void station_addInvalidTrackToStation();

    void station_addTrackToStation();
    //void station_assertIfDuplicateTrainAtStation();//  QVERIFY_THROWS_EXCEPTION
    void station_connectBidirectionalTrackToStations();

    void station_connectNonExistingTrackToStations();

    void station_connectTrackToStations();

    void station_emptyAfterCreated(); /* Verify that the station has no trains
                                       after created */
    void station_findTrackToNextStation();
    void station_findTrackToNextStationNegative();
    void station_invalidFloatCoordinatesAtCreation();
    /*void station_invalidName(); // Verify that the station names will contain onl
                                     [a-ö,A-Ö,0-9,-,_] */
    void station_junctionHasNoPassengersAfterAddingTen();
    /*void station_nameAlreadyTaken(); // Verify that the station will reject the
     *                                    command if the proposed name is already taken */
    void station_noMoreTrainsThanPlatforms();
    void station_nonNumericalCoordinatesAtCreation();
    void station_tenPassengersAfterAddingTen(); /* Verify that the station can
                                       add passengers properly*/
    void station_trainListAfterAddingRemoving();
    void station_trainArrivalDeparture();
    void station_validateCheckIfTrackLeavesStation();
    void station_validCoordinatesAtCreation();
    void station_validNegativeCoordinatesAtCreation();
    void station_trainListNonExistingTrain();
    void track_trackLength();
    void track_trackCoordinates();
    void track_trackCoordinatesInvalid();
    void track_startEndStation();
    void track_trainListAddDownstream();

    void traffic_regularTrackTest();
    void traffic_reversedTrackTest();

    void train_nbrOfPassengersAddRemove();
    void train_nbrOfPassengersVsCapacity();
    void train_statusAfterCreation();
    //TODO BELOW
    /*void station_nameAlreadyTaken(); // Verify that the station will rename new
                                          station with a random suffix if the
                                          proposed name is already taken */
    /*void station_invalidName(); // Verify that the station names will contain only
                                     [a-ö,A-Ö,0-9,-,_] */
    /*void station_verifyName();// Verify that the name is copied correctly */

    /*void station_timeForAdding100Stations(); // Observe the time needed for
                                           adding 100 stations */

    //TEST(trafficTest,oneTrainInTrainListModelAfterAddingATrain)
    //{ // Verify that trafficControl can add trains properly
    //  //  NOT FUNCTIONING, NEED TO CREATE QAPPLICATION BEFORE CREATING WIDGET
    //    char  arg0[] = "programName";
    //    char* argv[] = { &arg0[0], NULL };
    //    int   argc   = (int)(sizeof(argv) / sizeof(argv[0])) - 1;
    //  QApplication a(argc,argv);
    //  TrafficControl tc;
    //  tc.show();
    //  tc.addTrainToNetwork("Train1");
    //  EXPECT_THAT(tc.getNumberOfTrains(), Eq(1));
    //  tc.addTrainToNetwork("Train2");
    //  EXPECT_THAT(tc.getNumberOfTrains(), Eq(2));
    //}



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
};

void TstTrafficControl::init()
{
    trackListModel = new TrafficDataModel(TRACK, 0);
    trainListModel = new TrafficDataModel(TRAIN, 0);
    stationListModel =new TrafficDataModel(STATION, 0);
    handleQMLObject = NULL;
    nc = new NetworkControl(*trackListModel,
                            *trainListModel,
                            *stationListModel,
                            *handleQMLObject);
  }

void TstTrafficControl::cleanup(){
  delete trackListModel;
  delete trainListModel;
  delete stationListModel;
  delete handleQMLObject;
  delete nc;
}

void TstTrafficControl::network_addOneTrainToStation()
{
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRAIN trainA");
  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), true);
  QCOMPARE(nc->stationList.at(0)->getTrainList().length(), 1);
  int firstTrainAtStationA = nc->stationList.at(0)->getTrainList().at(0);
  QCOMPARE(nc->trainList.at(firstTrainAtStationA)->getName(), "trainA");
}

void TstTrafficControl::network_connectThreeTracksToTwoStations()
{ /* Verify that station can add leaving Tracks
   * testStation0 <-  track0 <-  testStation1
   * testStation0  -> track1  -> testStation1
   * testStation0  -> track1  -> testStation1
   */

  nc->parseCmd("ADD STATION station0");
  nc->parseCmd("ADD STATION station1");
  nc->parseCmd("ADD TRACK track0 1000");
  nc->parseCmd("ADD TRACK track1 1000");
  nc->parseCmd("ADD TRACK track2 1000");
  nc->parseCmd("CONNECT TRACK track0 FROM station1 TO station0");
  nc->parseCmd("CONNECT TRACK track1 FROM station0 TO station1");
  nc->parseCmd("CONNECT TRACK track2 FROM station1 TO station0");
  QCOMPARE(nc->stationList.at(0)->getLeavingTrackList().length(), 1);
  QCOMPARE(nc->stationList.at(0)->getLeavingTrackList().at(0), 1);
  QCOMPARE(nc->stationList.at(1)->getLeavingTrackList().length(), 2);
  QCOMPARE(nc->stationList.at(1)->getLeavingTrackList().at(0), 0);
  QCOMPARE(nc->stationList.at(1)->getLeavingTrackList().at(1), 2);
  QCOMPARE(nc->trackList.at(0)->getStartStation(), 1);
  QCOMPARE(nc->trackList.at(0)->getEndStation(), 0);
  QCOMPARE(nc->trackList.at(1)->getStartStation(), 0);
  QCOMPARE(nc->trackList.at(1)->getEndStation(), 1);
  QCOMPARE(nc->trackList.at(2)->getStartStation(), 1);
  QCOMPARE(nc->trackList.at(2)->getEndStation(), 0);
}


void TstTrafficControl::network_findReversedTrack()
{ /* Verify that station can find a reversed track
   * Gaersnaes <=> Sim_Gar_W <=> testStation1
   */
  QCOMPARE(nc->parseCmd("ADD TRACK Sim_Gar_W 11357 COORDINATES "
                           "55.55427467440431 14.35344709206785 "
                           "55.54651317914202 14.18147578739106"), true);
  QCOMPARE(nc->parseCmd("ADD STATION Simrishamn COORDINATES 55.55402301071182 14.35252057730063"), true);
  QCOMPARE(nc->parseCmd("ADD STATION Gaersnaes COORDINATES 55.54654125122775 14.18141868329311") ,true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK Sim_Gar_W FROM Simrishamn TO Gaersnaes"), true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK Sim_Gar_W FROM Gaersnaes TO Simrishamn"), true);
  QCOMPARE(nc->stationList.at(0)->findLeavingTrackIndexToStation(1), 0);
  QCOMPARE(nc->stationList.at(1)->findLeavingTrackIndexToStation(0), 0);
}

void TstTrafficControl::station_addingRemovingPassengers()
{ /* Verify that the station can add and remove passengers properly*/
  nc->parseCmd("ADD STATION station");
  QVERIFY(true == nc->stationList.at(0)->changeNbrOfPassengers(10));
  QCOMPARE(10, nc->stationList.at(0)->getNbrOfWaitingPassengers());
  QVERIFY(true == nc->stationList.at(0)->changeNbrOfPassengers(-10));
  QCOMPARE(0, nc->stationList.at(0)->getNbrOfWaitingPassengers());
}

void TstTrafficControl::station_addInvalidTrackToStation()
{ /* Verify that it is possible to add tracks to station - negative case*/
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRACK trackA 1234");
  QVERIFY(false == nc->stationList.at(0)->addTrack(UNDEFINED));
  QVERIFY(false == nc->stationList.at(0)->addTrack(-3));
  QVERIFY(false == nc->stationList.at(0)->addTrack(3));
  QVERIFY(true == nc->stationList.at(0)->addTrack(0));
  QVERIFY(false == nc->stationList.at(0)->addTrack(0));
}

void TstTrafficControl::station_addTrackToStation()
{ /* Verify that it is possible to add tracks to station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 2345");
  QVERIFY(true == nc->stationList.at(0)->addTrack(0));
  QVERIFY(true == nc->stationList.at(0)->addTrack(1));
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().at(0));
  QVERIFY(1 == nc->stationList.at(0)->getLeavingTrackList().at(1));
  QVERIFY(2 == nc->stationList.at(0)->getLeavingTrackList().length());
}

/*void TstTrafficControl::station_assertIfDuplicateTrainAtStation()
{
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  QVERIFY(true == nc->stationList.at(0)->trainArrival(0));
  QVERIFY(0 == nc->stationList.at(0)->getTrainList().at(0));
  //qInstallMessageHandler(myMessageOutput);
  EXPECT_DEATH(nc->stationList.at(0)->trainArrival(0),"test123");
  //qInstallMessageHandler(0);
}*/

void TstTrafficControl::station_connectBidirectionalTrackToStations()
{/* Verify the track's start/end stations are not overwritten when the track
  * is connected in the opposite direction between two stations
  */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB"));
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackA FROM stationB TO stationA"));
  QVERIFY(0 == nc->trackList.at(0)->getStartStation());
  QVERIFY(1 == nc->trackList.at(0)->getEndStation());
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().at(0));
}

void TstTrafficControl::station_connectNonExistingTrackToStations()
{ /* Verify that the network can't connect a non-existing track to a station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  QVERIFY(false == nc->connectTrackToStations(0, 0, 1));
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().length());
  QVERIFY(false == nc->connectTrackToStationsByName("nonExistingTrack", "stationA", "stationB"));
}

void TstTrafficControl::station_connectTrackToStations()
{/* Verify that the network can't connect a non-existing track to a station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  QVERIFY(true == nc->connectTrackToStationsByName("trackA", "stationA", "stationB"));
  QVERIFY(0 == nc->trackList.at(0)->getStartStation());
  QVERIFY(1 == nc->trackList.at(0)->getEndStation());
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().at(0));
  nc->parseCmd("ADD TRACK trackB 2345");
  QVERIFY(true == nc->connectTrackToStations(1, 1, 0));
  QVERIFY(1 == nc->trackList.at(1)->getStartStation());
  QVERIFY(0 == nc->trackList.at(1)->getEndStation());
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().at(0));
  QVERIFY(1 == nc->stationList.at(1)->getLeavingTrackList().at(0));
}

void TstTrafficControl::station_emptyAfterCreated()
{
  QVERIFY( 0 == nc->stationList.length());
  nc->parseCmd("ADD STATION station");
  QVERIFY( 1 == nc->stationList.length());
  QVERIFY(0 == nc->stationList.at(0)->getTrainList().length());
  QVERIFY(0 == nc->stationList.at(0)->getLeavingTrackList().length());
  QVERIFY(0 == nc->stationList.at(0)->getNbrOfWaitingPassengers());
  QVERIFY(false == nc->stationList.at(0)->setNbrOfPlatforms(0));
  QVERIFY(false == nc->stationList.at(0)->setNbrOfPlatforms(-1));
}

void TstTrafficControl::station_findTrackToNextStation()
{ /*Verify that a station can find a track that goes to another station
   *   stationA - trackC - stationB
   *   stationA - trackD - stationC
   */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD STATION stationC");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 1234");
  nc->parseCmd("ADD TRACK trackC 1234");
  nc->parseCmd("ADD TRACK trackD 1234");
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackD FROM stationA TO stationC"));
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackC FROM stationA TO stationB"));
  QVERIFY(2 == nc->stationList.at(0)->findLeavingTrackIndexToStation(1));
  QVERIFY(3 == nc->stationList.at(0)->findLeavingTrackIndexToStation(2));
}

void TstTrafficControl::station_findTrackToNextStationNegative()
{ /* Verify that:
   *   When searching for a non-existing station, the result shall be UNDEFINED
   *   When searching for itself, the result shall be UNDEFINED
   *   When searching for an existing but non-adjacent station, the result shall be UNDEFINED
   *   stationA - trackB - stationB - trackB - stationC
   */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD STATION stationC");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 1234");
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB"));
  QVERIFY(true == nc->parseCmd("CONNECT TRACK trackB FROM stationB TO stationC"));
  QVERIFY(0 == nc->stationList.at(0)->findLeavingTrackIndexToStation(1));
  QVERIFY(UNDEFINED == nc->stationList.at(0)->findLeavingTrackIndexToStation(2));
  QVERIFY(1 == nc->stationList.at(1)->findLeavingTrackIndexToStation(2));
  QVERIFY(UNDEFINED == nc->stationList.at(1)->findLeavingTrackIndexToStation(0));
  QVERIFY(UNDEFINED == nc->stationList.at(1)->findLeavingTrackIndexToStation(UNDEFINED));
  QVERIFY(UNDEFINED == nc->stationList.at(1)->findLeavingTrackIndexToStation(-2));
  QVERIFY(UNDEFINED == nc->stationList.at(1)->findLeavingTrackIndexToStation(4));
}

void TstTrafficControl::station_invalidFloatCoordinatesAtCreation()
{ /* Verify that station will ignore invalid coordinates at creation, for
     example 95 N, 100W*/
  nc->parseCmd("ADD STATION station COORDINATES "
               "105.7272962532263 13.16723731495294");
  QVERIFY(0.0 == nc->stationList.at(0)->getLatitude());
  QVERIFY(0.0 == nc->stationList.at(0)->getLongitude());
  QVERIFY(false == nc->stationList.at(0)->hasCoordinates());
  nc->parseCmd("ADD STATION station COORDINATES "
               "55.7272962532263 213.16723731495294");
  QVERIFY(0.0 == nc->stationList.at(1)->getLatitude());
  QVERIFY(0.0 == nc->stationList.at(1)->getLongitude());
  QVERIFY(false == nc->stationList.at(1)->hasCoordinates());
}

void TstTrafficControl::station_junctionHasNoPassengersAfterAddingTen()
{ /* Verify that a Junction cannot accept any waiting passengers */
  nc->parseCmd("ADD STATION station AS JUNCTION");
  nc->stationList.at(0)->changeNbrOfPassengers(10);
  QVERIFY(0 == nc->stationList.at(0)->getNbrOfWaitingPassengers());

}

void TstTrafficControl::station_noMoreTrainsThanPlatforms()
{ /* Verify that there can't be more trains on a station than platforms. */
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  nc->parseCmd("ADD TRAIN train1");
  nc->parseCmd("ADD TRAIN train2");
  nc->parseCmd("ADD TRAIN train3");
  QVERIFY(2 == nc->stationList.at(0)->getNbrOfPlatforms());
  nc->stationList.at(0)->setNbrOfPlatforms(3);
  QVERIFY(3 == nc->stationList.at(0)->getNbrOfPlatforms());
  QVERIFY(true == nc->stationList.at(0)->trainArrival(3));
  QVERIFY(1 == nc->stationList.at(0)->getTrainList().length());
  QVERIFY(3 == nc->stationList.at(0)->getTrainList().at(0));
  QVERIFY(true == nc->stationList.at(0)->trainArrival(1));
  QVERIFY(true == nc->stationList.at(0)->trainArrival(2));
  QVERIFY(3 == nc->stationList.at(0)->getTrainList().length());
  QVERIFY(false == nc->stationList.at(0)->trainArrival(0));
}

void TstTrafficControl::station_nonNumericalCoordinatesAtCreation()
{ /* Verify that station will ignore invalid coordinates at creation*/
  nc->parseCmd("ADD STATION station COORDINATES "
               "55.727hej2962532263 13.16723731495294");
  QVERIFY(0.0 == nc->stationList.at(0)->getLatitude());
  QVERIFY(0.0 == nc->stationList.at(0)->getLongitude());
  QVERIFY(false == nc->stationList.at(0)->hasCoordinates());
}

void TstTrafficControl::station_tenPassengersAfterAddingTen()
{
  nc->parseCmd("ADD STATION station");
  nc->stationList.at(0)->changeNbrOfPassengers(10);
  QVERIFY(10 == nc->stationList.at(0)->getNbrOfWaitingPassengers());
}

void TstTrafficControl::station_trainArrivalDeparture()
{ /* Verify that the station can add and remove present trains properly */
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  nc->parseCmd("ADD TRAIN train1");
  nc->parseCmd("ADD TRAIN train2");
  QCOMPARE(true, nc->stationList.at(0)->trainArrival(1));
  QCOMPARE(1, nc->stationList.at(0)->getTrainList().at(0));
  QCOMPARE(true, nc->stationList.at(0)->trainArrival(0));
  QCOMPARE(0, nc->stationList.at(0)->getTrainList().at(1));
  QCOMPARE(false, nc->stationList.at(0)->trainArrival(2));
  QCOMPARE(true, nc->stationList.at(0)->trainDeparture(1));
  QCOMPARE(0, nc->stationList.at(0)->getTrainList().at(0));
}

void TstTrafficControl::station_trainListAfterAddingRemoving()
{ /* Verify that the trainList is populated and depopulated properly */
  nc->parseCmd("ADD STATION station");
  nc->addTrainToNetwork("train1");
  nc->addTrainToNetwork("train2");
  nc->stationList.at(0)->trainArrival(0);
  nc->stationList.at(0)->trainArrival(1);
  QCOMPARE(2, nc->stationList.at(0)->getTrainList().length());
  nc->stationList.at(0)->trainDeparture(0);
  QCOMPARE(1, nc->stationList.at(0)->getTrainList().length());
  QCOMPARE(1, nc->stationList.at(0)->getTrainList().at(0));
  nc->parseCmd("ADD TRAIN trainB");
  nc->parseCmd("TRAIN SET CURRENT STATION station");
  QCOMPARE(2, nc->stationList.at(0)->getTrainList().at(1));
  QCOMPARE(2, nc->stationList.at(0)->getTrainList().length());
}

void TstTrafficControl::station_trainListNonExistingTrain()
{ /* Add a train to trainList and remove a non-existing train */
  nc->parseCmd("ADD STATION stationA");
  QCOMPARE(false, nc->stationList.at(0)->trainDeparture(3));
  nc->parseCmd("ADD TRAIN trainA");
  nc->parseCmd("ADD TRAIN trainB");
  QCOMPARE(true, nc->stationList.at(0)->trainArrival(1));
  QCOMPARE(false, nc->stationList.at(0)->trainDeparture(-1));
  QCOMPARE(false, nc->stationList.at(0)->trainDeparture(0));
  QCOMPARE(1, nc->stationList.at(0)->getTrainList().at(0));
  QCOMPARE(false, nc->stationList.at(0)->trainArrival(2));
  QCOMPARE(false, nc->stationList.at(0)->trainArrival(-1));
  QCOMPARE(1, nc->stationList.at(0)->getTrainList().length());
}


void TstTrafficControl::station_validateCheckIfTrackLeavesStation()
{
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 2345");
  nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB");
  QVERIFY(true == nc->stationList.at(0)->checkIfTrackLeavesStation(0));
  QVERIFY(false == nc->stationList.at(0)->checkIfTrackLeavesStation(1));
  QVERIFY(false == nc->stationList.at(0)->checkIfTrackLeavesStation(3));
  QVERIFY(false == nc->stationList.at(0)->checkIfTrackLeavesStation(-1));
}

void TstTrafficControl::station_validCoordinatesAtCreation()
{ /* Will station add coordinates and set "hasCoordinates=TRUE"*/
  QString latitude = "55.72729652532263";
  QString longitude = "13.16723731495294";
  nc->addStationToNetwork("station", false, latitude, longitude);
  QVERIFY(true == nc->stationList.at(0)->hasCoordinates());
  QVERIFY(latitude.toFloat() == nc->stationList.at(0)->getLatitude());
  QVERIFY(longitude.toFloat() == nc->stationList.at(0)->getLongitude());
}

void TstTrafficControl::station_validNegativeCoordinatesAtCreation()
{ /* Verify that station will add negative coordinates and set
     "hasCoordinates=TRUE"*/
  QString latitude = "-55.72729652532263";
  QString longitude = "-13.16723731495294";
  nc->addStationToNetwork("station", false, latitude, longitude);
  QVERIFY(true == nc->stationList.at(0)->hasCoordinates());
  QVERIFY(latitude.toFloat() == nc->stationList.at(0)->getLatitude());
  QVERIFY(longitude.toFloat() == nc->stationList.at(0)->getLongitude());
}

void TstTrafficControl::track_trackLength()
{ /* Verify that the track is added if length is correct */
  QCOMPARE(nc->parseCmd("ADD TRACK trackA 1234"), true);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB -1234"), false);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB 0"), false);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB tjugo"), false);
}

void TstTrafficControl::track_trackCoordinates()
{ /* Verify that the track adds coordinates if existing */
  QCOMPARE(nc->parseCmd("ADD TRACK trackA 1234"), true);
  QCOMPARE(nc->trackList.at(0)->hasCoordinates(),false);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "55.6416151613465 13.08148964751508 "
                           "55.6338439674224 13.06831950424542"), true);
  QCOMPARE(nc->trackList.at(1)->hasCoordinates(),true);
  QCOMPARE(nc->parseCmd("ADD TRACK trackC 1196 COORDINATES "
                           "-55.6416151613465 -13.08148964751508 "
                           "-55.6338439674224 -13.06831950424542"), true);
  QCOMPARE(nc->trackList.at(2)->hasCoordinates(),true);
}

void TstTrafficControl::track_trackCoordinatesInvalid()
{ /* Verify that the constructor will reject invalid coordinates and not create
   * station if any of the coordinates is invalid.
   */
  QCOMPARE(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "55.6416151613465 13.08148964751508 "
                           "55.6338439674224 13.068d31950424542"), false);
  QCOMPARE(nc->parseCmd("ADD TRACK track 1196 COORDINATES "
                           "55.6416151613465 213.08148964751508 "
                           "55.6338439674224 213.06831950424542"), false);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "95.6416151613465 13.08148964751508 "
                           "95.6338439674224 13.06831950424542"), false);
  QCOMPARE(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "), false);
  QCOMPARE(nc->trackList.length(), 0);
}

void TstTrafficControl::track_startEndStation()
{ /* Verifies that start and end station are empty after creation */
  nc->parseCmd("ADD TRACK trackB 1196");
  QCOMPARE(nc->trackList.at(0)->getEndStation(), UNDEFINED);
  QCOMPARE(nc->trackList.at(0)->getStartStation(), UNDEFINED);
}

//TEST_F(trackTest, trainListAddRmUpstream)
//{ /* Verify that it is possible to add and remove trains on trainList (FIFO)
//   * FIFO manner.
//   * stationA  -> trackAB  -> stationB
//   * trainA ->
//   * trainB ->
//   * trainB starts when trainA is more than 3000 m into the trackAB
//   */
//  QCOMPARE(nc->parseCmd("ADD STATION stationA"), true));
//  QCOMPARE(nc->parseCmd("ADD STATION stationB"), true));
//  QCOMPARE(nc->parseCmd("ADD TRACK trackAB 12345"), true));
//  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), true));
//  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), true));
//  QCOMPARE(nc->parseCmd("ADD TRAIN trainA"), true));
//  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), true));
//  QCOMPARE(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), true));
//  //Check that there is one track on queue
//  nc->stepTimeForNetwork(8);
//  do{
//    nc->stepTimeForNetwork(1);
//  }
//  while (nc->trainList.at(0)->getTrackPosition()<3000);
//  QCOMPARE(nc->trackList.at(0)->isLockedDownStream(), false));
//  QCOMPARE(nc->trackList.at(0)->isLockedUpStream(), true));
//  QCOMPARE(nc->parseCmd("ADD TRAIN trainB"), true));
//  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), true));
//  QCOMPARE(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), true));
//  nc->stepTimeForNetwork(8);
//  QCOMPARE(nc->trackList.at(0)->getTrainList().at(0),0));
//  QCOMPARE(nc->trackList.at(0)->getTrainList().at(1),1));
//  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),2));
//  do{
//    nc->stepTimeForNetwork(1);
//  }
//  while (nc->trainList.at(0)->getTrackPosition()<12344);
//  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),2));
//  nc->stepTimeForNetwork(10);
//  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),1));
//  QCOMPARE(nc->trackList.at(0)->getTrainList().at(0),1));
//  do{
//    nc->stepTimeForNetwork(1);
//  }
//  while (nc->trainList.at(1)->getTrackPosition()<12344);
//  nc->stepTimeForNetwork(10);
//  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),0));
//}

void TstTrafficControl::track_trainListAddDownstream()
{ /* Verify that it is possible to add and remove trains on trainList in FIFO manner, rejecting
   * Upstream trains if Downstream is used.
   * stationA  -> trackAB  -> stationB
   *                         <- trainA
   * trainB ->
   * trainB starts when trainA is more than 10 m into the trackAB
   */
  QCOMPARE(nc->parseCmd("ADD STATION stationA"), true);
  QCOMPARE(nc->parseCmd("ADD STATION stationB"), true);
  QCOMPARE(nc->parseCmd("ADD TRACK trackAB 12345"), true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), true);
  QCOMPARE(nc->parseCmd("ADD TRAIN trainA"), true);
  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationB"), true);
  QCOMPARE(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationA"), true);
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState()!=2);
  QCOMPARE(nc->trackList.at(0)->getTrainList().at(0),0);
  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),1);
  QCOMPARE(nc->parseCmd("ADD TRAIN trainB"), true);
  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), true);
  QCOMPARE(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), true);
  //CHECK THAT THERE IS ONLY ONE TRAIN ON TRACK
  nc->stepTimeForNetwork(8);
  QCOMPARE(nc->trackList.at(0)->getTrainList().at(0),0);
  QCOMPARE(nc->trackList.at(0)->getTrainList().length(),1);
  QCOMPARE(nc->trackList.at(0)->isReversedTraffic(), true);
  QCOMPARE(nc->trainList.at(1)->getState(), 1);//READY
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState() == 2);//RUNNING
  nc->stepTimeForNetwork(3);
  QCOMPARE(nc->trainList.at(1)->getState(), 2);//RUNNING
}

void TstTrafficControl::traffic_regularTrackTest()
{ /* Verify that track is marked as reversed if a train enters a track in
   * regular direction.
   * stationA -> stationB
   * trainAB->
   */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackAB 1234");
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), true);
  nc->parseCmd("ADD TRAIN trainAB");
  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), true);
  QCOMPARE(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), true);
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState()!=2); //RUNNING
  QCOMPARE(nc->trainList.at(0)->getCurrentTrack(), 0);
  QCOMPARE(nc->trainList.at(0)->getTrackPosition(), 0);
  QCOMPARE(nc->trainList.at(0)->getCurrentStation(), -1);
  QCOMPARE(nc->trackList.at(0)->isReversedTraffic(), false);
  nc->stepTimeForNetwork(1);
  QCOMPARE(nc->trainList.at(0)->getTrackPosition(), 1);
}

void TstTrafficControl::traffic_reversedTrackTest()
{ /* Verify that track is marked as reversed if a train enters a track in
   * opposite direction.
   * stationB -> stationC
   *            <-trainBC
   */
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD STATION stationC");
  nc->parseCmd("ADD TRACK trackBC 1234");
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackBC FROM stationB TO stationC"), true);
  QCOMPARE(nc->parseCmd("CONNECT TRACK trackBC FROM stationC TO stationB"), true);
  nc->parseCmd("ADD TRAIN trainCB");
  QCOMPARE(nc->parseCmd("TRAIN SET CURRENT STATION stationC"), true);
  nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB");
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState()!=2); //RUNNING
  QCOMPARE(nc->trainList.at(0)->getCurrentTrack(), 0);
  QCOMPARE(nc->trainList.at(0)->getTrackPosition(),
              nc->trackList.at(0)->getLength());

  QCOMPARE(nc->trainList.at(0)->getCurrentStation(), -1);

  QCOMPARE(nc->trackList.at(0)->isReversedTraffic(), true);
  nc->stepTimeForNetwork(1);

  QCOMPARE(nc->trainList.at(0)->getTrackPosition(),
              nc->trackList.at(0)->getLength() - 1);
}

//void TstTrafficControl::traffic_fastTrainBehindSlowTrainOnTrack)


void TstTrafficControl::train_nbrOfPassengersAddRemove()
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  QCOMPARE(nc->trainList.at(0)->getNbrOfPassengers(), 0);
  int deniedPassengers = nc->trainList.at(0)->load(3);
  QCOMPARE(nc->trainList.at(0)->getNbrOfPassengers(), 3);
  QCOMPARE(deniedPassengers, 0);
  deniedPassengers = nc->trainList.at(0)->load(-4);
  QCOMPARE(nc->trainList.at(0)->getNbrOfPassengers(), 0);
  QCOMPARE(deniedPassengers, -1);
}

void TstTrafficControl::train_nbrOfPassengersVsCapacity()
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  int capacity = nc->trainList.at(0)->getPassengerCapacity();
  int denied = nc->trainList.at(0)->load(4);
  denied = nc->trainList.at(0)->load(capacity);
  QCOMPARE(denied, 4);
  QCOMPARE(nc->trainList.at(0)->getNbrOfPassengers(), capacity);
}

void TstTrafficControl::train_statusAfterCreation()
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  QCOMPARE(nc->trainList.at(0)->getCurrentStation(), UNDEFINED);
  QCOMPARE(nc->trainList.at(0)->getCurrentTrack(), UNDEFINED);
  QCOMPARE(nc->trainList.at(0)->getTravelPlan().length(), 0);
}

QTEST_MAIN(TstTrafficControl)

#include "tst_trafficcontrol.moc"
