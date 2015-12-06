#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T20:50:52
#
#-------------------------------------------------

QT       += quick
QT       += core gui

QT       += quickwidgets
QT       += positioning
QT       += location

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Traffic
TEMPLATE = app

SOURCES += main.cpp\
        trafficcontrol.cpp \
    trafficdatamodel.cpp \
    tctrain.cpp \
    tctrack.cpp \
    tcstation.cpp \
    trafficcontrol_importNW.cpp \
    trafficclock.cpp \
    trafficmap.cpp

HEADERS  += trafficcontrol.h \
    trafficdatamodel.h \
    tctrain.h \
    tctrack.h \
    tcstation.h \
    trafficclock.h \
    trafficmap.h

FORMS    += trafficcontrol.ui

OTHER_FILES  =

DISTFILES += \
    map.qml

RESOURCES += \
    resources.qrc
