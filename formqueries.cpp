#include "formqueries.h"
#include "ui_formqueries.h"
#include <QDebug>


FormQueries::FormQueries(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormQueries)
{
    ui->setupUi(this);
    model=NULL;
    ui->comboBoxORED->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::ouverts)+"s",PCx_AuditModel::ouverts);
    ui->comboBoxORED->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::realises)+"s",PCx_AuditModel::realises);
    ui->comboBoxORED->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::engages)+"s",PCx_AuditModel::engages);
    ui->comboBoxORED->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::disponibles)+"s",PCx_AuditModel::disponibles);

    ui->comboBoxDFRFDIRI->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DF),PCx_AuditModel::DF);
    ui->comboBoxDFRFDIRI->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RF),PCx_AuditModel::RF);
    ui->comboBoxDFRFDIRI->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DI),PCx_AuditModel::DI);
    ui->comboBoxDFRFDIRI->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RI),PCx_AuditModel::RI);

    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::ouverts)+"s",PCx_AuditModel::ouverts);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::realises)+"s",PCx_AuditModel::realises);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::engages)+"s",PCx_AuditModel::engages);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::disponibles)+"s",PCx_AuditModel::disponibles);

    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DF),PCx_AuditModel::DF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RF),PCx_AuditModel::RF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DI),PCx_AuditModel::DI);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RI),PCx_AuditModel::RI);


    updateListOfAudits();
}

FormQueries::~FormQueries()
{
    if(model!=NULL)
    {
        delete model;
    }
    delete ui;
}


void FormQueries::updateListOfAudits()
{
    ui->comboBoxListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(PCx_AuditModel::FinishedAuditsOnly);
    //do not update text browser if no audit are available
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    //doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));

    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboBoxListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboBoxListAudits->setCurrentIndex(0);
    on_comboBoxListAudits_activated(0);
}

void FormQueries::on_comboBoxListAudits_activated(int index)
{
    if(index==-1||ui->comboBoxListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboBoxListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
    }
    model=new PCx_AuditModel(selectedAuditId,this);

    ui->comboBoxTypes->clear();
    ui->comboBoxTypes_2->clear();
    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTreeModel()->getTypes()->getTypes();
    Q_ASSERT(!listOfTypes.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboBoxTypes->addItem(p.second,p.first);
        ui->comboBoxTypes_2->addItem(p.second,p.first);
    }
    ui->comboBoxTypes->setCurrentIndex(0);
    ui->comboBoxTypes_2->setCurrentIndex(0);

    ui->comboBoxYear1->clear();
    ui->comboBoxYear2->clear();
    ui->comboBoxYear1->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear2->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear1->setCurrentIndex(0);
    ui->comboBoxYear2->setCurrentIndex(ui->comboBoxYear2->count()-1);

    ui->comboBoxYear1_2->clear();
    ui->comboBoxYear2_2->clear();
    ui->comboBoxYear1_2->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear2_2->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear1_2->setCurrentIndex(0);
    ui->comboBoxYear2_2->setCurrentIndex(ui->comboBoxYear2->count()-1);



}


