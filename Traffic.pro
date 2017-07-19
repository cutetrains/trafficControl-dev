#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T20:50:52
#
#-------------------------------------------------

#QT += qml network quick positioning location core gui quickwidgets
QT       += quick
QT       += core gui
QT       += qml
QT       += quickwidgets
QT       += positioning
QT       += location

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Traffic
TEMPLATE = app

SOURCES += src/main.cpp\
        src/trafficcontrol.cpp \
    src/trafficdatamodel.cpp \
    src/tctrain.cpp \
    src/tctrack.cpp \
    src/tcstation.cpp \
    src/trafficclock.cpp \
    src/trafficcontrolImportNW.cpp

HEADERS  += inc/trafficcontrol.h \
    inc/trafficdatamodel.h \
    inc/tctrain.h \
    inc/tctrack.h \
    inc/tcstation.h \
    inc/trafficclock.h

FORMS    += trafficcontrol.ui

OTHER_FILES  =

DISTFILES += \
    map.qml

RESOURCES += \
    resources.qrc
