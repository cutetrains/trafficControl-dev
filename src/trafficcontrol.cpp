/*************************************************************************
 * This file is part of TrafficControl.
 *
 * TrafficControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TrafficControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TrafficControl.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QDesktopServices>//Delete?
#include "../inc/trafficcontrol.h"
#include "ui_trafficcontrol.h"
#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace std;

TrafficControl::TrafficControl(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TrafficControl)
{
  ui->setupUi(this);
  qInfo()<<"Setting up UI";
  //TODO: COLLECT SETTINGS IN SEPARATE FUNCTIONS
  fastForwardSpinBox = new QDoubleSpinBox(this);
  fastForwardSpinBox->setMaximum(50);//TODO: use yaml file for settings?
  fastForwardSpinBox->setMinimum(0.5);
  fastForwardSpinBox->setSingleStep(0.5);
  fastForwardSpinBox->setValue(1);
  fastForwardSpinBox->setDecimals(1);
  fastForwardSpinBox->setSuffix("x");
  fastForwardSpinBox->setToolTip("Set the fast forward speed.");
  ui->toolBar->addWidget(fastForwardSpinBox);

  simulatedTimeLabel  = new QLabel(this);
  ui->statusBar->addWidget(simulatedTimeLabel);
  calculationTimeLabel = new QLabel(this);
  ui->statusBar->addWidget(calculationTimeLabel);

  trackListModel=new TrafficDataModel(TRACK, 0);
  trainListModel=new TrafficDataModel(TRAIN, 0);
  stationListModel=new TrafficDataModel(STATION, 0);
  ui->trackListTableView->setModel(trackListModel);
  ui->trackListTableView->show();
  ui->trainListTableView->setModel(trainListModel);
  ui->trainListTableView->show();
  ui->stationListTableView->setModel(stationListModel);
  ui->stationListTableView->show();

  QUrl source("qrc:Traffic/qml/map.qml");
  ui->mapQuickWidget->setSource(source);
  handleQMLObject = ui->mapQuickWidget->rootObject();

  ui->mapDockWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::MinimumExpanding);

  ui->actionMapToggle->setChecked(true);
  ui->actionTrackToggle->setChecked(true);
  ui->actionStationToggle->setChecked(true);
  ui->actionTrainToggle->setChecked(true);
  ui->actionDesignToggle->setChecked(true);
  ui->mapDockWidget->setFloating(true);
  ui->mapDockWidget->move(100,230);
  ui->mapDockWidget->resize(1000,700);
  ui->mapDockWidget->updateGeometry();

  networkControl = new NetworkControl(*trackListModel,
                                      *trainListModel,
                                      *stationListModel,
                                      *handleQMLObject);
  networkDesigner = new NetworkDesigner();
  connectSignalSlot();

  ui->stationListTableView->resizeColumnsToContents();
  ui->trackListTableView->resizeColumnsToContents();
  ui->trainListTableView->resizeColumnsToContents();
}

void TrafficControl::connectSignalSlot()
{
  connect(ui->actionImport_Network_File, SIGNAL(triggered(bool)),
          this, SLOT(readNetworkDefinitionFromFile()));
  connect(ui->actionStep_Simulation, SIGNAL(triggered(bool)),
          networkControl, SLOT(stepTimeForNetwork()));
  connect(ui->actionToggle_Simulation, SIGNAL(toggled(bool)),
          this, SLOT(onPlayStopButtonClicked(bool)));

  connect(ui->actionMapToggle, SIGNAL(toggled(bool)),
          this, SLOT(onToggleMapDockWidget(bool)));
  connect(ui->mapDockWidget, SIGNAL(visibilityChanged(bool)),
          this, SLOT(onToggleMapDockWidget(bool)));

  connect(ui->actionTrackToggle, SIGNAL(toggled(bool)),
          this, SLOT(onToggleTrackDockWidget(bool)));
  connect(ui->trackDockWidget, SIGNAL(visibilityChanged(bool)),
          this, SLOT(onToggleTrackDockWidget(bool)));

  connect(ui->actionStationToggle, SIGNAL(toggled(bool)),
          this, SLOT(onToggleStationDockWidget(bool)));
  connect(ui->stationDockWidget, SIGNAL(visibilityChanged(bool)),
          this, SLOT(onToggleStationDockWidget(bool)));

  connect(ui->actionTrainToggle, SIGNAL(toggled(bool)),
          this, SLOT(onToggleTrainDockWidget(bool)));
  connect(ui->trainDockWidget, SIGNAL(visibilityChanged(bool)),
          this, SLOT(onToggleTrainDockWidget(bool)));

  connect(ui->actionDesignToggle, SIGNAL(toggled(bool)),
         this, SLOT(onToggleDesignDockWidget(bool)));
  connect(ui->designDockWidget, SIGNAL(visibilityChanged(bool)),
          this, SLOT(onToggleDesignDockWidget(bool)));

  connect(ui->actionImportKMLFile, SIGNAL(triggered(bool)),
          this, SLOT(onOpenKmlFile()));
  connect(ui->importKmlPushButton, SIGNAL(clicked(bool)),
          this, SLOT(onOpenKmlFile()));

  connect(ui->actionSaveTnfFile, SIGNAL(triggered(bool)),
          this, SLOT(onSaveTnfFile()));
  connect(ui->saveTnfPushButton, SIGNAL(clicked(bool)),
          this, SLOT(onSaveTnfFile()));

  connect(ui->actionOpenTnoFile, SIGNAL(triggered(bool)),
          this, SLOT(onOpenTnoFile()));
  connect(ui->importTnoPushButton, SIGNAL(clicked(bool)),
          this, SLOT(onOpenTnoFile()));

  connect(fastForwardSpinBox, SIGNAL(valueChanged(double)),
          networkControl, SLOT(onFastForwardSpeedChanged(double)));
  connect(networkControl, SIGNAL(updateCalculationLoad(int)),
          this, SLOT(updateCalculationTime(int)));
  connect(networkControl, SIGNAL(updateSimulatedTimeSignalLabel(QString)),
          this, SLOT(updateSimulatedTimeLabel(QString)));

  connect(networkDesigner, SIGNAL(kmlToTnmConversionDone(QString)),
          this, SLOT(updateTnmTextBox(QString)));

  connect(networkDesigner, SIGNAL(tnoFileOpened(QString)),
          this, SLOT(updateTnoTextBox(QString)));

  connect(ui->actionHelpOverview, SIGNAL(triggered(bool)),
          this, SLOT(onHelpOverviewSelected()));
}

void TrafficControl::onHelpOverviewSelected()
{
  QDesktopServices::openUrl(QUrl("http://cutetrains.blogspot.com/p/trafficcontrol-overview-of-program.html"));
}

/*!
 * isChecked is TRUE if the simulation is paused.
 * Icon credit:
 * http://icons8.com/
 *
 * @param isPaused True to pause, False to resume
 */
