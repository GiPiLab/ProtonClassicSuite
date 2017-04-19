#include "formeditaudit.h"
#include "ui_formeditaudit.h"
#include "pcx_tables.h"
#include "productactivation.h"
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
#include <QTextDocument>
#include <QSettings>
#include "formauditinfos.h"

FormEditAudit::FormEditAudit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEditAudit)
{
    ui->setupUi(this);
    selectedNode=0;
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


void FormEditAudit::updateListOfAudits()
{
    ui->comboListAudits->clear();

    //QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::UnFinishedAuditsOnly);
    QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::AllAudits);
    bool nonEmpty=!listOfAudits.isEmpty();


    if(listOfAudits.isEmpty())
    {
         QMessageBox::information(this,tr("Information"),tr("Créez d'abord un audit dans la fenêtre de gestion des audits"));
    }
    else
    {
        QMessageBox::information(this,tr("Information"),tr("Les données sont modifiables pour les audits non terminés. Les valeurs se saisissent uniquement aux <b>feuilles</b> de l'arbre et les sommes sont mises à jour dynamiquement vers la racine"));
    }


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

    ui->tableViewDF->horizontalHeader()->setSectionResizeMode((int)PCx_EditableAuditModel::COLINDEXES::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode((int)PCx_EditableAuditModel::COLINDEXES::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode((int)PCx_EditableAuditModel::COLINDEXES::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode((int)PCx_EditableAuditModel::COLINDEXES::COL_ANNEE,QHeaderView::Fixed);


    QModelIndex rootIndex=auditModel->getAttachedTree()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);          
}

void FormEditAudit::on_treeView_clicked(const QModelIndex &index)
{
    selectedNode=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    if(selectedNode==0)
    {
        qFatal("Assertion failed");
    }

    auditModel->getTableModelDF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelDI()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRI()->setFilter(QString("id_node=%1").arg(selectedNode));
    bool isLeaf=auditModel->getAttachedTree()->isLeaf(selectedNode);
    bool isFinished=auditModel->isFinished();

    ProductActivation productActivation;
    ProductActivation::AvailableModules modulesFlags=productActivation.getAvailablesModules();


    if(isLeaf)
    {
        if(!isFinished && !(modulesFlags & ProductActivation::AvailableModule::DEMO))
        {
            ui->tableViewDF->setEditTriggers(QAbstractItemView::AllEditTriggers);
            ui->tableViewRF->setEditTriggers(QAbstractItemView::AllEditTriggers);
            ui->tableViewDI->setEditTriggers(QAbstractItemView::AllEditTriggers);
            ui->tableViewRI->setEditTriggers(QAbstractItemView::AllEditTriggers);
        }
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


void FormEditAudit::onAuditDataUpdated(unsigned int auditId)
{
    if(auditModel==nullptr)
        return;
    if(auditModel->getAuditId()==auditId)
    {
        if(ui->treeView->currentIndex().isValid())
        {
            on_treeView_clicked(ui->treeView->currentIndex());
        }
        else
        {
            QModelIndex rootIndex=auditModel->getAttachedTree()->index(0,0);
            on_treeView_clicked(rootIndex);
        }
    }
}

void FormEditAudit::on_pushButtonExportHTML_clicked()
{
    QString out=auditModel->generateHTMLHeader();
    out.append(auditModel->generateHTMLAuditTitle());


    MODES::DFRFDIRI mode=MODES::DFRFDIRI::DF;


    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=MODES::DFRFDIRI::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=MODES::DFRFDIRI::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=MODES::DFRFDIRI::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=MODES::DFRFDIRI::RI;


    PCx_Tables tables(auditModel);

    out.append(tables.getPCARawData(selectedNode,mode));
    out.append("</body></html>");

    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        return;
    }

    QSettings settings;
    QString settingStyle=settings.value("output/style","CSS").toString();
    if(settingStyle=="INLINE")
    {
        QTextDocument doc;
        doc.setHtml(out);

        out=doc.toHtml("utf-8");
        out.replace(" -qt-block-indent:0;","");
    }


    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<out;
    stream.flush();
    file.close();
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName().toHtmlEscaped()));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));
}
