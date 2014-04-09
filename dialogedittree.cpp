#include "dialogdisplaytree.h"
#include "dialogedittree.h"
#include "ui_dialogedittree.h"
#include "utility.h"
#include "pcx_treemodel.h"
#include <QtSql>
#include <QtGui>
#include <QMessageBox>
#include <QInputDialog>

DialogEditTree::DialogEditTree(QWidget *parent,QMdiArea *mdiArea) : QDialog(parent), ui(new Ui::DialogEditTree)
{
    ui->setupUi(this);
    model=NULL;
    this->mdiArea=mdiArea;
    updateListOfTree();
}

DialogEditTree::~DialogEditTree()
{
    if(model!=NULL)delete model;
    delete ui;
}


void DialogEditTree::on_addTypeButton_clicked()
{
    if(model!=NULL)
    {
        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Nouveau type"), tr("Nom du type à ajouter : "),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->getTypes()->addType(text);
        }
    }
}

void DialogEditTree::updateListOfTree()
{
    ui->comboBox->clear();

    QHash<int,QString> lot=PCx_TreeModel::getListOfTrees();
    foreach(int treeId,lot.keys())
    {
        ui->comboBox->insertItem(0,lot[treeId],treeId);
    }

    if(ui->comboBox->count()==0)
    {
        on_newTreeButton_clicked();
    }
    else
    {
        ui->comboBox->setCurrentIndex(0);
        on_comboBox_activated(0);
    }
}

void DialogEditTree::setReadOnly(bool state)
{
    ui->deleteNodeButton->setDisabled(state);
    ui->addNodeButton->setDisabled(state);
    ui->addTypeButton->setDisabled(state);
    ui->deleteTypeButton->setDisabled(state);
    ui->finishTreeButton->setDisabled(state);
    ui->modifyNodeButton->setDisabled(state);
    ui->treeView->setDisabled(state);
    ui->listTypesView->setDisabled(state);
}

void DialogEditTree::on_deleteTreeButton_clicked()
{
    if(model!=NULL)
    {
        if(QMessageBox::question(this,tr("Attention"),tr("Vous allez supprimer cet arbre. Cette action ne peut pas être annulée. En êtes-vous sûr ?"))==QMessageBox::No)
        {
            return;
        }

        unsigned int treeId=model->getTreeId();
        qDebug()<<"Deleting tree "<<treeId;

        int result=PCx_TreeModel::deleteTree(treeId);

        if(result==1)
        {
            ui->listTypesView->setModel(NULL);
            model->getTypes()->getTableModel()->clear();
            model->clear();

            updateListOfTree();
            delete model;
            model=NULL;

            emit(listOfTreeChanged());
            ui->comboBox->setCurrentIndex(-1);
        }
        else if(result==0)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe des audits liés à cet arbre. Supprimez-les d'abord"));
            return;
        }
    }
}

void DialogEditTree::onTypesChanged()
{
    qDebug()<<"Types changed !";
    model->updateTree();
    ui->treeView->expandToDepth(1);
}

void DialogEditTree::on_newTreeButton_clicked()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this->parentWidget(),tr("Nouvel arbre"), tr("Nom du nouvel arbre : "),QLineEdit::Normal,"",&ok);

    }while(ok && text.isEmpty());

    if(ok)
    {
        PCx_TreeModel::addNewTree(text);
        updateListOfTree();
        emit(listOfTreeChanged());
    }
}

void DialogEditTree::on_deleteTypeButton_clicked()
{
    if(model!=NULL)
    {
        QModelIndex index=ui->listTypesView->currentIndex();
        qDebug()<<index;
        if(index.row()>-1)
        {
            model->getTypes()->deleteType(model->getTypes()->getTableModel()->record(index.row()).field("id").value().toInt());
        }
    }
}

/*
 * Pense-bete pour accéder aux items et aux id
void DialogEditTree::on_treeView_clicked(const QModelIndex &index)
{
    qDebug()<<"Clicked : "<<index.data().toString()<<"Id = "<<index.data(Qt::UserRole+1).toInt()<< "Type = "<<index.data(Qt::UserRole+2).toInt();

    qDebug()<<"Selection model : "<<ui->treeView->selectionModel()->selectedIndexes()[0].data(Qt::UserRole+1).toInt();

    //Access id in listTypeView :
    QModelIndex indexType=ui->listTypesView->currentIndex();
    qDebug()<<"Type selected ID : "<<model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();
}*/

