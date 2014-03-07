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
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogDisplayTree
{
public:
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_4;
    QComboBox *comboBox;
    QTreeView *treeView;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *newTreeButton;
    QPushButton *deleteTreeButton;
    QPushButton *remplirButton;
    QLabel *label;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout;
    QPushButton *addTypeButton;
    QPushButton *deleteTypeButton;
    QTableView *tableView;

    void setupUi(QWidget *DialogDisplayTree)
    {
        if (DialogDisplayTree->objectName().isEmpty())
            DialogDisplayTree->setObjectName(QStringLiteral("DialogDisplayTree"));
        DialogDisplayTree->setWindowModality(Qt::NonModal);
        DialogDisplayTree->resize(681, 426);
        verticalLayout_6 = new QVBoxLayout(DialogDisplayTree);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        comboBox = new QComboBox(DialogDisplayTree);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        verticalLayout_4->addWidget(comboBox);

        treeView = new QTreeView(DialogDisplayTree);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setRootIsDecorated(false);
        treeView->header()->setVisible(false);

        verticalLayout_4->addWidget(treeView);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        newTreeButton = new QPushButton(DialogDisplayTree);
        newTreeButton->setObjectName(QStringLiteral("newTreeButton"));

        horizontalLayout_5->addWidget(newTreeButton);

        deleteTreeButton = new QPushButton(DialogDisplayTree);
        deleteTreeButton->setObjectName(QStringLiteral("deleteTreeButton"));

        horizontalLayout_5->addWidget(deleteTreeButton);


        verticalLayout_4->addLayout(horizontalLayout_5);

        remplirButton = new QPushButton(DialogDisplayTree);
        remplirButton->setObjectName(QStringLiteral("remplirButton"));

        verticalLayout_4->addWidget(remplirButton);

        label = new QLabel(DialogDisplayTree);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_4->addWidget(label);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        addTypeButton = new QPushButton(DialogDisplayTree);
        addTypeButton->setObjectName(QStringLiteral("addTypeButton"));

        horizontalLayout->addWidget(addTypeButton);

        deleteTypeButton = new QPushButton(DialogDisplayTree);
        deleteTypeButton->setObjectName(QStringLiteral("deleteTypeButton"));

        horizontalLayout->addWidget(deleteTypeButton);


        verticalLayout_5->addLayout(horizontalLayout);

        tableView = new QTableView(DialogDisplayTree);
        tableView->setObjectName(QStringLiteral("tableView"));

        verticalLayout_5->addWidget(tableView);


        horizontalLayout_2->addLayout(verticalLayout_5);


        verticalLayout_6->addLayout(horizontalLayout_2);


        retranslateUi(DialogDisplayTree);

        QMetaObject::connectSlotsByName(DialogDisplayTree);
    } // setupUi

    void retranslateUi(QWidget *DialogDisplayTree)
    {
        DialogDisplayTree->setWindowTitle(QApplication::translate("DialogDisplayTree", "DialogDisplayTree", 0));
        newTreeButton->setText(QApplication::translate("DialogDisplayTree", "Nouvel arbre", 0));
        deleteTreeButton->setText(QApplication::translate("DialogDisplayTree", "Supprimer cet arbre", 0));
        remplirButton->setText(QApplication::translate("DialogDisplayTree", "Remplir un nouvel arbre bidon", 0));
        label->setText(QApplication::translate("DialogDisplayTree", "TextLabel", 0));
        addTypeButton->setText(QApplication::translate("DialogDisplayTree", "Ajouter un type", 0));
        deleteTypeButton->setText(QApplication::translate("DialogDisplayTree", "Supprimer un type", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogDisplayTree: public Ui_DialogDisplayTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGDISPLAYTREE_H
