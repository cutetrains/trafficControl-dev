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

//TEST_F(trackTest, trainListAddRmUpstream)
//{ /* Verify that it is possible to add and remove trains on trainList in a
//   * FIFO manner.
//   * /
//}

//TEST_F(trackTest, trainListAddRmDownstream)
//{ /* Verify that it is possible to add and remove trains on trainList in a
//   * FIFO manner, rejecting Upstream trains if Downstream is used.
//   * /
//}

#endif // _TST_TCTRACKTESTS_H_
