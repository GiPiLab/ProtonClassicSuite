#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T11:54:56
#
#-------------------------------------------------

QT       += core gui sql printsupport

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProtonClassicSuite
TEMPLATE = app

include(QtXlsx/xlsx/qtxlsx.pri)

SOURCES += main.cpp\
	mainwindow.cpp \
    pcx_treemodel.cpp \
    pcx_typemodel.cpp \
    auditdatadelegate.cpp \
    utils.cpp \
    QCustomPlot/qcustomplot.cpp \
    formtablesgraphics.cpp \
    formdisplaytree.cpp \
    formedittree.cpp \
    formeditaudit.cpp \
    formmanageaudits.cpp \
    formreports.cpp \
    dialogoptions.cpp \
    pcx_tables.cpp \
    pcx_report.cpp \
    pcx_graphics.cpp \
    formqueries.cpp \
    pcx_queryvariation.cpp \
    pcx_queriesmodel.cpp \
    pcx_query.cpp \
    pcx_queryrank.cpp \
    pcx_queryminmax.cpp \
    dialogduplicateaudit.cpp \
    formauditinfos.cpp \
    pcx_editableauditmodel.cpp \
    pcx_audit.cpp \
    pcx_treemanage.cpp \
    pcx_auditmanage.cpp \
    pcx_stringdistancetablemodel.cpp \
    pcx_stringdistance.cpp

HEADERS  += mainwindow.h \
    pcx_treemodel.h \
    pcx_typemodel.h \
    auditdatadelegate.h \
    utils.h \
    QCustomPlot/qcustomplot.h \
    formtablesgraphics.h \
    formdisplaytree.h \
    formedittree.h \
    formeditaudit.h \
    formmanageaudits.h \
    formreports.h \
    dialogoptions.h \
    pcx_tables.h \
    pcx_report.h \
    pcx_graphics.h \
    formqueries.h \
    pcx_queryvariation.h \
    pcx_queriesmodel.h \
    pcx_query.h \
    pcx_queryrank.h \
    pcx_queryminmax.h \
    dialogduplicateaudit.h \
    formauditinfos.h \
    pcx_editableauditmodel.h \
    pcx_audit.h \
    pcx_treemanage.h \
    pcx_auditmanage.h \
    pcx_stringdistancetablemodel.h \
    pcx_stringdistance.h

FORMS    += mainwindow.ui \
    formtablesgraphics.ui \
    formdisplaytree.ui \
    formedittree.ui \
    formeditaudit.ui \
    formmanageaudits.ui \
    formreports.ui \
    dialogoptions.ui \
    formqueries.ui \
    dialogduplicateaudit.ui \
    formauditinfos.ui


QMAKE_CXXFLAGS_DEBUG += -Og

unix|win32: LIBS += -lgvc -lcgraph

win32: INCLUDEPATH += "c:/Program Files/Graphviz/include"

win32:CONFIG(release, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraph
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraphd

win32:INCLUDEPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"
win32:DEPENDPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"