void TrafficControl::onPlayStopButtonClicked(bool isPaused){
  // If the simulation isn't paused (isPaused=FALSE), the step button should be disabled (setEnabled = FALSE)
  ui->actionStep_Simulation->setEnabled(isPaused);
  ui->actionImport_Network_File->setEnabled(isPaused);
  if(true ==  isPaused) {
    ui->actionToggle_Simulation->setText("Resume Simulation");
    ui->actionToggle_Simulation->setToolTip("Resume Simulation");
  } else {
    ui->actionToggle_Simulation->setText("Pause Simulation");
    ui->actionToggle_Simulation->setToolTip("Pause Simulation");
  }
  networkControl->setSimulationPaused(isPaused);
}

/*!
 * Toggles the design window
 *
 * @param isVisible TRUE if the window should be visible
 */
void TrafficControl::onToggleDesignDockWidget(bool isVisible){
  ui->actionDesignToggle->setChecked(isVisible);
  ui->designDockWidget->setVisible(isVisible);
}

/*!
 * Toggles the map window
 *
 * @param isVisible TRUE if the window should be visible
 */
void TrafficControl::onToggleMapDockWidget(bool isVisible){
  ui->actionMapToggle->setChecked(isVisible);
  ui->mapDockWidget->setVisible(isVisible);
}

/*!
 * Toggles the train window
 *
 * @param isVisible TRUE if the window should be visible
 */
void TrafficControl::onToggleTrainDockWidget(bool isVisible){
  ui->actionTrainToggle->setChecked(isVisible);
  ui->trainDockWidget->setVisible(isVisible);
}

/*!
 * Toggles the track window
 *
 * @param isVisible TRUE if the window should be visible
 */
void TrafficControl::onToggleTrackDockWidget(bool isVisible){
  ui->actionTrackToggle->setChecked(isVisible);
  ui->trackDockWidget->setVisible(isVisible);
}

/*!
 * Toggles the station window
 *
 * @param isVisible TRUE if the window should be visible
 */
void TrafficControl::onToggleStationDockWidget(bool isVisible){
  ui->actionStationToggle->setChecked(isVisible);
  ui->stationDockWidget->setVisible(isVisible);
}

/*!
 * Open a KML file and send the contents to tcNetworkDesigner, if found
 *
 * @return TRUE if file OK
 */
bool TrafficControl::onOpenKmlFile(){
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open KML, TNF or TNM File"),
                                                  QDir::homePath(),
                                                  tr("Files (*.kml *.tnf *.tnm)"));
  QFileInfo fi(fileName);
  QString suffix = fi.suffix();
  if(QString::compare(suffix, "kml", Qt::CaseInsensitive ) == 0  ){
    networkDesigner->convertKmlToTnm(fileName);
  } else {
    networkDesigner->importTnmOrTnf(fileName);
  }

  return false;
}

