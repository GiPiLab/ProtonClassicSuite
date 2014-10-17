#include "formqueries.h"
#include "ui_formqueries.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include "pcx_query.h"
#include "pcx_queryvariation.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"
#include "utils.h"
#include "pcx_auditmanage.h"


FormQueries::FormQueries(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormQueries)
{
    ui->setupUi(this);
    model=nullptr;
    queriesModel=nullptr;
    report=nullptr;

    onColorChanged();

    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)+"s",PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)+"s",PCx_Audit::ORED::REALISES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)+"s",PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)+"s",PCx_Audit::ORED::DISPONIBLES);

    ui->comboBoxDFRFDIRI->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DF).toLower(),PCx_Audit::DFRFDIRI::DF);
    ui->comboBoxDFRFDIRI->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RF).toLower(),PCx_Audit::DFRFDIRI::RF);
    ui->comboBoxDFRFDIRI->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DI).toLower(),PCx_Audit::DFRFDIRI::DI);
    ui->comboBoxDFRFDIRI->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RI).toLower(),PCx_Audit::DFRFDIRI::RI);

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

    ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)+"s",PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)+"s",PCx_Audit::ORED::REALISES);
    ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)+"s",PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)+"s",PCx_Audit::ORED::DISPONIBLES);

    ui->comboBoxDFRFDIRI_2->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DF).toLower(),PCx_Audit::DFRFDIRI::DF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RF).toLower(),PCx_Audit::DFRFDIRI::RF);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DI).toLower(),PCx_Audit::DFRFDIRI::DI);
    ui->comboBoxDFRFDIRI_2->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RI).toLower(),PCx_Audit::DFRFDIRI::RI);

    ui->comboBoxBiggerSmaller->addItem(PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::GREATER),PCx_QueryRank::GREATER);
    ui->comboBoxBiggerSmaller->addItem(PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::SMALLER),PCx_QueryRank::SMALLER);

    ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)+"s",PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)+"s",PCx_Audit::ORED::REALISES);
    ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)+"s",PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)+"s",PCx_Audit::ORED::DISPONIBLES);

    ui->comboBoxDFRFDIRI_3->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DF).toLower(),PCx_Audit::DFRFDIRI::DF);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RF).toLower(),PCx_Audit::DFRFDIRI::RF);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::DI).toLower(),PCx_Audit::DFRFDIRI::DI);
    ui->comboBoxDFRFDIRI_3->addItem(PCx_Audit::modeToCompleteString(PCx_Audit::DFRFDIRI::RI).toLower(),PCx_Audit::DFRFDIRI::RI);

    ui->doubleSpinBox->setMaximum(INT64_MAX/FIXEDPOINTCOEFF);
    ui->doubleSpinBox_2->setRange(-INT64_MAX/FIXEDPOINTCOEFF,INT64_MAX/FIXEDPOINTCOEFF);
    ui->doubleSpinBox_3->setRange(-INT64_MAX/FIXEDPOINTCOEFF,INT64_MAX/FIXEDPOINTCOEFF);

    doc=new QTextDocument();
    xlsDoc=nullptr;
    ui->textEdit->setDocument(doc);
    updateListOfAudits();
}


FormQueries::~FormQueries()
{
    if(model!=nullptr)
    {
        delete model;
    }
    if(report!=nullptr)
    {
        delete report;
    }
    if(queriesModel!=nullptr)
        delete queriesModel;
    delete doc;
    if(xlsDoc!=nullptr)
        delete xlsDoc;
    delete ui;
}

void FormQueries::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormQueries::onColorChanged()
{
    ui->frame->setStyleSheet(QString("background-color:"+PCx_QueryVariation::getColor().name()));
    ui->frame_2->setStyleSheet(QString("background-color:"+PCx_QueryRank::getColor().name()));
    ui->frame_3->setStyleSheet(QString("background-color:"+PCx_QueryMinMax::getColor().name()));
}


