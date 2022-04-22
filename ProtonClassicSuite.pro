# ProtonClassicSuite
#
# Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
#
# labo@gipilab.org
#
# Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
# de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
#
#
# Ce logiciel est régi par la licence CeCILL soumise au droit français et
# respectant les principes de diffusion des logiciels libres. Vous pouvez
# utiliser, modifier et/ou redistribuer ce programme sous les conditions
# de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA
# sur le site "http://www.cecill.info".
#
# En contrepartie de l'accessibilité au code source et des droits de copie,
# de modification et de redistribution accordés par cette licence, il n'est
# offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
# seule une responsabilité restreinte pèse sur l'auteur du programme, le
# titulaire des droits patrimoniaux et les concédants successifs.
#
# A cet égard l'attention de l'utilisateur est attirée sur les risques
# associés au chargement, à l'utilisation, à la modification et/ou au
# développement et à la reproduction du logiciel par l'utilisateur étant
# donné sa spécificité de logiciel libre, qui peut le rendre complexe à
# manipuler et qui le réserve donc à des développeurs et des professionnels
# avertis possédant des connaissances informatiques approfondies. Les
# utilisateurs sont donc invités à charger et tester l'adéquation du
# logiciel à leurs besoins dans des conditions permettant d'assurer la
# sécurité de leurs systèmes et ou de leurs données et, plus généralement,
# à l'utiliser et l'exploiter dans les mêmes conditions de sécurité.
#
# Le fait que vous puissiez accéder à cet en-tête signifie que vous avez
# pris connaissance de la licence CeCILL, et que vous en avez accepté les
# termes.
#

QT       += core gui sql printsupport charts

CONFIG += c++17


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProtonClassicSuite
TEMPLATE=app

include(QXlsx/QXlsx.pri)

SOURCES += main.cpp\
	mainwindow.cpp \
    auditdatadelegate.cpp \
    utils.cpp \
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
    formauditprevisions.cpp \
    formmanageprevisions.cpp \
    pcx_previsionitemcriteria.cpp \
    pcx_previsionitemtablemodel.cpp \
    pcx_previsionitem.cpp \
    formdisplayprevisionreport.cpp \
    treemap.cpp \
    treemapwidget.cpp \
    formaudittreemap.cpp \
    zoomableqchartview.cpp

HEADERS  += mainwindow.h \
    auditdatadelegate.h \
    utils.h \
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
    formauditprevisions.h \
    formmanageprevisions.h \
    pcx_previsionitemcriteria.h \
    pcx_previsionitemtablemodel.h \
    pcx_previsionitem.h \
    formdisplayprevisionreport.h \
    treemap.h \
    treemapwidget.h \
    formaudittreemap.h \
    zoomableqchartview.h

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
    formmanageprevisions.ui \
    formdisplayprevisionreport.ui \
    formaudittreemap.ui

RC_ICONS = icons/logo64_64.ico

#Profile
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

win32: LIBS += -lgvc -lcgraph

unix:!android{
    LIBS += -lgvc -lcgraph
}


win32: INCLUDEPATH += "c:/Program Files/Graphviz/include"

win32:CONFIG(release, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraph
else:win32:CONFIG(debug, debug|release): LIBS += -L"$$PWD/../../../../Program Files/Graphviz/lib/release/lib/" -lcgraphd

win32:INCLUDEPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"
win32:DEPENDPATH += "$$PWD/../../../../Program Files/Graphviz/lib/release"

RESOURCES += \
    Images.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


