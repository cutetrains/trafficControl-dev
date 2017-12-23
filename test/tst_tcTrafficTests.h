#ifndef _TST_TCTRAFFICTESTS_H_
#define _TST_TCTRAFFICTESTS_H_

#define UNDEFINED -1

using namespace testing;

class trafficTest : public networkTest{
};

// TravelPlan tests
//setTrackPosition = 0 test

TEST_F(trafficTest, regularTrackTest)
{ /* Verify that track is marked as reversed if a train enters a track in
   * regular direction.
   * stationA -> stationB
   * trainAB->
   */
  nc->parseCmd("ADD STATION stationA");
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD TRACK trackAB 1234");
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationA TO stationB"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackAB FROM stationB TO stationA"), Eq(true));
  nc->parseCmd("ADD TRAIN trainAB");
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationA"), Eq(true));
  EXPECT_THAT(nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB"), Eq(true));
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState()!=2); //RUNNING
  EXPECT_THAT(nc->trainList.at(0)->getCurrentTrack(), Eq(0));
  EXPECT_THAT(nc->trainList.at(0)->getTrackPosition(), Eq(0));
  EXPECT_THAT(nc->trainList.at(0)->getCurrentStation(), Eq(-1));
  EXPECT_THAT(nc->trackList.at(0)->isReversedTraffic(), Eq(false));
  nc->stepTimeForNetwork(1);
  EXPECT_THAT(nc->trainList.at(0)->getTrackPosition(), Eq(1));
}

TEST_F(trafficTest, reversedTrackTest)
{ /* Verify that track is marked as reversed if a train enters a track in
   * opposite direction.
   * stationB -> stationC
   *            <-trainBC
   */
  nc->parseCmd("ADD STATION stationB");
  nc->parseCmd("ADD STATION stationC");
  nc->parseCmd("ADD TRACK trackBC 1234");
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackBC FROM stationB TO stationC"), Eq(true));
  EXPECT_THAT(nc->parseCmd("CONNECT TRACK trackBC FROM stationC TO stationB"), Eq(true));
  nc->parseCmd("ADD TRAIN trainCB");
  EXPECT_THAT(nc->parseCmd("TRAIN SET CURRENT STATION stationC"), Eq(true));
  nc->parseCmd("TRAIN TRAVELPLAN ADD STATION stationB");
  do{
    nc->stepTimeForNetwork(1);
  }
  while (nc->trainList.at(0)->getState()!=2); //RUNNING
  EXPECT_THAT(nc->trainList.at(0)->getCurrentTrack(), Eq(0));
  EXPECT_THAT(nc->trainList.at(0)->getTrackPosition(),
              Eq(nc->trackList.at(0)->getLength()));

  EXPECT_THAT(nc->trainList.at(0)->getCurrentStation(), Eq(-1));

  EXPECT_THAT(nc->trackList.at(0)->isReversedTraffic(), Eq(true));
  nc->stepTimeForNetwork(1);

  EXPECT_THAT(nc->trainList.at(0)->getTrackPosition(),
              Eq(nc->trackList.at(0)->getLength() - 1));
}

//TEST_F(trafficTest, fastTrainBehindSlowTrainOnTrack)

#endif //_TST_TCTRAFFICTESTS_H_