void FormQueries::updateListOfAudits()
{
    ui->comboBoxListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditManage::getListOfAudits(PCx_AuditManage::FinishedAuditsOnly);
    //do not update text browser if no audit are available
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    if(!nonEmpty)
        doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));

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

    if(model!=nullptr)
        delete model;

    if(report!=nullptr)
        delete report;

    if(queriesModel!=nullptr)
        delete queriesModel;

    model=new PCx_Audit(selectedAuditId);
    report=new PCx_Report(model);
    queriesModel=new PCx_QueriesModel(selectedAuditId,this);

    ui->listView->setModel(queriesModel);

    ui->comboBoxTypes->clear();
    ui->comboBoxTypes_2->clear();
    ui->comboBoxTypes_3->clear();
    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTree()->getAllTypes();
    Q_ASSERT(!listOfTypes.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboBoxTypes->addItem(p.second,p.first);
        ui->comboBoxTypes_2->addItem(p.second,p.first);
        ui->comboBoxTypes_3->addItem(p.second,p.first);
    }
    ui->comboBoxTypes->insertItem(0,tr("[noeuds]"),PCx_Query::ALLTYPES);
    ui->comboBoxTypes_2->insertItem(0,tr("[noeuds]"),PCx_Query::ALLTYPES);
    ui->comboBoxTypes_3->insertItem(0,tr("[noeuds]"),PCx_Query::ALLTYPES);

    ui->comboBoxTypes->setCurrentIndex(0);
    ui->comboBoxTypes_2->setCurrentIndex(0);
    ui->comboBoxTypes_3->setCurrentIndex(0);

    ui->comboBoxYear1->clear();
    ui->comboBoxYear2->clear();

    QStringList yearsStringList=model->getYearsStringList();
    ui->comboBoxYear1->addItems(yearsStringList);
    ui->comboBoxYear2->addItems(yearsStringList);
    ui->comboBoxYear1->setCurrentIndex(0);
    ui->comboBoxYear2->setCurrentIndex(ui->comboBoxYear2->count()-1);

    ui->comboBoxYear1_2->clear();
    ui->comboBoxYear2_2->clear();
    ui->comboBoxYear1_2->addItems(yearsStringList);
    ui->comboBoxYear2_2->addItems(yearsStringList);
    ui->comboBoxYear1_2->setCurrentIndex(0);
    ui->comboBoxYear2_2->setCurrentIndex(ui->comboBoxYear2_2->count()-1);

    ui->comboBoxYear1_3->clear();
    ui->comboBoxYear2_3->clear();
    ui->comboBoxYear1_3->addItems(yearsStringList);
    ui->comboBoxYear2_3->addItems(yearsStringList);
    ui->comboBoxYear1_3->setCurrentIndex(0);
    ui->comboBoxYear2_3->setCurrentIndex(ui->comboBoxYear2_3->count()-1);
    doc->clear();
}


bool FormQueries::getParamsReqRank(unsigned int &typeId, PCx_Audit::ORED &ored, PCx_Audit::DFRFDIRI &dfrfdiri,
                                   unsigned int &number, PCx_QueryRank::GREATERSMALLER &grSm, unsigned int &year1, unsigned int &year2)
{
    typeId=ui->comboBoxTypes_2->currentData().toUInt();
    ored=(PCx_Audit::ORED)ui->comboBoxORED_2->currentData().toUInt();
    dfrfdiri=(PCx_Audit::DFRFDIRI)ui->comboBoxDFRFDIRI_2->currentData().toUInt();
    grSm=(PCx_QueryRank::GREATERSMALLER)ui->comboBoxBiggerSmaller->currentData().toUInt();
    number=ui->spinBoxNumRank->value();
    year1=ui->comboBoxYear1_2->currentText().toUInt();
    year2=ui->comboBoxYear2_2->currentText().toUInt();
    return true;
}

