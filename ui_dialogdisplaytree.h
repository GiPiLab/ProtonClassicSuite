/********************************************************************************
** Form generated from reading UI file 'dialogdisplaytree.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGDISPLAYTREE_H
#define UI_DIALOGDISPLAYTREE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_DialogDisplayTree
{
public:
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QTreeView *treeView;
    QLabel *label;
    QPushButton *printViewButton;

    void setupUi(QDialog *DialogDisplayTree)
    {
        if (DialogDisplayTree->objectName().isEmpty())
            DialogDisplayTree->setObjectName(QStringLiteral("DialogDisplayTree"));
        DialogDisplayTree->resize(324, 283);
        horizontalLayout = new QHBoxLayout(DialogDisplayTree);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        treeView = new QTreeView(DialogDisplayTree);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setSelectionMode(QAbstractItemView::NoSelection);
        treeView->setHeaderHidden(true);

        gridLayout->addWidget(treeView, 1, 0, 1, 2);

        label = new QLabel(DialogDisplayTree);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        printViewButton = new QPushButton(DialogDisplayTree);
        printViewButton->setObjectName(QStringLiteral("printViewButton"));

        gridLayout->addWidget(printViewButton, 2, 0, 1, 2);


        horizontalLayout->addLayout(gridLayout);


        retranslateUi(DialogDisplayTree);

        QMetaObject::connectSlotsByName(DialogDisplayTree);
    } // setupUi

    void retranslateUi(QDialog *DialogDisplayTree)
    {
        DialogDisplayTree->setWindowTitle(QApplication::translate("DialogDisplayTree", "Instantan\303\251 d'arbre", 0));
        label->setText(QApplication::translate("DialogDisplayTree", "TextLabel", 0));
        printViewButton->setText(QApplication::translate("DialogDisplayTree", "Imprimer la vue", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogDisplayTree: public Ui_DialogDisplayTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGDISPLAYTREE_H
