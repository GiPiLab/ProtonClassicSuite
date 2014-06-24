#include "formqueries.h"
#include "ui_formqueries.h"
#include <QDebug>
#include "pcx_queries.h"
#include "pcx_queryvariation.h"


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

    ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::INCREASE),PCx_QueryVariation::INCREASE);
    ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::DECREASE),PCx_QueryVariation::DECREASE);
    ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::VARIATION),PCx_QueryVariation::VARIATION);

    ui->comboBoxPercentEuro->addItem(PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::PERCENT),PCx_QueryVariation::PERCENT);
    ui->comboBoxPercentEuro->addItem(PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::POINTS),PCx_QueryVariation::POINTS);

    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::LOWERTHAN),PCx_QueryVariation::LOWERTHAN);
    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::LOWEROREQUAL),PCx_QueryVariation::LOWEROREQUAL);
    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::EQUAL),PCx_QueryVariation::EQUAL);
    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::NOTEQUAL),PCx_QueryVariation::NOTEQUAL);
    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::GREATEROREQUAL),PCx_QueryVariation::GREATEROREQUAL);
    ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::GREATERTHAN),PCx_QueryVariation::GREATERTHAN);

    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::ouverts)+"s",PCx_AuditModel::ouverts);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::realises)+"s",PCx_AuditModel::realises);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::engages)+"s",PCx_AuditModel::engages);
    ui->comboBoxORED_2->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::disponibles)+"s",PCx_AuditModel::disponibles);

    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DF),PCx_AuditModel::DF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RF),PCx_AuditModel::RF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DI),PCx_AuditModel::DI);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RI),PCx_AuditModel::RI);

    ui->comboBoxORED_3->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::ouverts)+"s",PCx_AuditModel::ouverts);
    ui->comboBoxORED_3->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::realises)+"s",PCx_AuditModel::realises);
    ui->comboBoxORED_3->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::engages)+"s",PCx_AuditModel::engages);
    ui->comboBoxORED_3->addItem(PCx_AuditModel::OREDtoCompleteString(PCx_AuditModel::disponibles)+"s",PCx_AuditModel::disponibles);

    ui->comboBoxDFRFDIRI_3->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DF),PCx_AuditModel::DF);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RF),PCx_AuditModel::RF);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::DI),PCx_AuditModel::DI);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_AuditModel::modeToCompleteString(PCx_AuditModel::RI),PCx_AuditModel::RI);

    doc=new QTextDocument();
    ui->textBrowser->setDocument(doc);
    updateListOfAudits();
}


FormQueries::~FormQueries()
{
    if(model!=NULL)
    {
        delete model;
    }
    delete doc;
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
        delete report;
    }
    model=new PCx_AuditModel(selectedAuditId,this);
    report=new PCx_Report(model);

    ui->comboBoxTypes->clear();
    ui->comboBoxTypes_2->clear();
    ui->comboBoxTypes_3->clear();
    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTreeModel()->getTypes()->getTypes();
    Q_ASSERT(!listOfTypes.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboBoxTypes->addItem(p.second,p.first);
        ui->comboBoxTypes_2->addItem(p.second,p.first);
        ui->comboBoxTypes_3->addItem(p.second,p.first);
    }
    ui->comboBoxTypes->insertItem(0,tr("[noeuds]"),PCx_Queries::ALLTYPES);
    ui->comboBoxTypes_2->insertItem(0,tr("[noeuds]"),PCx_Queries::ALLTYPES);
    ui->comboBoxTypes_3->insertItem(0,tr("[noeuds]"),PCx_Queries::ALLTYPES);

    ui->comboBoxTypes->setCurrentIndex(0);
    ui->comboBoxTypes_2->setCurrentIndex(0);
    ui->comboBoxTypes_3->setCurrentIndex(0);

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
    ui->comboBoxYear2_2->setCurrentIndex(ui->comboBoxYear2_2->count()-1);

    ui->comboBoxYear1_3->clear();
    ui->comboBoxYear2_3->clear();
    ui->comboBoxYear1_3->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear2_3->addItems(model->getAuditInfos().yearsStringList);
    ui->comboBoxYear1_3->setCurrentIndex(0);
    ui->comboBoxYear2_3->setCurrentIndex(ui->comboBoxYear2_3->count()-1);
    doc->clear();
}

void FormQueries::getParamsReq1(unsigned int &typeId,PCx_AuditModel::ORED &ored, PCx_AuditModel::DFRFDIRI &dfrfdiri,
                   PCx_QueryVariation::INCREASEDECREASE &increase, PCx_QueryVariation::PERCENTORPOINTS &percent,
                   PCx_QueryVariation::OPERATORS &oper,double &val, unsigned int &year1, unsigned int &year2) const
{
    typeId=ui->comboBoxTypes->currentData().toUInt();
    ored=(PCx_AuditModel::ORED)ui->comboBoxORED->currentData().toUInt();
    dfrfdiri=(PCx_AuditModel::DFRFDIRI)ui->comboBoxDFRFDIRI->currentData().toUInt();
    increase=(PCx_QueryVariation::INCREASEDECREASE)ui->comboBoxAugDim->currentData().toUInt();
    percent=(PCx_QueryVariation::PERCENTORPOINTS)ui->comboBoxPercentEuro->currentData().toUInt();
    oper=(PCx_QueryVariation::OPERATORS)ui->comboBoxOperator->currentData().toUInt();
    val=ui->doubleSpinBox->value();
    year1=ui->comboBoxYear1->currentText().toUInt();
    year2=ui->comboBoxYear2->currentText().toUInt();
}


void FormQueries::on_pushButtonExecReq1_clicked()
{
    unsigned int typeId;
    PCx_AuditModel::ORED ored;
    PCx_AuditModel::DFRFDIRI dfrfdiri;
    PCx_QueryVariation::PERCENTORPOINTS pop;
    PCx_QueryVariation::INCREASEDECREASE incdec;
    PCx_QueryVariation::OPERATORS oper;
    double val;
    unsigned int year1,year2;
    getParamsReq1(typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2);

    PCx_QueryVariation qv(model,typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2);

    QString output=report->generateHTMLHeader();
    output.append(qv.exec());
    doc->setHtml(output);
}