bool FormQueries::getParamsReqMinMax(unsigned int &typeId, PCx_Audit::ORED &ored, PCx_Audit::DFRFDIRI &dfrfdiri, qint64 &val1, qint64 &val2, unsigned int &year1, unsigned int &year2)
{
    typeId=ui->comboBoxTypes_3->currentData().toUInt();
    ored=(PCx_Audit::ORED)ui->comboBoxORED_3->currentData().toUInt();
    dfrfdiri=(PCx_Audit::DFRFDIRI)ui->comboBoxDFRFDIRI_3->currentData().toUInt();
    val1=doubleToFixedPoint(ui->doubleSpinBox_2->value());
    val2=doubleToFixedPoint(ui->doubleSpinBox_3->value());
    year1=ui->comboBoxYear1_3->currentText().toUInt();
    year2=ui->comboBoxYear2_3->currentText().toUInt();
    return true;
}

bool FormQueries::getParamsReqVariation(unsigned int &typeId,PCx_Audit::ORED &ored, PCx_Audit::DFRFDIRI &dfrfdiri,
                   PCx_QueryVariation::INCREASEDECREASE &increase, PCx_QueryVariation::PERCENTORPOINTS &percent,
                   PCx_QueryVariation::OPERATORS &oper,qint64 &val, unsigned int &year1, unsigned int &year2)
{
    typeId=ui->comboBoxTypes->currentData().toUInt();
    ored=(PCx_Audit::ORED)ui->comboBoxORED->currentData().toUInt();
    dfrfdiri=(PCx_Audit::DFRFDIRI)ui->comboBoxDFRFDIRI->currentData().toUInt();
    increase=(PCx_QueryVariation::INCREASEDECREASE)ui->comboBoxAugDim->currentData().toUInt();
    percent=(PCx_QueryVariation::PERCENTORPOINTS)ui->comboBoxPercentEuro->currentData().toUInt();
    oper=(PCx_QueryVariation::OPERATORS)ui->comboBoxOperator->currentData().toUInt();
    val=doubleToFixedPoint(ui->doubleSpinBox->value());
    year1=ui->comboBoxYear1->currentText().toUInt();
    year2=ui->comboBoxYear2->currentText().toUInt();
    if(year1==year2)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Les années doivent être différentes !"));
        return false;
    }
    return true;
}


void FormQueries::on_pushButtonExecReqVariation_clicked()
{
    unsigned int typeId;
    PCx_Audit::ORED ored;
    PCx_Audit::DFRFDIRI dfrfdiri;
    PCx_QueryVariation::PERCENTORPOINTS pop;
    PCx_QueryVariation::INCREASEDECREASE incdec;
    PCx_QueryVariation::OPERATORS oper;
    qint64 val;
    unsigned int year1,year2;
    if(getParamsReqVariation(typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2)==false)
    {
        return;
    }

    PCx_QueryVariation qv(model,typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2);

    currentHtmlDoc=model->generateHTMLHeader();
    currentHtmlDoc.append(model->generateHTMLAuditTitle());
    if(xlsDoc!=nullptr)
    {
        delete xlsDoc;
        xlsDoc=nullptr;
    }
    xlsDoc=new QXlsx::Document();
    currentHtmlDoc.append(qv.exec(xlsDoc));
    currentHtmlDoc.append("</body></html>");
    doc->setHtml(currentHtmlDoc);
}


void FormQueries::on_comboBoxAugDim_activated(int index)
{
    Q_UNUSED(index);
    if((PCx_QueryVariation::INCREASEDECREASE)ui->comboBoxAugDim->currentData().toInt()==PCx_QueryVariation::VARIATION)
    {
        ui->doubleSpinBox->setMinimum(-INT64_MAX/FIXEDPOINTCOEFF);
    }
    else
    {
        ui->doubleSpinBox->setMinimum(0.0);
    }
}

