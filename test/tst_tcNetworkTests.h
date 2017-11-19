#ifndef _TST_TCNETWORKTESTS_H_
#define _TST_TCNETWORKTESTS_H_

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QApplication>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "inc/trafficcontrol.h"
#include "inc/trafficdatamodel.h"
//#include "tst_tcStationTests.h"

#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace testing;


class networkTest : public ::testing::Test
{
protected:
  TrafficDataModel *trackListModel;
  TrafficDataModel *trainListModel;
  TrafficDataModel *stationListModel;
  QObject* handleQMLObject;
  NetworkControl *nc;
  virtual void SetUp()
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

  virtual void TearDown()
  {
    delete trackListModel;
    delete trainListModel;
    delete stationListModel;
    delete handleQMLObject;
    delete nc;
  }
};


TEST_F(networkTest, addOneTrainToStation)
{
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRAIN trainA");
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(1));
  int firstTrainAtStationA = nc->stationList.at(0)->getTrainList().at(0);
  EXPECT_THAT(nc->trainList.at(firstTrainAtStationA)->getName(), Eq("trainA"));
}

TEST_F(networkTest, connectThreeTracksToTwoStations)
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
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().length(), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(0), Eq(1));
  EXPECT_THAT(nc->stationList.at(1)->getLeavingTrackList().length(), Eq(2));
  EXPECT_THAT(nc->stationList.at(1)->getLeavingTrackList().at(0), Eq(0));
  EXPECT_THAT(nc->stationList.at(1)->getLeavingTrackList().at(1), Eq(2));
  EXPECT_THAT(nc->trackList.at(0)->getStartStation(), Eq(1));
  EXPECT_THAT(nc->trackList.at(0)->getEndStation(), Eq(0));
  EXPECT_THAT(nc->trackList.at(1)->getStartStation(), Eq(0));
  EXPECT_THAT(nc->trackList.at(1)->getEndStation(), Eq(1));
  EXPECT_THAT(nc->trackList.at(2)->getStartStation(), Eq(1));
  EXPECT_THAT(nc->trackList.at(2)->getEndStation(), Eq(0));
}


TEST_F(networkTest, findReversedTrack)
{ /* Verify that station can find a reversed track
   * Gaersnaes <=> Sim_Gar_W <=> testStation1
   */
  EXPECT_THAT(nc->parseCmd("ADD TRACK Sim_Gar_W 11357 COORDINATES "
                           "55.55427467440431 14.35344709206785 "
                           "55.54651317914202 14.18147578739106"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD STATION Simrishamn COORDINATES 55.55402301071182 14.35252057730063"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD STATION Gaersnaes COORDINATES 55.54654125122775 14.18141868329311") ,Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK Sim_Gar_W FROM Simrishamn TO Gaersnaes"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK Sim_Gar_W FROM Gaersnaes TO Simrishamn"), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->findLeavingTrackIndexToStation(1), Eq(0));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(0), Eq(0));
}

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

#endif //_TST_TCNETWORKTESTS_H_
