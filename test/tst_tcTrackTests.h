#ifndef TST_TCTRACKTESTS_H
#define TST_TCTRACKTESTS_H

#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace testing;

class trackTest : public networkTest{
};

TEST_F(trackTest, constructor_trackLength)
{ /* Verify that the train has no current station after creation */
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackA 1234"), Eq(true));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB -1234"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB 0"), Eq(false));
  EXPECT_THAT(nc->parseCmd("ADD TRACK trackB tjugo"), Eq(false));
}

#endif // TST_TCTRACKTESTS_H
