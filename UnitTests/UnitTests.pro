#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T12:51:37
#
#-------------------------------------------------

QT       += testlib

DEFINES *= QT_USE_QSTRINGBUILDER

include(../ProtonClassicSuite_forUnitTests.pri)

#QT       -= gui

TARGET = tst_unittests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#QMAKE_CXXFLAGS+=-Og --coverage
#QMAKE_LFLAGS+=--coverage

SOURCES += utils.cpp \
    tst_protonclassicsuiteunittests_main.cpp \
    tst_protonclassicsuiteunittests_pcx_audit.cpp \
    tst_protonclassicsuiteunittests_pcx_tree.cpp \
    tst_protonclassicsuiteunittests_pcx_tablesandgraphics.cpp \
    tst_protonclassicsuiteunittests_pcx_queries.cpp \
    tst_protonclassicsuiteunittests_pcx_previsions.cpp \
    tst_protonclassicsuiteunittests_pcx_reporting.cpp

#DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += utils.h \
    tst_protonclassicsuiteunittests.h
