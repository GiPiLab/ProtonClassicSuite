QT       += core gui sql printsupport

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(QtXlsx/xlsx/qtxlsx.pri)



SOURCES +=  $$PWD/QCustomPlot/qcustomplot.cpp \
    $$PWD/pcx_tables.cpp \
    $$PWD/pcx_report.cpp \
    $$PWD/pcx_graphics.cpp \
    $$PWD/pcx_queryvariation.cpp \
    $$PWD/pcx_queriesmodel.cpp \
    $$PWD/pcx_query.cpp \
    $$PWD/pcx_queryrank.cpp \
    $$PWD/pcx_queryminmax.cpp \
    $$PWD/pcx_editableauditmodel.cpp \
    $$PWD/pcx_audit.cpp \
    $$PWD/pcx_stringdistance.cpp \
    $$PWD/pcx_nodesimilaritytablemodel.cpp \
    $$PWD/pcx_tree.cpp \
    $$PWD/pcx_treemodel.cpp \
    $$PWD/pcx_auditwithtreemodel.cpp \
    $$PWD/pcx_reporting.cpp \
    $$PWD/pcx_reportingwithtreemodel.cpp \
    $$PWD/pcx_reportingtableoverviewmodel.cpp \
    $$PWD/pcx_reportingtablesupervisionmodel.cpp \
    $$PWD/pcx_prevision.cpp \
    $$PWD/pcx_previsionitemcriteria.cpp \
    $$PWD/pcx_previsionitemtablemodel.cpp \
    $$PWD/utils.cpp \
    $$PWD/pcx_previsionitem.cpp



HEADERS  += $$PWD/QCustomPlot/qcustomplot.h \
    $$PWD/pcx_tables.h \
    $$PWD/pcx_report.h \
    $$PWD/pcx_graphics.h \
    $$PWD/pcx_queryvariation.h \
    $$PWD/pcx_queriesmodel.h \
    $$PWD/pcx_query.h \
    $$PWD/pcx_queryrank.h \
    $$PWD/pcx_queryminmax.h \
    $$PWD/pcx_editableauditmodel.h \
    $$PWD/pcx_audit.h \
    $$PWD/pcx_stringdistance.h \
    $$PWD/pcx_nodesimilaritytablemodel.h \
    $$PWD/pcx_tree.h \
    $$PWD/pcx_treemodel.h \
    $$PWD/pcx_auditwithtreemodel.h \
    $$PWD/pcx_reporting.h \
    $$PWD/pcx_reportingwithtreemodel.h \
    $$PWD/pcx_reportingtableoverviewmodel.h \
    $$PWD/pcx_reportingtablesupervisionmodel.h \
    $$PWD/pcx_prevision.h \
    $$PWD/pcx_previsionitemcriteria.h \
    $$PWD/pcx_previsionitemtablemodel.h \
    $$PWD/utils.h \
    $$PWD/pcx_previsionitem.h


#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

unix|win32: LIBS += -lgvc -lcgraph

win32: INCLUDEPATH += "c:/Program Files/Graphviz/include"

win32:CONFIG(release, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraph
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraphd

win32:INCLUDEPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"
win32:DEPENDPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"

