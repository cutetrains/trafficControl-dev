#ifndef _TST_TCSTATIONTESTS_H_
#define _TST_TCSTATIONTESTS_H_

#define UNDEFINED -1

using namespace testing;

class stationTest : public networkTest{
};

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

TEST_F(stationTest, addTrackToStation)
{ /* Verify that it is possible to add tracks to station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 2345");
  EXPECT_THAT(nc->stationList.at(0)->addTrack(0), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->addTrack(1), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(0), Eq(0));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(1), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().length(), Eq(2));
}

TEST_F(stationTest, addInvalidTrackToStation)
{ /* Verify that it is possible to add tracks to station - negative case*/
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD TRACK trackA 1234");
  EXPECT_THAT(nc->stationList.at(0)->addTrack(UNDEFINED),
              Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->addTrack(-3), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->addTrack(3), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->addTrack(0), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->addTrack(0), Eq(false));
}

TEST_F(stationTest, validateCheckIfTrackLeavesStation)
{
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  nc->parseCmd("ADD TRACK trackB 2345");
  nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB");
  EXPECT_THAT(nc->stationList.at(0)->checkIfTrackLeavesStation(0),
              Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->checkIfTrackLeavesStation(1),
              Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->checkIfTrackLeavesStation(3),
              Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->checkIfTrackLeavesStation(-1),
              Eq(false));
}

TEST_F(stationTest, connectNonExistingTrackToStations)
{ /* Verify that the network can't connect a non-existing track to a station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  EXPECT_THAT(nc->connectTrackToStations(0, 0, 1), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().length(),Eq(0));
  EXPECT_THAT(nc->connectTrackToStationsByName("nonExistingTrack", "stationA", "stationB"), Eq(false));
}

TEST_F(stationTest, connectTrackToStations)
{/* Verify that the network can't connect a non-existing track to a station */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  EXPECT_THAT(nc->connectTrackToStationsByName("trackA",
                                                     "stationA",
                                                     "stationB"),
                                                         Eq(true));
  EXPECT_THAT(nc->trackList.at(0)->getStartStation(), Eq(0));
  EXPECT_THAT(nc->trackList.at(0)->getEndStation(), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(0),
              Eq(0));
  nc->parseCmd("ADD TRACK trackB 2345");
  EXPECT_THAT(nc->connectTrackToStations(1, 1, 0), Eq(true));
  EXPECT_THAT(nc->trackList.at(1)->getStartStation(), Eq(1));
  EXPECT_THAT(nc->trackList.at(1)->getEndStation(), Eq(0));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(0),
              Eq(0));
  EXPECT_THAT(nc->stationList.at(1)->getLeavingTrackList().at(0),
              Eq(1));
}

TEST_F(stationTest, connectBidirectionalTrackToStations)
{/* Verify the track's start/end stations are not overwritten when the track
  * is connected in the opposite direction between two stations
  */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackA 1234");
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB"),
                           Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackA FROM stationB TO stationA"),
                           Eq(true));
  EXPECT_THAT(nc->trackList.at(0)->getStartStation(), Eq(0));
  EXPECT_THAT(nc->trackList.at(0)->getEndStation(), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().at(0), Eq(0));
}

TEST_F(stationTest, constructor_invalidFloatCoordinatesAtCreation)
{ /* Verify that station will ignore invalid coordinates at creation, for
     example 95 N, 100W*/
  nc->parseCmd("ADD STATION station COORDINATES "
               "105.7272962532263 13.16723731495294");
  EXPECT_THAT(nc->stationList.at(0)->getLatitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(0)->getLongitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(0)->hasCoordinates(), Eq(false));
  nc->parseCmd("ADD STATION station COORDINATES "
               "55.7272962532263 213.16723731495294");
  EXPECT_THAT(nc->stationList.at(1)->getLatitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(1)->getLongitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(1)->hasCoordinates(), Eq(false));
}

TEST_F(stationTest, constructor_junctionHasNoPassengersAfterAddingTen)
{ /* Verify that a Junction cannot accept any waiting passengers */
  nc->parseCmd("ADD STATION station AS JUNCTION");
  nc->stationList.at(0)->changeNbrOfPassengers(10);
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfWaitingPassengers(),
              Eq(0));
}

