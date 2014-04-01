/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionManageTree;
    QAction *actionReset;
    QAction *actionExit;
    QAction *actionDisplayTree;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QMdiArea *mdiArea;
    QMenuBar *menuBar;
    QMenu *menuBases;
    QMenu *menuArbres;
    QMenu *menuAudits;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(701, 490);
        actionManageTree = new QAction(MainWindow);
        actionManageTree->setObjectName(QStringLiteral("actionManageTree"));
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionDisplayTree = new QAction(MainWindow);
        actionDisplayTree->setObjectName(QStringLiteral("actionDisplayTree"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QStringLiteral("mdiArea"));

        horizontalLayout->addWidget(mdiArea);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 701, 19));
        menuBases = new QMenu(menuBar);
        menuBases->setObjectName(QStringLiteral("menuBases"));
        menuArbres = new QMenu(menuBar);
        menuArbres->setObjectName(QStringLiteral("menuArbres"));
        menuAudits = new QMenu(menuBar);
        menuAudits->setObjectName(QStringLiteral("menuAudits"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuBases->menuAction());
        menuBar->addAction(menuArbres->menuAction());
        menuBar->addAction(menuAudits->menuAction());
        menuBases->addAction(actionReset);
        menuBases->addAction(actionExit);
        menuArbres->addAction(actionManageTree);
        menuArbres->addAction(actionDisplayTree);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ProtonClassicSuite", 0));
        actionManageTree->setText(QApplication::translate("MainWindow", "&Gestion des arbres", 0));
        actionReset->setText(QApplication::translate("MainWindow", "&Effacer toutes les donn\303\251es", 0));
        actionExit->setText(QApplication::translate("MainWindow", "&Quitter", 0));
        actionDisplayTree->setText(QApplication::translate("MainWindow", "&Afficher un arbre", 0));
        menuBases->setTitle(QApplication::translate("MainWindow", "&Fichiers", 0));
        menuArbres->setTitle(QApplication::translate("MainWindow", "&Arbres", 0));
        menuAudits->setTitle(QApplication::translate("MainWindow", "A&udits", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
