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
#include "../inc/TrafficControl.h"
#include "ui_TrafficControl.h"
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
  ui->nwFileDockWidget->hide();//Investigate

  fastForwardSpinBox = new QDoubleSpinBox(this);

  ui->toolBar->addWidget(fastForwardSpinBox); //fastForwfastForwardSpinBoxardSpinBox
  fastForwardSpinBox->setMaximum(50);
  fastForwardSpinBox->setMinimum(0.5);
  fastForwardSpinBox->setSingleStep(0.5);
  fastForwardSpinBox->setValue(1);
  fastForwardSpinBox->setDecimals(1);
  fastForwardSpinBox->setSuffix("x");
  fastForwardSpinBox->setToolTip("Set the fast forward speed.");

  simulatedTimeLabel  = new QLabel(this);
  simulatedTimeLabel->setText("tt:mm:ss");
  ui->statusBar->addWidget(simulatedTimeLabel);
  calculationTimeLabel = new QLabel(this);//calculationTimeLabel and simulatedTimeLabel
  calculationTimeLabel->setText("aaa ms ( bbb % )");
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

  networkControl = new NetworkControl(*trackListModel,
                                      *trainListModel,
                                      *stationListModel,
                                      *handleQMLObject);

  connect(ui->actionImport_Network_File, SIGNAL(triggered(bool)), this, SLOT(readNetworkDefinitionFromFile()));

  connect(ui->actionStep_Simulation, SIGNAL(triggered(bool)), networkControl, SLOT(stepTimeForNetwork()));

  ui->actionToggle_Simulation->setChecked(true);
  connect(ui->actionToggle_Simulation, SIGNAL(toggled(bool)), this, SLOT(onPlayStopButtonClicked(bool)));

  connect(fastForwardSpinBox, SIGNAL(valueChanged(double)), networkControl, SLOT(onFastForwardSpeedChanged(double)));

  connect(networkControl, SIGNAL(updateCalculationLoad(int)), this, SLOT(updateCalculationTime(int)));
  connect(networkControl, SIGNAL(updateSimulatedTimeSignalLabel(QString)), this, SLOT(updateSimulatedTimeLabel(QString)));
  ui->stationListTableView->resizeColumnsToContents();
  ui->trackListTableView->resizeColumnsToContents();
  ui->trainListTableView->resizeColumnsToContents();
}

/*!
* Opens a file and parses the commands to networkControl
*
* @return result
*/
bool TrafficControl::readNetworkDefinitionFromFile()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"),
                                                  "C://temp//train//",
                                                  tr("Files (*.*)"));
  QFile file(fileName);
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
  networkControl->parseMultipleNetworkCommand(fileStrings);

  return true;
}

void TrafficControl::updateSimulatedTimeLabel(QString sTime)
{
  simulatedTimeLabel->setText(sTime);
}

void TrafficControl::updateCalculationTime(int calculationTimeMs){//This can be merged with updateSimulatedTimeLabel
  // 100*calculationTimeMs/1000*speedFactor
  // Example: 20 ms / 500 ms: 100*20/1000*2 = 4000/1000 = 4%
  int systemLoad = (int) calculationTimeMs / 10 * fastForwardSpinBox->value();
  calculationTimeLabel->setText(QString::number(calculationTimeMs)+ " ms ( "+QString::number(systemLoad)+" % )");
}

/*
 * isChecked is TRUE if the simulation is paused.
 * Icon credit:
 * http://icons8.com/
 */
void TrafficControl::onPlayStopButtonClicked(bool isPaused){
  if(true == isPaused) {
    ui->actionToggle_Simulation->setIcon(QIcon(":/resources/Media-Controls-Play-icon.png"));
  } else {
    ui->actionToggle_Simulation->setIcon(QIcon(":/resources/Media-Controls-Pause-icon.png"));
  }
  networkControl->setSimulationPaused(isPaused);
}

/*!
 * The destructor method
 * @TODO Investigate what objects ad data structures shall be removed at
 * termination of the program.
 *
 * @TODO Investigate dependencies when removing objects
 */
TrafficControl::~TrafficControl()
{
  delete trackListModel;
  delete trainListModel;
  delete stationListModel;
  delete networkControl;
  delete ui;
}