void FormQueries::on_pushButtonSaveReqVariation_clicked()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouvelle requête"), tr("Nom de la requête : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        unsigned int typeId;
        PCx_Audit::ORED ored;
        PCx_Audit::DFRFDIRI dfrfdiri;
        PCx_QueryVariation::PERCENTORPOINTS pop;
        PCx_QueryVariation::INCREASEDECREASE incdec;
        PCx_QueryVariation::OPERATORS oper;
        qint64 val;
        unsigned int year1,year2;
        if(getParamsReqVariation(typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2)==false)
        {
            return;
        }

        PCx_QueryVariation qv(model,typeId,ored,dfrfdiri,incdec,pop,oper,val,year1,year2);
        if(!qv.canSave(text))
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà une requête de ce type portant ce nom !"));
            return;
        }
        if(!qv.save(text))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Impossible d'enregistrer la requête !"));
            return;
        }
        queriesModel->update();
    }
}

void FormQueries::on_pushButtonDelete_clicked()
{
    QModelIndexList selection=ui->listView->selectionModel()->selectedIndexes();
    if(selection.isEmpty())return;

    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>supprimer</b> les requêtes sélectionnées ? Cette action ne peut être annulée"))==QMessageBox::No)
    {
        return;
    }

    foreach (QModelIndex idx,selection)
    {
        unsigned int selectedQueryId=queriesModel->record(idx.row()).value("id").toUInt();
        PCx_Query::deleteQuery(model->getAuditId(),selectedQueryId);
    }
    queriesModel->update();
}

QString FormQueries::execQueries(QModelIndexList items, QXlsx::Document *xlsDocument)
{
    QProgressDialog progress(tr("Calculs en cours..."),tr("Annuler"),0,items.count(),this);
    progress.setMinimumDuration(200);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setValue(0);
    QString output=model->generateHTMLHeader();
    output.append(model->generateHTMLAuditTitle());

    int count=0;
    foreach(QModelIndex idx,items)
    {
        unsigned int selectedQueryId=queriesModel->record(idx.row()).value("id").toUInt();
        PCx_Query::QUERIESTYPES selectedQueryType=(PCx_Query::QUERIESTYPES)queriesModel->record(idx.row()).value("query_mode").toUInt();

        switch(selectedQueryType)
        {
        case PCx_Query::VARIATION:
        {
            PCx_QueryVariation pqv(model,selectedQueryId);
            output.append(pqv.exec(xlsDocument));
            break;
        }

        case PCx_Query::RANK:
        {
            PCx_QueryRank pqr(model,selectedQueryId);
            output.append(pqr.exec(xlsDocument));
            break;
        }

        case PCx_Query::MINMAX:
        {
            PCx_QueryMinMax pqmm(model,selectedQueryId);
            output.append(pqmm.exec(xlsDocument));
            break;
        }
        }
        count++;
        if(!progress.wasCanceled())
        {
            progress.setValue(count);
        }
        else
        {
            return QString();
        }
    }
    output.append("</body></html>");
    return output;
}


