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
    utility.cpp \
    dbutils.cpp \
    types.cpp \
    pcx_treemodel.cpp \
    dialogaddchangetreenode.cpp

HEADERS  += mainwindow.h \
    dialogdisplaytree.h \
    utility.h \
    dbutils.h \
    types.h \
    pcx_treemodel.h \
    dialogaddchangetreenode.h

FORMS    += mainwindow.ui \
    dialogdisplaytree.ui \
    dialogaddchangetreenode.ui
