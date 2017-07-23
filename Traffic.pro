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
TEMPLATE = app

TARGET = Traffic

SOURCES += src/main.cpp \
    src/trafficcontrol.cpp \
    src/trafficdatamodel.cpp \
    src/tctrain.cpp \
    src/tctrack.cpp \
    src/tcstation.cpp \
    src/trafficclock.cpp \
    src/trafficcontrolImportNW.cpp

debug {
#    TARGET = Traffic_test
    QT += testlib
    SOURCES -= src/main.cpp
    SOURCES += test/tst_traffic.cpp
}

HEADERS  += inc/trafficcontrol.h \
    inc/tcstation.h \
    inc/tctrack.h \
    inc/tctrain.h \
    inc/trafficclock.h \
    inc/trafficdatamodel.h

FORMS    += trafficcontrol.ui

OTHER_FILES  =

DISTFILES += \
    map.qml

RESOURCES += \
    resources.qrc
