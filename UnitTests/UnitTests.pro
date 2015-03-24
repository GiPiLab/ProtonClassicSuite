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

SOURCES += \
    tst_protonclassicsuiteunittests_main.cpp \
    tst_protonclassicsuiteunittests_tree.cpp \
    tst_protonclassicsuiteunittests_audit.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tst_protonclassicsuiteunittests.h
