#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T11:54:56
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProtonClassicSuite
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp \
    dialogdisplaytree.cpp \
    pcx_treemodel.cpp \
    utility.cpp \
    dbutils.cpp

HEADERS  += mainwindow.h \
    dialogdisplaytree.h \
    pcx_treemodel.h \
    utility.h \
    dbutils.h

FORMS    += mainwindow.ui \
    dialogdisplaytree.ui