TEST_F(stationTest, constructor_leavingTrackListEmptyAtCreation)
{ /* Verify that station has no defined leaving tracks at creation */
  nc->parseCmd("ADD STATION station COORDINATES "
               "55.7272962532263 13.16723731495294");
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().length(), Eq(0));
}

TEST_F(stationTest, constructor_noCoordinatesAtCreation)
{ /* Will station report no coordinates if station is created without
     coordinates*/
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->hasCoordinates(), Eq(false));
}

TEST_F(stationTest, constructor_noLeavingTracksAfterCreation)
{ /*Verify that the station has no leaving tracks after creation */
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getLeavingTrackList().length(), Eq(0));
}

TEST_F(stationTest, constructor_nonNumericalCoordinatesAtCreation)
{ /* Verify that station will ignore invalid coordinates at creation*/
  nc->parseCmd("ADD STATION station COORDINATES "
               "55.727hej2962532263 13.16723731495294");
  EXPECT_THAT(nc->stationList.at(0)->getLatitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(0)->getLongitude(), Eq(0.0));
  EXPECT_THAT(nc->stationList.at(0)->hasCoordinates(), Eq(false));
}

TEST_F(stationTest, constructor_noPassengersAfterCreated)
{ /* Verify that the station has no passengers after created */
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfWaitingPassengers(), Eq(0));
}

TEST_F(stationTest, constructor_noTrainsAfterCreation)
{ /* Verify that the station has no trains after created */
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(0));
}

TEST_F(stationTest, constructor_validCoordinatesAtCreation)
{ /* Will station add coordinates and set "hasCoordinates=TRUE"*/
  QString latitude = "55.72729652532263";
  QString longitude = "13.16723731495294";
  nc->addStationToNetwork("station", false, latitude, longitude);
  EXPECT_THAT(nc->stationList.at(0)->hasCoordinates(), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getLatitude(), Eq(latitude.toFloat()));
  EXPECT_THAT(nc->stationList.at(0)->getLongitude(), Eq(longitude.toFloat()));
}

TEST_F(stationTest, constructor_validNegativeCoordinatesAtCreation)
{ /* Verify that station will add negative coordinates and set
     "hasCoordinates=TRUE"*/
  QString latitude = "-55.72729652532263";
  QString longitude = "-13.16723731495294";
  nc->addStationToNetwork("station", false, latitude, longitude);
  EXPECT_THAT(nc->stationList.at(0)->hasCoordinates(), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getLatitude(),
              Eq(latitude.toFloat()));
  EXPECT_THAT(nc->stationList.at(0)->getLongitude(),
              Eq(longitude.toFloat()));
}

TEST_F(stationTest, findTrackToNextStation)
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
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackD FROM stationA TO stationC"),
              Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackC FROM stationA TO stationB"),
              Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->findLeavingTrackIndexToStation(1), Eq(2));
  EXPECT_THAT(nc->stationList.at(0)->findLeavingTrackIndexToStation(2), Eq(3));
}

TEST_F(stationTest, findTrackToNextStationNegative)
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
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackA FROM stationA TO stationB"),
              Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackB FROM stationB TO stationC"),
              Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->findLeavingTrackIndexToStation(1), Eq(0));
  EXPECT_THAT(nc->stationList.at(0)->findLeavingTrackIndexToStation(2), Eq(UNDEFINED));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(2), Eq(1));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(0), Eq(UNDEFINED));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(UNDEFINED), Eq(UNDEFINED));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(-2), Eq(UNDEFINED));
  EXPECT_THAT(nc->stationList.at(1)->findLeavingTrackIndexToStation(4), Eq(UNDEFINED));
}

TEST_F(stationTest, noNegativeNbrOfPlatforms)
{ /* Verify that it is impossible to set 0 or negative number of platforms*/
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->setNbrOfPlatforms(0), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->setNbrOfPlatforms(-1), Eq(false));
}

