#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T20:50:52
#
#-------------------------------------------------

QT       += quick
QT       += core gui
QT       += qml
QT       += quickwidgets
QT       += positioning
QT       += location

RC_FILE = resources/trafficControl.rc

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
    src/tcnetworkcontrol.cpp \
    src/tcnetworkcontrolcommandparser.cpp

#UNCOMMENT SECTION BELOW TO TEST
#debug {
#    GOOGLETEST_DIR = $$PWD/../googletest
#    include(gtest_dependency.pri)
#    CONFIG += console c++11
#    CONFIG -= app_bundle
#    CONFIG += thread
#    SOURCES -= src/main.cpp
#    SOURCES +=     test/mainTest.cpp
#    HEADERS +=     test/tst_tcStationTests.h \
#    test/tst_tcNetworkTests.h \
#    test/tst_tcTrainTests.h \
#    test/tst_tcTrackTests.h \
#    test/tst_tcTrafficTests.h
#}

HEADERS  += inc/trafficcontrol.h \
    inc/tcstation.h \
    inc/tctrack.h \
    inc/tctrain.h \
    inc/trafficclock.h \
    inc/trafficdatamodel.h \
    inc/tcnetworkcontrol.h

FORMS    += trafficcontrol.ui

OTHER_FILES  =

DISTFILES += \
    map.qml

RESOURCES += \
    resources.qrc
