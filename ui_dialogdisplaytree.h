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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogDisplayTree
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *chargerButton;
    QComboBox *comboBox;
    QTreeView *treeView;
    QPushButton *remplirButton;
    QLabel *label;

    void setupUi(QWidget *DialogDisplayTree)
    {
        if (DialogDisplayTree->objectName().isEmpty())
            DialogDisplayTree->setObjectName(QStringLiteral("DialogDisplayTree"));
        DialogDisplayTree->setWindowModality(Qt::NonModal);
        DialogDisplayTree->resize(473, 415);
        horizontalLayout = new QHBoxLayout(DialogDisplayTree);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        chargerButton = new QPushButton(DialogDisplayTree);
        chargerButton->setObjectName(QStringLiteral("chargerButton"));

        verticalLayout->addWidget(chargerButton);

        comboBox = new QComboBox(DialogDisplayTree);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        verticalLayout->addWidget(comboBox);

        treeView = new QTreeView(DialogDisplayTree);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setRootIsDecorated(false);
        treeView->header()->setVisible(false);

        verticalLayout->addWidget(treeView);

        remplirButton = new QPushButton(DialogDisplayTree);
        remplirButton->setObjectName(QStringLiteral("remplirButton"));

        verticalLayout->addWidget(remplirButton);

        label = new QLabel(DialogDisplayTree);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(DialogDisplayTree);

        QMetaObject::connectSlotsByName(DialogDisplayTree);
    } // setupUi

    void retranslateUi(QWidget *DialogDisplayTree)
    {
        DialogDisplayTree->setWindowTitle(QApplication::translate("DialogDisplayTree", "DialogDisplayTree", 0));
        chargerButton->setText(QApplication::translate("DialogDisplayTree", "Charger la liste", 0));
        remplirButton->setText(QApplication::translate("DialogDisplayTree", "Remplir un nouvel arbre bidon", 0));
        label->setText(QApplication::translate("DialogDisplayTree", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogDisplayTree: public Ui_DialogDisplayTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGDISPLAYTREE_H
