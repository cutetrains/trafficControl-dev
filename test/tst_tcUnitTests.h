#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "inc/trafficcontrol.h"

using namespace testing;

TEST(stationTest, noPassengersAfterCreated)
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
  ASSERT_THAT(newStation->getNbrOfWaitingPassengers(), Eq(0));
  delete newStation;
}
