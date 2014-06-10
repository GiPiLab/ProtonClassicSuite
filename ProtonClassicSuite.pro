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
    pcx_treemodel.cpp \
    pcx_typemodel.cpp \
    pcx_auditmodel.cpp \
    pcx_auditinfos.cpp \
    auditdatadelegate.cpp \
    utils.cpp \
    pcx_auditmodel_tables.cpp \
    QCustomPlot/qcustomplot.cpp \
    pcx_auditmodel_graphics.cpp \
    formtablesgraphics.cpp \
    formdisplaytree.cpp \
    formedittree.cpp \
    formeditaudit.cpp \
    formmanageaudits.cpp

HEADERS  += mainwindow.h \
    pcx_treemodel.h \
    pcx_typemodel.h \
    pcx_auditmodel.h \
    pcx_auditinfos.h \
    auditdatadelegate.h \
    utils.h \
    QCustomPlot/qcustomplot.h \
    formtablesgraphics.h \
    formdisplaytree.h \
    formedittree.h \
    formeditaudit.h \
    formmanageaudits.h

FORMS    += mainwindow.ui \
    formtablesgraphics.ui \
    formdisplaytree.ui \
    formedittree.ui \
    formeditaudit.ui \
    formmanageaudits.ui