void DialogEditTree::on_addNodeButton_clicked()
{
    if(model==NULL)return;
    QModelIndexList selection=ui->treeView->selectionModel()->selectedIndexes();

    if(!selection.isEmpty())
    {
        int selectedId=selection[0].data(Qt::UserRole+1).toInt();
        qDebug()<<"Node Selected Id : "<<selectedId;

        QModelIndex indexType=ui->listTypesView->currentIndex();
        if(indexType.row()<0)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le type de l'élément à ajouter dans la zone de droite"));
            return;
        }

        int selectedTypeId=model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();

        qDebug()<<"Type selected ID : "<<selectedTypeId;

        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Nouveau noeud"), tr("Donnez un nom au nouveau noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->addNode(selectedId,selectedTypeId,text,selection[0]);
        }

    }
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez d'abord le noeud père dans l'arbre"));
        return;
    }
}

void DialogEditTree::on_modifyNodeButton_clicked()
{
    if(model==NULL)return;

    QModelIndexList selection=ui->treeView->selectionModel()->selectedIndexes();

    if(!selection.isEmpty())
    {
        int selectedId=selection[0].data(Qt::UserRole+1).toInt();
        qDebug()<<"Node Selected Id : "<<selectedId;

        //No modification for the root
        if(selectedId==1)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Vous ne pouvez pas modifier la racine de l'arbre"));
            return;
        }

        QModelIndex indexType=ui->listTypesView->currentIndex();
        if(indexType.row()<0)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le nouveau type du noeud à modifier dans la zone de droite"));
            return;
        }

        int selectedTypeId=model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();

        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Modifier noeud"), tr("Nouveau nom du noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->updateNode(selection[0],text,selectedTypeId);
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le noeud à modifier"));
        return;
    }
}

void DialogEditTree::on_treeView_doubleClicked()
{
    on_modifyNodeButton_clicked();
}

void DialogEditTree::on_deleteNodeButton_clicked()
{
    if(model==NULL)return;


    QModelIndexList selection=ui->treeView->selectionModel()->selectedIndexes();

    if(!selection.isEmpty())
    {
        //Assume only single selection
        int selectedId=selection[0].data(Qt::UserRole+1).toInt();
        qDebug()<<"Node Selected Id : "<<selectedId;

        //No modification for the root
        if(selectedId==1)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Vous ne pouvez pas supprimer la racine de l'arbre"));
            return;
        }

        if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment supprimer le noeud <b>%1</b> ainsi que tous ses descendants ?").arg(selection[0].data().toString()))==QMessageBox::No)
        {
            return;
        }
        model->deleteNode(selection[0]);
    }
}

void DialogEditTree::on_finishTreeButton_clicked()
{
    if(model!=NULL)
    {
        if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment terminer cet arbre ? Une fois terminé, vous ne pourrez plus le modifier."))==QMessageBox::No)
        {
            return;
        }
        model->finishTree();
        emit(listOfTreeChanged());
        updateListOfTree();
    }
}

void DialogEditTree::on_viewTreeButton_clicked()
{
    DialogDisplayTree *ddt=new DialogDisplayTree(model,this);
    mdiArea->addSubWindow(ddt);
    //displayTrees.append(ddt);
    ddt->show();
}

void DialogEditTree::on_comboBox_activated(int index)
{
    if(index==-1)return;
    if(model!=NULL)
    {
        ui->listTypesView->setModel(NULL);
        delete model;
    }

    model=new PCx_TreeModel(ui->comboBox->currentData().toInt());

    ui->treeView->setModel(model);
    ui->treeView->expandToDepth(1);

    ui->listTypesView->setModel(model->getTypes()->getTableModel());
    ui->listTypesView->setModelColumn(1);

    connect(model->getTypes(),SIGNAL(typesUpdated()),this,SLOT(onTypesChanged()));
    setReadOnly(model->isFinished());
}
