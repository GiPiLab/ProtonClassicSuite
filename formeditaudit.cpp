#include "formeditaudit.h"
#include "ui_formeditaudit.h"
#include <QDebug>
#include "utils.h"
#include <QMessageBox>
#include <QtGlobal>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QElapsedTimer>
#include <QProgressDialog>
#include <QSettings>
#include "formauditinfos.h"

FormEditAudit::FormEditAudit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEditAudit)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    ui->tableViewDF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auditModel=nullptr;

    delegateDF=new auditDataDelegate(ui->tableViewDF);
    delegateRF=new auditDataDelegate(ui->tableViewRF);
    delegateDI=new auditDataDelegate(ui->tableViewDI);
    delegateRI=new auditDataDelegate(ui->tableViewRI);

    ui->tableViewDF->setItemDelegate(delegateDF);
    ui->tableViewRF->setItemDelegate(delegateRF);
    ui->tableViewDI->setItemDelegate(delegateDI);
    ui->tableViewRI->setItemDelegate(delegateRI);

    updateListOfAudits();
    updateRandomButtonVisibility();
}

FormEditAudit::~FormEditAudit()
{
    delete ui;
    if(auditModel!=nullptr)
    {
        delete auditModel;
    }
    delete(delegateDF);
    delete(delegateRF);
    delete(delegateDI);
    delete(delegateRI);
}

void FormEditAudit::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormEditAudit::updateRandomButtonVisibility()
{
    QSettings settings;
    bool randomAllowed=settings.value("misc/randomAllowed",false).toBool();
    ui->randomDataButton->setEnabled(randomAllowed);
}

void FormEditAudit::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::UnFinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    ui->splitter->setEnabled(nonEmpty);

    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

QSize FormEditAudit::sizeHint() const
{
    return QSize(900,400);
}

void FormEditAudit::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;
    if(auditModel!=nullptr)
    {
        delete auditModel;
    }

    QItemSelectionModel *m=ui->treeView->selectionModel();
    //Read-write audit model
    auditModel=new PCx_EditableAuditModel(selectedAuditId,0);
    ui->treeView->setModel(auditModel->getAttachedTree());
    delete m;
    ui->treeView->expandToDepth(1);

    ui->tableViewDF->setModel(auditModel->getTableModelDF());
    ui->tableViewDF->hideColumn(0);
    ui->tableViewDF->hideColumn(1);
    ui->tableViewRF->setModel(auditModel->getTableModelRF());
    ui->tableViewRF->hideColumn(0);
    ui->tableViewRF->hideColumn(1);
    ui->tableViewDI->setModel(auditModel->getTableModelDI());
    ui->tableViewDI->hideColumn(0);
    ui->tableViewDI->hideColumn(1);
    ui->tableViewRI->setModel(auditModel->getTableModelRI());
    ui->tableViewRI->hideColumn(0);
    ui->tableViewRI->hideColumn(1);

    //Roots
    auditModel->getTableModelDF()->setFilter(QString("id_node=1"));
    auditModel->getTableModelRF()->setFilter(QString("id_node=1"));
    auditModel->getTableModelDI()->setFilter(QString("id_node=1"));
    auditModel->getTableModelRI()->setFilter(QString("id_node=1"));
    ui->label->setText(auditModel->getAttachedTree()->index(0,0).data().toString());

    ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableViewDF->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);


    QModelIndex rootIndex=auditModel->getAttachedTree()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
}

void FormEditAudit::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    Q_ASSERT(selectedNode>0);

    auditModel->getTableModelDF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelDI()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRI()->setFilter(QString("id_node=%1").arg(selectedNode));
    bool isLeaf=auditModel->getAttachedTree()->isLeaf(selectedNode);

    if(isLeaf)
    {
        ui->tableViewDF->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewRF->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewDI->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewRI->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    else
    {
        ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    ui->label->setText(index.data().toString());
}

void FormEditAudit::on_randomDataButton_clicked()
{
    MODES::DFRFDIRI mode=MODES::DFRFDIRI::DF;
    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=MODES::DFRFDIRI::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=MODES::DFRFDIRI::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=MODES::DFRFDIRI::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=MODES::DFRFDIRI::RI;


    if(question(tr("Remplir les <b>%1</b> de l'audit de données aléatoires ?").arg(MODES::modeToCompleteString(mode)))==QMessageBox::No)
    {
        return;
    }

    //QElapsedTimer timer;
    //timer.start();

    auditModel->fillWithRandomData(mode);
    QSqlTableModel *tblModel=auditModel->getTableModel(mode);
    if(tblModel!=nullptr)
        tblModel->select();

    //qDebug()<<"Done in "<<timer.elapsed()<<"ms";
}

void FormEditAudit::on_clearDataButton_clicked()
{

    MODES::DFRFDIRI mode=MODES::DFRFDIRI::DF;

    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=MODES::DFRFDIRI::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=MODES::DFRFDIRI::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=MODES::DFRFDIRI::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=MODES::DFRFDIRI::RI;

    if(question(tr("Effacer toutes les <b>%1</b> ?").arg(MODES::modeToCompleteString(mode)))==QMessageBox::No)
    {
        return;
    }

    auditModel->clearAllData(mode);
}

void FormEditAudit::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
    ui->treeView->expandToDepth(0);
}

void FormEditAudit::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormEditAudit::on_statsButton_clicked()
{
    FormAuditInfos *infos=new FormAuditInfos(auditModel->getAuditId(),this);
    infos->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *mdiSubWin=(QMdiSubWindow *)this->parentWidget();
    QMdiArea *mdiArea=mdiSubWin->mdiArea();
    mdiArea->addSubWindow(infos);
    infos->show();
}

void FormEditAudit::on_pushButtonExportLeaves_clicked()
{
    MODES::DFRFDIRI mode=MODES::DFRFDIRI::DF;


    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=MODES::DFRFDIRI::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=MODES::DFRFDIRI::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=MODES::DFRFDIRI::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=MODES::DFRFDIRI::RI;


    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
    if(fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
        fileName.append(".xlsx");
    fi=QFileInfo(fileName);

    bool res=auditModel->exportLeavesDataXLSX(mode,fileName);

    if(res==true)
    {
        QMessageBox::information(this,tr("Succès"),tr("<b>%1</b> enregistrées.").arg(MODES::modeToCompleteString(mode)));
    }
    else
    {
        QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement échoué"));
    }

}

void FormEditAudit::on_pushButtonImportLeaves_clicked()
{
    MODES::DFRFDIRI mode=MODES::DFRFDIRI::DF;


    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=MODES::DFRFDIRI::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=MODES::DFRFDIRI::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=MODES::DFRFDIRI::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=MODES::DFRFDIRI::RI;


    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"),"",tr("Fichiers XLSX (*.xlsx)"));
    if(fileName.isEmpty())
        return;

    bool res=auditModel->importDataFromXLSX(fileName,mode);
    QSqlTableModel *tblModel=auditModel->getTableModel(mode);
    if(tblModel!=nullptr)
        tblModel->select();

    if(res==true)
    {
        QMessageBox::information(this,tr("Succès"),tr("%1 chargées !").arg(MODES::modeToCompleteString(mode)));
    }
}
