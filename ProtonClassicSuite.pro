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
    utility.cpp \
    dbutils.cpp \
    pcx_treemodel.cpp \
    pcx_typemodel.cpp \
    dialogedittree.cpp

HEADERS  += mainwindow.h \
    utility.h \
    dbutils.h \
    pcx_treemodel.h \
    pcx_typemodel.h \
    dialogedittree.h

FORMS    += mainwindow.ui \
    dialogedittree.ui
