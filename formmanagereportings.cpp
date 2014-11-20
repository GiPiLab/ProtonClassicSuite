#include "pcx_reporting.h"
#include "formmanagereportings.h"
#include "ui_formmanagereportings.h"
#include "formdisplaytree.h"
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QStandardPaths>


FormManageReportings::FormManageReportings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormManageReportings)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    updateListOfTrees();
    updateListOfReportings();
}

FormManageReportings::~FormManageReportings()
{
    qDebug()<<"Destructor";
    if(selectedReporting!=nullptr)
        delete selectedReporting;
    delete ui;
}

void FormManageReportings::onLOTchanged()
{
    updateListOfTrees();
}

void FormManageReportings::updateListOfTrees()
{
    ui->comboListOfTrees->clear();

    QList<QPair<unsigned int,QString> > lot=PCx_Tree::getListOfTrees(true);
    setEnabled(!lot.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,lot)
    {
        ui->comboListOfTrees->insertItem(0,p.second,p.first);
    }
    ui->comboListOfTrees->setCurrentIndex(0);
}

void FormManageReportings::on_pushButtonDisplayTree_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
        return;
    }
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé le reporting"));
        return;
    }

    unsigned int selectedTree=ui->comboListOfTrees->currentData().toUInt();

    FormDisplayTree *ddt=new FormDisplayTree(selectedTree,this);
    ddt->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *mdiSubWin=(QMdiSubWindow *)this->parentWidget();
    QMdiArea *mdiArea=mdiSubWin->mdiArea();
    mdiArea->addSubWindow(ddt);
    ddt->show();
}

void FormManageReportings::on_pushButtonAddReporting_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
        return;
    }
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé le reporting"));
        return;
    }

    unsigned int selectedTree=ui->comboListOfTrees->currentData().toUInt();

    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouveau reporting"), tr("Nom du reporting à ajouter : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        if(PCx_Reporting::addNewReporting(text,selectedTree)>0)
        {
            updateListOfReportings();
            emit(listOfReportingsChanged());
        }
    }
}

void FormManageReportings::updateListOfReportings()
{
    ui->comboListOfReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    ui->groupBoxReportings->setEnabled(!listOfReportings.isEmpty());
    QPair<unsigned int,QString> p;
    foreach(p,listOfReportings)
    {
        ui->comboListOfReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListOfReportings->setCurrentIndex(0);
    this->on_comboListOfReportings_activated(0);
}


void FormManageReportings::on_pushButtonDeleteReporting_clicked()
{
    if(ui->comboListOfReportings->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>supprimer</b> le reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText()))==QMessageBox::No)
    {
        return;
    }
    if(PCx_Reporting::deleteReporting(ui->comboListOfReportings->currentData().toUInt())==false)
        die();

    updateListOfReportings();
    emit(listOfReportingsChanged());

}



void FormManageReportings::on_comboListOfReportings_activated(int index)
{
    if(index==-1 || ui->comboListOfReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListOfReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }

    selectedReporting=new PCx_Reporting(selectedReportingId);

    ui->labelTreeName->setText(QString("%1 (%2 noeuds)").arg(selectedReporting->getAttachedTreeName()).arg(selectedReporting->getAttachedTree()->getNumberOfNodes()));

}

void FormManageReportings::on_pushButtonExportLeaves_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le squelette de reporting"),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
            fileName.append(".xlsx");
        fi=QFileInfo(fileName);

        bool res=selectedReporting->exportLeavesSkeleton(fileName);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("Squelette de reporting enregistré !"));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }

    }


}

void FormManageReportings::on_pushButtonLoadDF_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DF);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DF)));
        }
    }
}



void FormManageReportings::on_pushButtonLoadRF_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::RF);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::RF)));
        }
    }
}

void FormManageReportings::on_pushButtonLoadDI_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DI);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DI)));
        }
    }
}

void FormManageReportings::on_pushButtonLoadRI_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::RI);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::RI)));
        }
    }
}

void FormManageReportings::on_pushButtonExportDF_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer les %1").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF)),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
            fileName.append(".xlsx");
        fi=QFileInfo(fileName);

        bool res=selectedReporting->exportLeavesDataXLSX(MODES::DFRFDIRI::DF,fileName);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF)));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}

void FormManageReportings::on_pushButtonExportRF_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer les %1").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF)),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
            fileName.append(".xlsx");
        fi=QFileInfo(fileName);

        bool res=selectedReporting->exportLeavesDataXLSX(MODES::DFRFDIRI::RF,fileName);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF)));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}

void FormManageReportings::on_pushButtonExportDI_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer les %1").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI)),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
            fileName.append(".xlsx");
        fi=QFileInfo(fileName);

        bool res=selectedReporting->exportLeavesDataXLSX(MODES::DFRFDIRI::DI,fileName);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI)));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}

void FormManageReportings::on_pushButtonExportRI_clicked()
{
    if(selectedReporting!=nullptr)
    {
        QFileDialog fileDialog;
        fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer les %1").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI)),"",tr("Fichiers XLSX (*.xlsx)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
            fileName.append(".xlsx");
        fi=QFileInfo(fileName);

        bool res=selectedReporting->exportLeavesDataXLSX(MODES::DFRFDIRI::RI,fileName);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI)));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}