void FormQueries::on_pushButtonExecFromList_clicked()
{
    QModelIndexList selection=ui->listView->selectionModel()->selectedIndexes();
    if(selection.isEmpty())return;
    doc->clear();
    if(xlsDoc!=nullptr)
    {
        delete xlsDoc;
        xlsDoc=nullptr;
    }
    xlsDoc=new QXlsx::Document();
    currentHtmlDoc=execQueries(selection,xlsDoc);

    //Aborded in progress dialog
    if(currentHtmlDoc.isEmpty())
    {
        delete xlsDoc;
        xlsDoc=nullptr;
        return;
    }
    doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_listView_activated(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_pushButtonExecFromList_clicked();
}

void FormQueries::on_pushButtonSave_clicked()
{
    if(doc->isEmpty())return;
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le résultat de la requête en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible"));
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    QSettings settings;
    QString settingStyle=settings.value("output/style","CSS").toString();

    if(settingStyle=="INLINE")
    {
        stream<<doc->toHtml("utf-8");
    }
    else
    {
        stream<<currentHtmlDoc;
    }

    stream.flush();
    file.close();

}

void FormQueries::on_pushButtonExecReqRank_clicked()
{
    unsigned int typeId,number,year1,year2;
    PCx_Audit::ORED ored;
    PCx_Audit::DFRFDIRI dfrfdiri;
    PCx_QueryRank::GREATERSMALLER grSm;

    if(getParamsReqRank(typeId,ored,dfrfdiri,number,grSm,year1,year2)==false)
    {
        return;
    }

    PCx_QueryRank qr(model,typeId,ored,dfrfdiri,grSm,number,year1,year2);

    currentHtmlDoc=model->generateHTMLHeader();
    currentHtmlDoc.append(model->generateHTMLAuditTitle());

    if(xlsDoc!=nullptr)
    {
        delete xlsDoc;
        xlsDoc=nullptr;
    }
    xlsDoc=new QXlsx::Document();
    currentHtmlDoc.append(qr.exec(xlsDoc));
    currentHtmlDoc.append("</body></html>");
    doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_pushButtonSaveReqRank_clicked()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouvelle requête"), tr("Nom de la requête : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        unsigned int typeId,number,year1,year2;
        PCx_Audit::ORED ored;
        PCx_Audit::DFRFDIRI dfrfdiri;
        PCx_QueryRank::GREATERSMALLER grSm;

        if(getParamsReqRank(typeId,ored,dfrfdiri,number,grSm,year1,year2)==false)
        {
            return;
        }

        PCx_QueryRank qr(model,typeId,ored,dfrfdiri,grSm,number,year1,year2);


        if(!qr.canSave(text))
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà une requête de ce type portant ce nom !"));
            return;
        }
        if(!qr.save(text))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Impossible d'enregistrer la requête !"));
            return;
        }
        queriesModel->update();
    }
}

void FormQueries::on_pushButtonExecReq3_clicked()
{
    unsigned int typeId,year1,year2;
    PCx_Audit::ORED ored;
    PCx_Audit::DFRFDIRI dfrfdiri;
    qint64 val1,val2;


    if(getParamsReqMinMax(typeId,ored,dfrfdiri,val1,val2,year1,year2)==false)
    {
        return;
    }

    PCx_QueryMinMax qmm(model,typeId,ored,dfrfdiri,val1,val2,year1,year2);

    currentHtmlDoc=model->generateHTMLHeader();
    currentHtmlDoc.append(model->generateHTMLAuditTitle());

    if(xlsDoc!=nullptr)
    {
        delete xlsDoc;
        xlsDoc=nullptr;
    }
    xlsDoc=new QXlsx::Document();
    currentHtmlDoc.append(qmm.exec(xlsDoc));
    currentHtmlDoc.append("</body></html>");
    doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_pushButtonSaveReq3_clicked()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouvelle requête"), tr("Nom de la requête : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        unsigned int typeId,year1,year2;
        qint64 val1,val2;
        PCx_Audit::ORED ored;
        PCx_Audit::DFRFDIRI dfrfdiri;

        if(getParamsReqMinMax(typeId,ored,dfrfdiri,val1,val2,year1,year2)==false)
        {
            return;
        }

        PCx_QueryMinMax qr(model,typeId,ored,dfrfdiri,val1,val2,year1,year2);


        if(!qr.canSave(text))
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà une requête de ce type portant ce nom !"));
            return;
        }
        if(!qr.save(text))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Impossible d'enregistrer la requête !"));
            return;
        }
        queriesModel->update();
    }
}

void FormQueries::on_pushButtonSaveXLSX_clicked()
{
    if(doc->isEmpty()||xlsDoc==nullptr)return;
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le résultat de la requête en XLSX"), "",tr("Fichiers XLSX (*.xlsx)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
        fileName.append(".xlsx");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible"));
        return;
    }

    xlsDoc->saveAs(&file);

    file.close();
}
