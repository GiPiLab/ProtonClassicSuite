#include "pcx_reporting.h"
#include "formmanagereportings.h"
#include "ui_formmanagereportings.h"
#include "formdisplaytree.h"
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QSettings>
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
    updateRandomVisibility();
}

FormManageReportings::~FormManageReportings()
{
    if(selectedReporting!=nullptr)
        delete selectedReporting;
    delete ui;
}

void FormManageReportings::onLOTchanged()
{
    updateListOfTrees();
}

void FormManageReportings::onListOfAuditsChanged()
{
    updateListOfPotentialAudits();
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
    QMdiSubWindow *subWin=mdiArea->addSubWindow(ddt);
    subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
    ddt->show();
}

void FormManageReportings::on_pushButtonAddReporting_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
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

    redo:
    do
    {
        text=QInputDialog::getText(this,tr("Nouveau reporting"), tr("Nom du reporting à ajouter : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        if(PCx_Reporting::reportingNameExists(text))
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà un reporting portant ce nom !"));
            goto redo;
        }
        if(text.size()>MAXOBJECTNAMELENGTH)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Nom trop long !"));
            goto redo;
        }
        if(PCx_Reporting::addNewReporting(text,selectedTree)>0)
        {
            QMessageBox::information(this,tr("Succès"),tr("Nouveau reporting ajouté. Utilisez les boutons ci-dessus pour y ajouter des données."));
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

void FormManageReportings::updateListOfPotentialAudits()
{
    ui->comboListOfAudits->clear();
    if(selectedReporting==nullptr)
        return;

    QList<QPair<unsigned int,QString> > listOfAuditsWithThisTree=PCx_Audit::getListOfAuditsAttachedWithThisTree(selectedReporting->getAttachedTreeId());
    ui->pushButtonFillAudit->setEnabled(!listOfAuditsWithThisTree.isEmpty());
    QPair<unsigned int,QString> p;
    foreach(p,listOfAuditsWithThisTree)
    {
        ui->comboListOfAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListOfAudits->setCurrentIndex(0);
}

void FormManageReportings::updateRandomVisibility()
{
    QSettings settings;
    bool randomAllowed=settings.value("misc/randomAllowed",false).toBool();
    ui->pushButtonRandomDF->setEnabled(randomAllowed);
    ui->pushButtonRandomRF->setEnabled(randomAllowed);
    ui->pushButtonRandomDI->setEnabled(randomAllowed);
    ui->pushButtonRandomRI->setEnabled(randomAllowed);
}

void FormManageReportings::on_pushButtonDeleteReporting_clicked()
{
    if(ui->comboListOfReportings->currentIndex()==-1)
    {
        return;
    }
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> le reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()))==QMessageBox::No)
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

    updateListOfPotentialAudits();

    updateReportingInfos();

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

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DFRFDIRI::DF);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toHtmlEscaped()));
            updateReportingInfos();
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

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DFRFDIRI::RF);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toHtmlEscaped()));
            updateReportingInfos();
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

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DFRFDIRI::DI);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toHtmlEscaped()));
            updateReportingInfos();
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

        bool res=selectedReporting->importDataFromXLSX(fileName,MODES::DFRFDIRI::RI);

        if(res==true)
        {
            QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toHtmlEscaped()));
            updateReportingInfos();
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
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toHtmlEscaped()));
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
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toHtmlEscaped()));
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
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toHtmlEscaped()));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}

void FormManageReportings::updateReportingInfos()
{
    if(selectedReporting!=nullptr)
    {
        ui->labelTreeName->setText(QString("%1 (%2 noeuds)").arg(selectedReporting->getAttachedTreeName()).arg(selectedReporting->getAttachedTree()->getNumberOfNodes()));
        ui->labelLastDF->setText(selectedReporting->getLastReportingDate(MODES::DFRFDIRI::DF).toString(Qt::DefaultLocaleShortDate));
        ui->labelLastRF->setText(selectedReporting->getLastReportingDate(MODES::DFRFDIRI::RF).toString(Qt::DefaultLocaleShortDate));
        ui->labelLastDI->setText(selectedReporting->getLastReportingDate(MODES::DFRFDIRI::DI).toString(Qt::DefaultLocaleShortDate));
        ui->labelLastRI->setText(selectedReporting->getLastReportingDate(MODES::DFRFDIRI::RI).toString(Qt::DefaultLocaleShortDate));
        emit(reportingDataUpdated(selectedReporting->getReportingId()));
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
            QMessageBox::information(this,tr("Succès"),tr("%1 enregistrées !").arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toHtmlEscaped()));
        }
        else
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
        }
    }
}

void FormManageReportings::on_pushButtonClearDF_clicked()
{
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> toutes les données de %2 du reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->clearAllData(MODES::DFRFDIRI::DF);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonClearRF_clicked()
{
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> toutes les données de %2 du reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->clearAllData(MODES::DFRFDIRI::RF);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonClearDI_clicked()
{
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> toutes les données de %2 du reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->clearAllData(MODES::DFRFDIRI::DI);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonClearRI_clicked()
{
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> toutes les données de %2 du reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->clearAllData(MODES::DFRFDIRI::RI);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonRandomDF_clicked()
{
    if(question(tr("Ajouter des données aléatoires pour les %2 15 jours après la dernière situation du reporting <b>%1</b> ?").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->addRandomDataForNext15(MODES::DFRFDIRI::DF);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonRandomRF_clicked()
{
    if(question(tr("Ajouter des données aléatoires pour les %2 15 jours après la dernière situation du reporting <b>%1</b> ?").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->addRandomDataForNext15(MODES::DFRFDIRI::RF);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonRandomDI_clicked()
{
    if(question(tr("Ajouter des données aléatoires pour les %2 15 jours après la dernière situation du reporting <b>%1</b> ?").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->addRandomDataForNext15(MODES::DFRFDIRI::DI);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonRandomRI_clicked()
{
    if(question(tr("Ajouter des données aléatoires pour les %2 15 jours après la dernière situation du reporting <b>%1</b> ?").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toHtmlEscaped()))==QMessageBox::Yes)
    {
        selectedReporting->addRandomDataForNext15(MODES::DFRFDIRI::RI);
        updateReportingInfos();
    }
}

void FormManageReportings::on_pushButtonFillAudit_clicked()
{
    if(question(tr("Ajouter les données de la dernière situation du reporting <b>%1</b> à l'audit <b>%2</b> ?").arg(ui->comboListOfReportings->currentText().toHtmlEscaped()).arg(ui->comboListOfAudits->currentText().toHtmlEscaped()))==QMessageBox::Yes)
    {
        unsigned int selectedAuditId=ui->comboListOfAudits->currentData().toUInt();

        PCx_Audit audit(selectedAuditId);
        if(selectedReporting->addLastReportingDateToExistingAudit(&audit)==true)
        {
            emit(auditDataUpdated(selectedAuditId));
            QMessageBox::information(nullptr,tr("Succès !"),tr("Données ajoutées à l'audit !"));
        }
    }
}
