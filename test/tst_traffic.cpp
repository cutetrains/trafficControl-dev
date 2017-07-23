#include <QtTest>
#include <QList>
#include <QString>
#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include "../trafficControl-dev/inc/trafficcontrol.h"

//class TstTraffic : public QObject
class TstTraffic : public QMainWindow
{
    Q_OBJECT

public:
    TstTraffic();
    ~TstTraffic();

private slots:
    void initTestCase();
    void stationEmptyAfterCreated();
//    void junctionHasNoPassengersAfterAddingTen(); //GOOD TDD TEST CASE
    void tenPassengersAfterAddingTen();
    void noPassengersAfterAddingTenRemovingTwenty();
//    void oneTrainInTrainListModelAfterAddingATrain();
    void cleanupTestCase();
};

TstTraffic::TstTraffic()
{

}

TstTraffic::~TstTraffic()
{

}

void TstTraffic::initTestCase()
{

  //qDebug("called before everything else");

}

void TstTraffic::cleanupTestCase()
{
  qDebug("called after everything else");
}

void TstTraffic::stationEmptyAfterCreated()
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
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 0);
  delete newStation;
}

/*void TstTraffic::junctionHasNoPassengersAfterAddingTen()
{
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
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 0);
  delete newStation;
}*/

void TstTraffic::tenPassengersAfterAddingTen()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QVERIFY(0 == trackList.length());
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);
  newStation->changeNbrOfPassengers(10);
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 10);
}

void TstTraffic::noPassengersAfterAddingTenRemovingTwenty()
{
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  QVERIFY(0 == trackList.length());
  Station* newStation = new Station("testStation",
                                  false,
                                  "",
                                  "",
                                  trackList,
                                  trainList,
                                  stationList);

  newStation->changeNbrOfPassengers(10);
  newStation->changeNbrOfPassengers(-20);
  QVERIFY(newStation->getNbrOfWaitingPassengers() == 0);
}

/*void TstTraffic::oneTrainInTrainListModelAfterAddingATrain()
{
  TrafficControl tc;
  tc.addTrainToNetwork("Train1");
  QVERIFY(tc.getNumberOfTrains() ==1);
    tc.addTrainToNetwork("Train2");
  QVERIFY(tc.getNumberOfTrains() ==2);
}*/

QTEST_MAIN(TstTraffic)

#include "tst_traffic.moc"