TEST_F(stationTest, noMoreTrainsThanPlatforms)
{ /* Verify that there can't be more trains on a station than platforms. */
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  nc->parseCmd("ADD TRAIN train1");
  nc->parseCmd("ADD TRAIN train2");
  nc->parseCmd("ADD TRAIN train3");
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfPlatforms(), Eq(2));
  nc->stationList.at(0)->setNbrOfPlatforms(3);
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfPlatforms(), Eq(3));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(3), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(3));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(1), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(2), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(3));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(0), Eq(false));
}

TEST_F(stationTest, assertIfDuplicateTrainAtStation)
{ /*MANUAL TEST TO VERIFY THAT ASSERT IS THROWN*/
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(0), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(0));
  qInstallMessageHandler(myMessageOutput);
  EXPECT_DEATH(nc->stationList.at(0)->trainArrival(0),"");
  qInstallMessageHandler(0);
}

TEST_F(stationTest, noPassengersAfterAddingTenRemovingTwenty)
{ /* Verify that the station can add and remove passengers properly*/
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->changeNbrOfPassengers(10), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->changeNbrOfPassengers(-20), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfWaitingPassengers(),
              Eq(0));
}

TEST_F(stationTest, trainArrivalDeparture)
{ /* Verify that the station can add and remove present trains properly */
  nc->parseCmd("ADD STATION station");
  nc->parseCmd("ADD TRAIN train0");
  nc->parseCmd("ADD TRAIN train1");
  nc->parseCmd("ADD TRAIN train2");
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(1), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(0), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(1), Eq(0));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(2), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->trainDeparture(1), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(0));
}

TEST_F(stationTest, trainListAfterAddingRemoving)
{ /* Verify that the trainList is populated and depopulated properly */
  nc->parseCmd("ADD STATION station");
  nc->addTrainToNetwork("train1");
  nc->addTrainToNetwork("train2");
  nc->stationList.at(0)->trainArrival(0);
  nc->stationList.at(0)->trainArrival(1);
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(2));
  nc->stationList.at(0)->trainDeparture(0);
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(1));
}

TEST_F(stationTest, trainListAfterAddingTwo)
{ /*Verify that the trainList is populated and depopulated properly */
  nc->parseCmd("ADD STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(0));
  nc->parseCmd("ADD TRAIN trainA");
  EXPECT_THAT(nc->trainList.at(0)->getID(), Eq(0));
  nc->parseCmd("TRAIN SET CURRENT STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(0));
  nc->parseCmd("ADD TRAIN trainB");
  nc->parseCmd("TRAIN SET CURRENT STATION station");
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(1), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(2));
}

TEST_F(stationTest, trainListNonExistingTrain)
{ /* Add a train to trainList and remove a non-existing train */
  nc->parseCmd("ADD STATION stationA");
  EXPECT_THAT(nc->stationList.at(0)->trainDeparture(3), Eq(false));
  nc->parseCmd("ADD TRAIN trainA");
  nc->parseCmd("ADD TRAIN trainB");
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(1), Eq(true));
  EXPECT_THAT(nc->stationList.at(0)->trainDeparture(-1), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->trainDeparture(0), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().at(0), Eq(1));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(2), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->trainArrival(-1), Eq(false));
  EXPECT_THAT(nc->stationList.at(0)->getTrainList().length(), Eq(1));
}


TEST_F(stationTest, tenPassengersAfterAddingTen)
{ /* Verify that the station can add passengers properly*/
  nc->parseCmd("ADD STATION station");
  nc->stationList.at(0)->changeNbrOfPassengers(10);
  EXPECT_THAT(nc->stationList.at(0)->getNbrOfWaitingPassengers(), Eq(10));
}

/*void station_nameAlreadyTaken(); // Verify that the station will reject the
 *                                    command if the proposed name is already taken */
/*void station_invalidName(); // Verify that the station names will contain onl
                                 [a-ö,A-Ö,0-9,-,_] */
/*void station_verifyName();// Verify that the name is copied correctly */

#endif //_TST_TCSTATIONTESTS_H_
