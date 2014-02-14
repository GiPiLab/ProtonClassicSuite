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
    QAction *actionNouvelle_base;
    QAction *actionOuvrir_base;
    QAction *actionG_rer;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QMdiArea *mdiArea;
    QMenuBar *menuBar;
    QMenu *menuArbres;
    QMenu *menuAudits;
    QMenu *menuAudits_2;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(701, 490);
        actionAfficher = new QAction(MainWindow);
        actionAfficher->setObjectName(QStringLiteral("actionAfficher"));
        actionNouvelle_base = new QAction(MainWindow);
        actionNouvelle_base->setObjectName(QStringLiteral("actionNouvelle_base"));
        actionOuvrir_base = new QAction(MainWindow);
        actionOuvrir_base->setObjectName(QStringLiteral("actionOuvrir_base"));
        actionG_rer = new QAction(MainWindow);
        actionG_rer->setObjectName(QStringLiteral("actionG_rer"));
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
        menuArbres = new QMenu(menuBar);
        menuArbres->setObjectName(QStringLiteral("menuArbres"));
        menuAudits = new QMenu(menuBar);
        menuAudits->setObjectName(QStringLiteral("menuAudits"));
        menuAudits_2 = new QMenu(menuBar);
        menuAudits_2->setObjectName(QStringLiteral("menuAudits_2"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuArbres->menuAction());
        menuBar->addAction(menuAudits->menuAction());
        menuBar->addAction(menuAudits_2->menuAction());
        menuArbres->addAction(actionNouvelle_base);
        menuArbres->addAction(actionOuvrir_base);
        menuAudits->addAction(actionAfficher);
        menuAudits->addAction(actionG_rer);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionAfficher->setText(QApplication::translate("MainWindow", "Afficher", 0));
        actionNouvelle_base->setText(QApplication::translate("MainWindow", "Nouvelle base", 0));
        actionOuvrir_base->setText(QApplication::translate("MainWindow", "Ouvrir base", 0));
        actionG_rer->setText(QApplication::translate("MainWindow", "G\303\251rer", 0));
        menuArbres->setTitle(QApplication::translate("MainWindow", "Bases", 0));
        menuAudits->setTitle(QApplication::translate("MainWindow", "Arbres", 0));
        menuAudits_2->setTitle(QApplication::translate("MainWindow", "Audits", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
