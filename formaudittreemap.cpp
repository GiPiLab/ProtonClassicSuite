#include "formaudittreemap.h"
#include "ui_formaudittreemap.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>

const QString FormAuditTreemap::TOUT=QString("tout");

FormAuditTreemap::FormAuditTreemap(QWidget *parent):QWidget(parent),ui(new Ui::FormAuditTreemap)
{
    selectedYear=0;
    previousSelectedNodes.push(0);
    selectedNode=0;
    selectedMode=MODES::DF;
    selectedORED=PCx_Audit::ORED::OUVERTS;
    selectedAudit=nullptr;
    ui->setupUi(this);

    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS,true),PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES,true),PCx_Audit::ORED::REALISES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES,true),PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES,true),PCx_Audit::ORED::DISPONIBLES);

    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toLower(),MODES::DFRFDIRI::DF);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toLower(),MODES::DFRFDIRI::RF);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toLower(),MODES::DFRFDIRI::DI);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toLower(),MODES::DFRFDIRI::RI);
    updateListOfAudits();

}

FormAuditTreemap::~FormAuditTreemap()
{
    if(selectedAudit!=nullptr)
    {
        delete selectedAudit;
    }
    delete ui;
}

void FormAuditTreemap::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormAuditTreemap::updateListOfAudits()
{
    ui->comboListAudits->clear();
    previousSelectedNodes.clear();
    previousSelectedNodes.push(0);
    selectedNode=0;

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::FinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);

    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void FormAuditTreemap::updateTreeMapWidget()
{    
    if(selectedAudit!=nullptr)
    {
        ui->treeMapWidget->setData(selectedAudit,selectedMode,selectedORED,selectedYear,selectedNode);
    }
}

QSize FormAuditTreemap::sizeHint() const
{
    return QSize(450,400);
}

void FormAuditTreemap::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    if(selectedAuditId==0)
    {
        qFatal("Assertion failed");
    }

    if(selectedAudit!=nullptr)
        delete selectedAudit;

    selectedAudit=new PCx_Audit(selectedAuditId);

    ui->comboBoxYear->clear();
    ui->comboBoxYear->addItem(TOUT);

    QStringList yearsStringList=selectedAudit->getYearsStringList();    
    ui->comboBoxYear->addItems(yearsStringList);
    ui->comboBoxYear->setCurrentIndex(0);
    on_comboBoxYear_activated(0);
}

void FormAuditTreemap::on_comboBoxMode_activated(int index)
{

     if(index==-1||ui->comboBoxMode->count()==0)return;
     selectedMode=(MODES::DFRFDIRI)ui->comboBoxMode->currentData().toUInt();
     updateTreeMapWidget();
}

void FormAuditTreemap::on_comboBoxORED_activated(int index)
{

    if(index==-1||ui->comboBoxORED->count()==0)return;
    selectedORED=(PCx_Audit::ORED)ui->comboBoxORED->currentData().toUInt();
    updateTreeMapWidget();
}

void FormAuditTreemap::on_comboBoxYear_activated(int index)
{    
    if(index==-1||ui->comboBoxYear->count()==0)return;
    if(ui->comboBoxYear->currentText()==TOUT)
    {
        selectedYear=0;
        previousSelectedNodes.push(selectedNode);
        selectedNode=0;
    }
    else
    {
        selectedYear=ui->comboBoxYear->currentText().toUInt();
        if(selectedNode==0)
        {
            previousSelectedNodes.push(0);
            selectedNode=1;
        }
    }
    if(selectedAudit!=nullptr)
        updateTreeMapWidget();
}

void FormAuditTreemap::on_treeMapWidget_clicked(const QString &name, int id, int year, double value)
{
    if(selectedAudit==nullptr)
        return;
    if(id!=selectedNode && !selectedAudit->getAttachedTree()->isLeaf(id))
        previousSelectedNodes.push(selectedNode);
    selectedNode=id;
    selectedYear=year;
    if(year!=0)
        ui->comboBoxYear->setCurrentText(QString::number(year));
    else
        ui->comboBoxYear->setCurrentText(TOUT);
    updateTreeMapWidget();
}

void FormAuditTreemap::on_treeMapWidget_highlighted(const QString &name, int id, int year, double value)
{
    ui->labelNodeName->setText(name.toHtmlEscaped());
    ui->labelNodeValue->setText(QString(NUMBERSFORMAT::formatDouble(value)+"€ en %1").arg(year));
}

void FormAuditTreemap::on_pushButtonPrevious_clicked()
{
    if(selectedAudit==nullptr)
        return;
    if(!previousSelectedNodes.isEmpty())
    {
        selectedNode=previousSelectedNodes.pop();
        if(selectedNode==0)
        {
            ui->comboBoxYear->setCurrentText(TOUT);
        }
        updateTreeMapWidget();
        if(previousSelectedNodes.isEmpty())
        {
            previousSelectedNodes.push(0);
        }
    }
}

void FormAuditTreemap::on_pushButtonSave_clicked()
{
    if(selectedAudit==nullptr)
        return;
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en PNG"), "",tr("Images PNG (*.png)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("png",Qt::CaseInsensitive)!=0)
        fileName.append(".png");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        return;
    }
    file.close();

    if(ui->treeMapWidget->grab().save(fileName))
        QMessageBox::information(this,tr("Information"),tr("Image enregistrée"));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Image non enregistrée !"));
}
