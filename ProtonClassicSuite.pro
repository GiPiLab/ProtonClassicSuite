#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T11:54:56
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProtonClassicSuite
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
    utility.cpp \
    dbutils.cpp \
    pcx_treemodel.cpp \
    pcx_typemodel.cpp \
    dialogedittree.cpp \
    dialogdisplaytree.cpp \
    dialogmanageaudits.cpp \
    pcx_auditmodel.cpp \
    dialogeditaudit.cpp \
    pcx_auditinfos.cpp

HEADERS  += mainwindow.h \
    utility.h \
    dbutils.h \
    pcx_treemodel.h \
    pcx_typemodel.h \
    dialogedittree.h \
    dialogdisplaytree.h \
    dialogmanageaudits.h \
    pcx_auditmodel.h \
    dialogeditaudit.h \
    pcx_auditinfos.h

FORMS    += mainwindow.ui \
    dialogedittree.ui \
    dialogdisplaytree.ui \
    dialogmanageaudits.ui \
    dialogeditaudit.ui
