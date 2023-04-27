#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T20:50:52
#
#-------------------------------------------------

QT       += quick core gui qml \
   quickwidgets positioning xml
#QT       += location
#QT       += network

RC_FILE = resources/trafficControl.rc
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

# install -delete!
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
INSTALLS += target

TARGET = Traffic

SOURCES += src/main.cpp \
    src/trafficcontrol.cpp \
    src/trafficdatamodel.cpp \
    src/tctrain.cpp \
    src/tctrack.cpp \
    src/tcstation.cpp \
    src/trafficclock.cpp \
    src/tcnetworkcontrol.cpp \
    src/tcnetworkcontrolcommandparser.cpp \
    src/networkdesigner.cpp \

#QTTEST
#debug {
#    QT += testlib
#    SOURCES -= src/main.cpp
#    SOURCES += \
#    test/tst_trafficcontrol.cpp
#}

HEADERS  += inc/trafficcontrol.h \
    inc/tcstation.h \
    inc/tctrack.h \
    inc/tctrain.h \
    inc/trafficclock.h \
    inc/trafficdatamodel.h \
    inc/tcnetworkcontrol.h \
    inc/networkdesigner.h
FORMS    += trafficcontrol.ui

OTHER_FILES  =

DISTFILES += \
    map.qml

RESOURCES += \
    resources.qrc
