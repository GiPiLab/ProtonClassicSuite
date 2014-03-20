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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogDisplayTree
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QListView *listTypesView;
    QTreeView *treeView;
    QPushButton *addTypeButton;
    QPushButton *newTreeButton;
    QPushButton *deleteTypeButton;
    QPushButton *addNodeButton;
    QPushButton *finishTreeButton;
    QPushButton *deleteTreeButton;
    QComboBox *comboBox;
    QPushButton *modifyNodeButton;
    QPushButton *deleteNodeButton;
    QPushButton *remplirButton;

    void setupUi(QWidget *DialogDisplayTree)
    {
        if (DialogDisplayTree->objectName().isEmpty())
            DialogDisplayTree->setObjectName(QStringLiteral("DialogDisplayTree"));
        DialogDisplayTree->setWindowModality(Qt::NonModal);
        DialogDisplayTree->resize(522, 324);
        verticalLayout = new QVBoxLayout(DialogDisplayTree);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        listTypesView = new QListView(DialogDisplayTree);
        listTypesView->setObjectName(QStringLiteral("listTypesView"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listTypesView->sizePolicy().hasHeightForWidth());
        listTypesView->setSizePolicy(sizePolicy);
        listTypesView->setAlternatingRowColors(true);

        gridLayout->addWidget(listTypesView, 1, 3, 1, 1);

        treeView = new QTreeView(DialogDisplayTree);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setRootIsDecorated(false);
        treeView->header()->setVisible(false);

        gridLayout->addWidget(treeView, 1, 0, 1, 3);

        addTypeButton = new QPushButton(DialogDisplayTree);
        addTypeButton->setObjectName(QStringLiteral("addTypeButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addTypeButton->sizePolicy().hasHeightForWidth());
        addTypeButton->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(addTypeButton, 2, 3, 1, 1);

        newTreeButton = new QPushButton(DialogDisplayTree);
        newTreeButton->setObjectName(QStringLiteral("newTreeButton"));

        gridLayout->addWidget(newTreeButton, 4, 0, 1, 1);

        deleteTypeButton = new QPushButton(DialogDisplayTree);
        deleteTypeButton->setObjectName(QStringLiteral("deleteTypeButton"));
        sizePolicy1.setHeightForWidth(deleteTypeButton->sizePolicy().hasHeightForWidth());
        deleteTypeButton->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(deleteTypeButton, 4, 3, 1, 1);

        addNodeButton = new QPushButton(DialogDisplayTree);
        addNodeButton->setObjectName(QStringLiteral("addNodeButton"));

        gridLayout->addWidget(addNodeButton, 2, 0, 1, 1);

        finishTreeButton = new QPushButton(DialogDisplayTree);
        finishTreeButton->setObjectName(QStringLiteral("finishTreeButton"));

        gridLayout->addWidget(finishTreeButton, 4, 2, 1, 1);

        deleteTreeButton = new QPushButton(DialogDisplayTree);
        deleteTreeButton->setObjectName(QStringLiteral("deleteTreeButton"));

        gridLayout->addWidget(deleteTreeButton, 4, 1, 1, 1);

        comboBox = new QComboBox(DialogDisplayTree);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        gridLayout->addWidget(comboBox, 0, 0, 1, 4);

        modifyNodeButton = new QPushButton(DialogDisplayTree);
        modifyNodeButton->setObjectName(QStringLiteral("modifyNodeButton"));

        gridLayout->addWidget(modifyNodeButton, 2, 1, 1, 1);

        deleteNodeButton = new QPushButton(DialogDisplayTree);
        deleteNodeButton->setObjectName(QStringLiteral("deleteNodeButton"));

        gridLayout->addWidget(deleteNodeButton, 2, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        remplirButton = new QPushButton(DialogDisplayTree);
        remplirButton->setObjectName(QStringLiteral("remplirButton"));
        sizePolicy1.setHeightForWidth(remplirButton->sizePolicy().hasHeightForWidth());
        remplirButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(remplirButton);


        retranslateUi(DialogDisplayTree);

        QMetaObject::connectSlotsByName(DialogDisplayTree);
    } // setupUi

    void retranslateUi(QWidget *DialogDisplayTree)
    {
        DialogDisplayTree->setWindowTitle(QApplication::translate("DialogDisplayTree", "DialogDisplayTree", 0));
        addTypeButton->setText(QApplication::translate("DialogDisplayTree", "Ajouter un type", 0));
        newTreeButton->setText(QApplication::translate("DialogDisplayTree", "Ajouter un arbre", 0));
        deleteTypeButton->setText(QApplication::translate("DialogDisplayTree", "Supprimer un type", 0));
        addNodeButton->setText(QApplication::translate("DialogDisplayTree", "Ajouter noeud", 0));
        finishTreeButton->setText(QApplication::translate("DialogDisplayTree", "Terminer un arbre", 0));
        deleteTreeButton->setText(QApplication::translate("DialogDisplayTree", "Supprimer un arbre", 0));
        modifyNodeButton->setText(QApplication::translate("DialogDisplayTree", "Modifier noeud", 0));
        deleteNodeButton->setText(QApplication::translate("DialogDisplayTree", "Supprimer noeud", 0));
        remplirButton->setText(QApplication::translate("DialogDisplayTree", "Al\303\251atoire", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogDisplayTree: public Ui_DialogDisplayTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGDISPLAYTREE_H
