#ifndef _TST_TCTRACKTESTS_H_
#define _TST_TCTRACKTESTS_H_

#define UNDEFINED -1

using namespace testing;

class trackTest : public networkTest{
};

TEST_F(trackTest, constructor_trackLength)
{ /* Verify that the track is added if length is correct */
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackA 1234"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB -1234"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 0"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB tjugo"), Eq(false));
}

TEST_F(trackTest, constructor_trackCoordinates)
{ /* Verify that the track adds coordinates if existing */
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackA 1234"), Eq(true));
  EXPECT_THAT(nc->trackList.at(0)->hasCoordinates(),Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "55.6416151613465 13.08148964751508 "
                           "55.6338439674224 13.06831950424542"), Eq(true));
  EXPECT_THAT(nc->trackList.at(1)->hasCoordinates(),Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackC 1196 COORDINATES "
                           "-55.6416151613465 -13.08148964751508 "
                           "-55.6338439674224 -13.06831950424542"), Eq(true));
  EXPECT_THAT(nc->trackList.at(2)->hasCoordinates(),Eq(true));
}

TEST_F(trackTest, constructor_trackCoordinatesInvalid)
{ /* Verify that the constructor will reject invalid coordinates and not create
   * station if any of the coordinates is invalid.
   */
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "55.6416151613465 13.08148964751508 "
                           "55.6338439674224 13.068d31950424542"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK track 1196 COORDINATES "
                           "55.6416151613465 213.08148964751508 "
                           "55.6338439674224 213.06831950424542"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "
                           "95.6416151613465 13.08148964751508 "
                           "95.6338439674224 13.06831950424542"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 1196 COORDINATES "), Eq(false));
  EXPECT_THAT(nc->trackList.length(), Eq(0));
  EXPECT_THAT(0, Eq(0));
}

TEST_F(trackTest, constructor_startEndStation)
{ /* Verifies that start and end station are empty after creation */
  nc->parseCmd("ADD TRACK trackB 1196");
  EXPECT_THAT(nc->trackList.at(0)->getEndStation(), Eq(UNDEFINED));
  EXPECT_THAT(nc->trackList.at(0)->getStartStation(), Eq(UNDEFINED));
}

TEST_F(trackTest, trainListAddRmUpstream)
{ /* Verify that it is possible to add and remove trains on trainList (FIFO)
   * FIFO manner.
   * stationA  -> trackAB  -> stationB
   * trainA ->
   * trainB ->
   * trainB starts when trainA is more than 3000 m into the trackAB
   */
  EXPECT_THAT(nc->parseCmd("ADD STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD STATION stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackAB 12345"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRAIN trainA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), Eq(true));
  //Check that there is one track on queue
  nc->stepTimeForNetwork(8);
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getTrackPosition()<3000);
  EXPECT_THAT(nc->trackList.at(0)->isLockedDownStream(), Eq(false));
  EXPECT_THAT(nc->trackList.at(0)->isLockedUpStream(), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRAIN trainB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), Eq(true));
  nc->stepTimeForNetwork(8);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().at(0),Eq(0));
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().at(1),Eq(1));
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(2));
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getTrackPosition()<12344);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(2));
  nc->stepTimeForNetwork(10);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(1));
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().at(0),Eq(1));
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(1)->getTrackPosition()<12344);
  nc->stepTimeForNetwork(10);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(0));
}

TEST_F(trackTest, trainListAddDownstream)
{ /* Verify that it is possible to add and remove trains on trainList in FIFO manner, rejecting
   * Upstream trains if Downstream is used.
   * stationA  -> trackAB  -> stationB
   *                         <- trainA
   * trainB ->
   * trainB starts when trainA is more than 10 m into the trackAB
   */
  EXPECT_THAT(nc->parseCmd("ADD STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD STATION stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackAB 12345"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRAIN trainA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationA"), Eq(true));
  nc->stepTimeForNetwork(8);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().at(0),Eq(0));
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(1));
  EXPECT_THAT(nc->parseCmd("ADD TRAIN trainB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), Eq(true));
  //CHECK THAT THERE IS ONLY ONE TRAIN ON TRACK
  nc->stepTimeForNetwork(8);
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().at(0),Eq(0));
  EXPECT_THAT(nc->trackList.at(0)->getTrainList().length(),Eq(1));
  EXPECT_THAT(nc->trackList.at(0)->isReversedTraffic(), Eq(true));
  EXPECT_THAT(nc->trainList.at(1)->getState(), Eq(1));//READY
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState() == 2); //RUNNING
  nc->stepTimeForNetwork(3);
  EXPECT_THAT(nc->trainList.at(1)->getState(), Eq(2));//RUNNING
}

//TEST_F(trackTest, trackLock)


#endif // _TST_TCTRACKTESTS_H_
