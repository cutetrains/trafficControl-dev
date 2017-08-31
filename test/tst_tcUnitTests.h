#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QApplication>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "inc/trafficcontrol.h"

using namespace testing;

TEST(stationTest, noPassengersAfterCreated)
{ /* Verify that the station has no passengers after created */
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
  EXPECT_THAT(newStation->getNbrOfWaitingPassengers(), Eq(0));
  delete newStation;
}

TEST(stationTest, noTrainsAfterCreation)
{ /* Verify that the station has no trains after created */
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
  EXPECT_THAT(newStation->getTrainList().length(), Eq(0));
  delete newStation;
}

TEST(stationTest, noCoordinatesAtCreation)
{ /* Will station report no coordinates if station is created without
     coordinates*/
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
  EXPECT_THAT(newStation->hasCoordinates(), Eq(false));
  delete newStation;
}

TEST(stationTest, validCoordinatesAtCreation)
{ /* Will station add coordinates and set "hasCoordinates=TRUE"*/
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
  EXPECT_THAT(newStation->hasCoordinates(), Eq(true));
  EXPECT_THAT(newStation->getLatitude(), Eq(latitude.toFloat()));
  EXPECT_THAT(newStation->getLongitude(), Eq(longitude.toFloat()));
  delete newStation;
}

TEST(stationTest, validNegativeCoordinatesAtCreation)
{ /* Verify that station will add negative coordinates and set
     "hasCoordinates=TRUE"*/
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
  EXPECT_THAT(newStation->hasCoordinates(), Eq(true));
  EXPECT_THAT(newStation->getLatitude(), Eq(latitude.toFloat()));
  EXPECT_THAT(newStation->getLongitude(), Eq(longitude.toFloat()));
  delete newStation;
}

TEST(stationTest, nonNumericalCoordinatesAtCreation)
{ /* Verify that station will ignore invalid coordinates at creation*/
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
  EXPECT_THAT(newStation->getLatitude(), Eq(0.0));
  EXPECT_THAT(newStation->getLongitude(), Eq(0.0));
  EXPECT_THAT(newStation->hasCoordinates(), Eq(false));
  delete newStation;
}

TEST(stationTest, invalidFloatCoordinatesAtCreation)
{ /* Verify that station will ignore invalid coordinates at creation, for
     example 95 N, 100W*/
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
  EXPECT_THAT(newStation->getLatitude(), Eq(0.0));
  EXPECT_THAT(newStation->getLongitude(), Eq(0.0));
  EXPECT_THAT(newStation->hasCoordinates(), Eq(false));
  delete newStation;
  Station* newStation2 = new Station("testStation",
                                  false,
                                  "55.7272962532263",
                                  "213.16723731495294",
                                  trackList,
                                  trainList,
                                  stationList);
  EXPECT_THAT(newStation2->getLatitude(), Eq(0.0));
  EXPECT_THAT(newStation2->getLongitude(), Eq(0.0));
  EXPECT_THAT(newStation2->hasCoordinates(), Eq(false));
  delete newStation2;
}

/*void station_nameAlreadyTaken(); // Verify that the station will rename new
                                      station with a random suffix if the
                                      proposed name is already taken */
/*void station_invalidName(); // Verify that the station names will contain onl
                                 [a-ö,A-Ö,0-9,-,_] */
/*void station_verifyName();// Verify that the name is copied correctly */

TEST(stationTest, leavingTrackListEmptyAtCreation)
{ /* Verify that station has no defined leaving tracks at creation */
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
  EXPECT_THAT(newStation->getLeavingTrackList().length(), Eq(0));
  delete newStation;
}

/*void station_timeForAdding100Stations(); // Observe the time needed for
                                       adding 100 stations */

TEST(stationTest, junctionHasNoPassengersAfterAddingTen)
{ /* Verify that a Junction cannot accept any waiting passengers */
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
  EXPECT_THAT(newStation->getNbrOfWaitingPassengers(), Eq(0));
  delete newStation;
}
/*void station_verifyTrainListAfterAddingRemoving(); // Verify that the
                                       trainList is populated and depopulated
                                       properly */
/*void station_noMoreTrainsThanPlatforms();*/

/*TEST(stationTest, noLeavingTracksAfterCreation); //Verify that the station has no leaving tracks after creation */

TEST(stationTest, tenPassengersAfterAddingTen)
{ /* Verify that the station can add passengers properly*/
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
  newStation->changeNbrOfPassengers(10);
  EXPECT_THAT(newStation->getNbrOfWaitingPassengers(), Eq(10));
}

TEST(stationTest, noPassengersAfterAddingTenRemovingTwenty)
{ /* Verify that the station can add and remove passengers properly*/
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

  newStation->changeNbrOfPassengers(10);
  newStation->changeNbrOfPassengers(-20);
  EXPECT_THAT(newStation->getNbrOfWaitingPassengers(), Eq(0));
}

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
