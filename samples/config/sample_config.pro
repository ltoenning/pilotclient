QT       += core network
QT       -= gui

TARGET = sample_config
TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp

LIBS    += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin



