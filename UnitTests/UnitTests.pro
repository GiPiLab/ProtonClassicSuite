#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T12:51:37
#
#-------------------------------------------------

QT       += testlib



include(../ProtonClassicSuite_forUnitTests.pri)

#QT       -= gui

TARGET = tst_unittests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS+=-Og --coverage
QMAKE_LFLAGS+=--coverage

SOURCES += tst_unittests.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"
