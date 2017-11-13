#ifndef TST_TCTRAINTESTS_H
#define TST_TCTRAINTESTS_H

#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace testing;

class trainTest : public networkTest{
};

TEST_F(trainTest, nbrOfPassengersAddRemove)
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  EXPECT_THAT(nc->trainList.at(0)->getNbrOfPassengers(), Eq(0));
  int deniedPassengers = nc->trainList.at(0)->load(3);
  EXPECT_THAT(nc->trainList.at(0)->getNbrOfPassengers(), Eq(3));
  EXPECT_THAT(deniedPassengers, Eq(0));
  deniedPassengers = nc->trainList.at(0)->load(-4);
  EXPECT_THAT(nc->trainList.at(0)->getNbrOfPassengers(), Eq(0));
  EXPECT_THAT(deniedPassengers, Eq(-1));
}

TEST_F(trainTest, nbrOfPassengersVsCapacity)
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  int capacity = nc->trainList.at(0)->getPassengerCapacity();
  int denied = nc->trainList.at(0)->load(4);
  denied = nc->trainList.at(0)->load(capacity);
  EXPECT_THAT(denied, Eq(4));
  EXPECT_THAT(nc->trainList.at(0)->getNbrOfPassengers(), Eq(capacity));
}

TEST_F(trainTest, noCurrentStationAfterCreation)
{ /* Verify that the train has no current station after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  EXPECT_THAT(nc->trainList.at(0)->getCurrentStation(), Eq(UNDEFINED));
}

TEST_F(trainTest, noCurrentTrackAfterCreation)
{ /* Verify that the train has no current track after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  EXPECT_THAT(nc->trainList.at(0)->getCurrentTrack(), Eq(UNDEFINED));
}

TEST_F(trainTest, travelPlanEmptyAfterCreation)
{ /* Verify that the travel plan iS empty after creation */
  nc->parseCmd("ADD TRAIN testTrain");
  EXPECT_THAT(nc->trainList.at(0)->getTravelPlan().length(), Eq(0));
}

#endif // TST_TCTRAINTESTS_H
