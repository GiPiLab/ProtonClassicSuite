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
    QAction *actionAfficher;
    QAction *actionEffacerTout;
    QAction *actionOuvrir_base;
    QAction *actionG_rer;
    QAction *actionQuitter;
    QAction *actionAfficher_2;
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
        actionAfficher = new QAction(MainWindow);
        actionAfficher->setObjectName(QStringLiteral("actionAfficher"));
        actionEffacerTout = new QAction(MainWindow);
        actionEffacerTout->setObjectName(QStringLiteral("actionEffacerTout"));
        actionOuvrir_base = new QAction(MainWindow);
        actionOuvrir_base->setObjectName(QStringLiteral("actionOuvrir_base"));
        actionG_rer = new QAction(MainWindow);
        actionG_rer->setObjectName(QStringLiteral("actionG_rer"));
        actionQuitter = new QAction(MainWindow);
        actionQuitter->setObjectName(QStringLiteral("actionQuitter"));
        actionAfficher_2 = new QAction(MainWindow);
        actionAfficher_2->setObjectName(QStringLiteral("actionAfficher_2"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QStringLiteral("mdiArea"));
        mdiArea->setViewMode(QMdiArea::SubWindowView);
        mdiArea->setTabsClosable(false);

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
        menuBases->addAction(actionEffacerTout);
        menuBases->addAction(actionQuitter);
        menuArbres->addAction(actionAfficher);
        menuAudits->addAction(actionAfficher_2);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionAfficher->setText(QApplication::translate("MainWindow", "Afficher et modifier", 0));
        actionEffacerTout->setText(QApplication::translate("MainWindow", "&Effacer toutes les donn\303\251es", 0));
        actionOuvrir_base->setText(QApplication::translate("MainWindow", "Ouvrir base", 0));
        actionG_rer->setText(QApplication::translate("MainWindow", "G\303\251rer", 0));
        actionQuitter->setText(QApplication::translate("MainWindow", "&Quitter", 0));
        actionAfficher_2->setText(QApplication::translate("MainWindow", "Afficher", 0));
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
