#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T11:54:56
#
#-------------------------------------------------

QT       += core gui sql printsupport

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProtonClassicSuite
TEMPLATE = app

include(QtXlsx/xlsx/qtxlsx.pri)

SOURCES += main.cpp\
	mainwindow.cpp \
    auditdatadelegate.cpp \
    utils.cpp \
    QCustomPlot/qcustomplot.cpp \
    formdisplaytree.cpp \
    formedittree.cpp \
    formeditaudit.cpp \
    formmanageaudits.cpp \
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
    pcx_stringdistance.cpp \
    formtreeconsistency.cpp \
    pcx_nodesimilaritytablemodel.cpp \
    pcx_tree.cpp \
    pcx_treemodel.cpp \
    pcx_auditwithtreemodel.cpp \
    pcx_reporting.cpp \
    formmanagereportings.cpp \
    pcx_reportingwithtreemodel.cpp \
    pcx_reportingtableoverviewmodel.cpp \
    pcx_reportingtablesupervisionmodel.cpp \
    formreportingsupervision.cpp \
    formreportingoverview.cpp \
    formreportinggraphics.cpp \
    formreportingexplore.cpp \
    formreportingreports.cpp \
    dialogabout.cpp \
    formauditexplore.cpp \
    formauditreports.cpp \
    pcx_prevision.cpp \
    pcx_previsioncriteria.cpp \
    formauditprevisions.cpp \
    formmanageprevisions.cpp

HEADERS  += mainwindow.h \
    auditdatadelegate.h \
    utils.h \
    QCustomPlot/qcustomplot.h \
    formdisplaytree.h \
    formedittree.h \
    formeditaudit.h \
    formmanageaudits.h \
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
    pcx_stringdistance.h \
    formtreeconsistency.h \
    pcx_nodesimilaritytablemodel.h \
    pcx_tree.h \
    pcx_treemodel.h \
    pcx_auditwithtreemodel.h \
    pcx_reporting.h \
    formmanagereportings.h \
    pcx_reportingwithtreemodel.h \
    pcx_reportingtableoverviewmodel.h \
    pcx_reportingtablesupervisionmodel.h \
    formreportingsupervision.h \
    formreportingoverview.h \
    formreportinggraphics.h \
    formreportingexplore.h \
    formreportingreports.h \
    dialogabout.h \
    formauditexplore.h \
    formauditreports.h \
    pcx_prevision.h \
    pcx_previsioncriteria.h \
    formauditprevisions.h \
    formmanageprevisions.h

FORMS    += mainwindow.ui \
    formdisplaytree.ui \
    formedittree.ui \
    formeditaudit.ui \
    formmanageaudits.ui \
    dialogoptions.ui \
    formqueries.ui \
    dialogduplicateaudit.ui \
    formauditinfos.ui \
    formtreeconsistency.ui \
    formmanagereportings.ui \
    formreportingsupervision.ui \
    formreportingoverview.ui \
    formreportinggraphics.ui \
    formreportingexplore.ui \
    formreportingreports.ui \
    dialogabout.ui \
    formauditexplore.ui \
    formauditreports.ui \
    formauditprevisions.ui \
    formmanageprevisions.ui

RC_ICONS = icons/logo64_64.ico
#QMAKE_CXXFLAGS_DEBUG += -Og

unix|win32: LIBS += -lgvc -lcgraph

win32: INCLUDEPATH += "c:/Program Files/Graphviz/include"

win32:CONFIG(release, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraph
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraphd

win32:INCLUDEPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"
win32:DEPENDPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"

RESOURCES += \
    Images.qrc