/*!
 * Open a TNO file and send the contents to tcNetworkDesigner, if found
 *
 * @return TRUE if file OK
 */
bool TrafficControl::onOpenTnoFile(){
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Traffic Network Operations (TNO) File"),
                                                  QDir::homePath(),
                                                  tr("Files (*.tno)"));
  qInfo()<<fileName;
  networkDesigner->importTno(fileName);
  return false;
}

/*!
 * Save he contents of the two textEdif fields to a TNF (Traffic Network File)
 *
 * @return TRUE if file OK
 */
bool TrafficControl::onSaveTnfFile()
{
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save Traffic Network File (TNF)"),
                                                  QDir::homePath(),
                                                  tr("Files (*.tnf)"));
  qDebug()<<fileName;
  qDebug()<<ui->trafficNetworkMapTextEdit->toPlainText()+ui->trafficNetworkOperationTextEdit->toPlainText();
  QFile f( fileName );
  if(!f.open( QIODevice::WriteOnly  | QIODevice::Text ))
  {
    qDebug()<<"ERROR  : Could not open file."; // TODO: Add popup window with error message? Check the caller function!
    return false;
  }
  QTextStream stream(&f);
  stream << ui->trafficNetworkMapTextEdit->toPlainText();
  stream << ui->trafficNetworkOperationTextEdit->toPlainText();
  stream.flush();
  f.close();
  return true;
}

/*!
 * Opens a file and parses the commands to networkControl
 *
 * @return result True for succes
 */
bool TrafficControl::readNetworkDefinitionFromFile()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Train Network File"),
                                                  QDir::homePath(),
                                                  tr("Files (*.*)"));
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug()<<"ERROR  : Could not open file.";// TODO Create a pop up window?
    return false;
  }

  QTextStream in(&file);
  QStringList fileStrings;
  while(!in.atEnd())
  {
    fileStrings << in.readLine();
  }
  file.close();
  networkControl->parseMultipleNetworkCommand(fileStrings);

  return true;
}

/*!
 * Updates the label for simulated time
 *
 * @param sTime the new simulated time
 */
void TrafficControl::updateSimulatedTimeLabel(QString sTime)
{
  simulatedTimeLabel->setText(sTime);
}

/*!
 * Updates the TrafficNetworkMap textbox in user interface.
 * TNM is generated from KML file using KML parser.
 *
 * @param tnmFile the generated TrafficNetworkMap string
 */
void TrafficControl::updateTnmTextBox(QString tnmFile)
{
  ui->trafficNetworkMapTextEdit->setText(tnmFile);

  //TODO CALL NW CONTROL FROM NETWORKDESIGNER USING SIGNALS/SLOTS
  QStringList tnmFileSplitted = tnmFile.split("\r\n", Qt::SkipEmptyParts);
  QString s;
  foreach(s, tnmFileSplitted){
    networkControl->parseCmd(s);
  }
}

/*!
 * Updates the TrafficNetworkOperation textbox in user interface.
 * TNO is created manually
 *
 * @param tnoFile Traffic Network Operations file
 */
void TrafficControl::updateTnoTextBox(QString tnoFile)
{
  ui->trafficNetworkOperationTextEdit->setText(tnoFile);

  QStringList tnoFileSplitted = tnoFile.split("\r\n", Qt::SkipEmptyParts);

  QString s;
  foreach(s, tnoFileSplitted){
    networkControl->parseCmd(s);
  }
}

/*!
* Prints the time needed for the last calculation, both in milliseconds and as a percentage
* of the last interval
*
* @param calculationTimeMs the time needed for the last calculation (updating trains, tracks and
* stations.
*/
void TrafficControl::updateCalculationTime(int calculationTimeMs){
  // 100*calculationTimeMs/1000*speedFactor
  // Example: 20 ms / 500 ms: 100*20/1000*2 = 4000/1000 = 4%
  int systemLoad = (int) calculationTimeMs / 10 * fastForwardSpinBox->value();
  calculationTimeLabel->setText(QString::number(calculationTimeMs)+ " ms ( "+QString::number(systemLoad)+" % )");
}

/*!
 * The destructor method
 * @todo Investigate what objects ad data structures shall be removed at
 * termination of the program.
 *
 * @todo Investigate dependencies when removing objects
 */
TrafficControl::~TrafficControl()
{
  delete networkDesigner;
  delete trackListModel;
  delete trainListModel;
  delete stationListModel;
  delete networkControl;
  delete fastForwardSpinBox;
  delete simulatedTimeLabel;
  delete calculationTimeLabel;

  delete ui;
}
