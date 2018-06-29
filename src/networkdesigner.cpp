#include "../inc/networkdesigner.h"

NetworkDesigner::NetworkDesigner(QObject *parent) : QObject(parent)
{
}

/*!
 * Updates the TrafficNetworkMap textbox in user interface.
 * TNM is generated from KML file using KML parser.
 *
 * @param tnmFile the generated TrafficNetworkMap string
 */
bool NetworkDesigner::convertKmlToTnm(QString kmlFileName)
{
  QString  program( "python.exe " );
  QProcess p;
  QStringList args = QStringList();
  //HARDCODED, MUST BE GENERIC!!!
  args << "C:/Users/gusta/GIT/trafficControl-dev/scripts/KMLParser/TrackStationConnector.py";
  args <<kmlFileName;
  p.start(program,args);
  p.waitForFinished(-1);
  QString trafficNetworkMapFile = p.readAll();
  emit kmlToTnmConversionDone(trafficNetworkMapFile);

  //TODO: ANALYSE TNM OUTPUT AND SEND REPORT TO USER INTERFACE. THIS CAN BE ADDED TO THE EXISTING SIGNAL
  return false;
}

bool NetworkDesigner::importTno(QString tnoFileName)
{
    QFile file(tnoFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug()<<"ERROR  : Could not open file.";
    }

    QTextStream in(&file);
    QStringList fileStrings;
    while(!in.atEnd())
    {
      fileStrings << in.readLine();
    }
    file.close();
    qDebug()<<"FILE: "<<fileStrings;
    emit tnoFileOpened(fileStrings.join("\r\n"));
    return false;
}
