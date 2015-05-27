#include "formaudittreemap.h"
#include "ui_formaudittreemap.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QPainter>


FormAuditTreemap::FormAuditTreemap(QWidget *parent):QWidget(parent),ui(new Ui::FormAuditTreemap)
{
    selectedNode=1;
    selectedMode=MODES::DFRFDIRI::DF;
    selectedORED=PCx_Audit::ORED::OUVERTS;
    selectedAudit=nullptr;
    ui->setupUi(this);

    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS,true),(int)PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES,true),(int)PCx_Audit::ORED::REALISES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES,true),(int)PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES,true),(int)PCx_Audit::ORED::DISPONIBLES);

    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toLower(),(int)MODES::DFRFDIRI::DF);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toLower(),(int)MODES::DFRFDIRI::RF);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toLower(),(int)MODES::DFRFDIRI::DI);
    ui->comboBoxMode->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toLower(),(int)MODES::DFRFDIRI::RI);
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

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);
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
        ui->labelSelectedNodeName->setText(selectedAudit->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped());
        if(ui->radioButtonGroupByMode->isChecked())
        {
            ui->treeMapWidget->setDataGroupByModes(selectedAudit,selectedORED,selectedYear,selectedNode);
        }
        else if(ui->radioButtonGroupByORED->isChecked())
        {
            ui->treeMapWidget->setDataGroupByORED(selectedAudit,selectedMode,selectedYear,selectedNode);
        }
        else if(ui->radioButtonGroupByYear->isChecked())
        {
            ui->treeMapWidget->setDataGroupByYears(selectedAudit,selectedMode,selectedORED,selectedNode);
        }
        else
        {
            ui->treeMapWidget->setDataGroupByNode(selectedAudit,selectedMode,selectedORED,selectedYear,selectedNode);
        }
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
    selectedNode=1;

    ui->comboBoxYear->clear();

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

    selectedYear=ui->comboBoxYear->currentText().toUInt();

    if(selectedAudit!=nullptr)
        updateTreeMapWidget();
}

void FormAuditTreemap::on_treeMapWidget_clicked(const QString &name, int id, int year, double value)
{   
    Q_UNUSED(name);
    Q_UNUSED(year);
    Q_UNUSED(value);
    if(selectedAudit==nullptr)
        return;    
    selectedNode=id;    
    updateTreeMapWidget();
}

void FormAuditTreemap::on_treeMapWidget_highlighted(const QString &name, int id, int year, double value)
{
    Q_UNUSED(id);
    ui->labelNodeName->setText(name.toHtmlEscaped());
    ui->labelNodeValue->setText(QString(NUMBERSFORMAT::formatDouble(value)+"€ en %1").arg(year));
}

void FormAuditTreemap::on_pushButtonGoToPid_clicked()
{
    if(selectedAudit==nullptr)
        return;

    if(selectedNode>1)
        selectedNode=selectedAudit->getAttachedTree()->getParentId(selectedNode);

    updateTreeMapWidget();
}

void FormAuditTreemap::on_pushButtonSave_clicked()
{
    if(selectedAudit==nullptr)
        return;

    QString caption=selectedAudit->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped()+"\n";
    QStringList items;
    if(!ui->radioButtonGroupByMode->isChecked())
    {
        items.append(MODES::modeToCompleteString(selectedMode).toHtmlEscaped());
    }
    if(!ui->radioButtonGroupByORED->isChecked())
    {
        items.append(PCx_Audit::OREDtoCompleteString(selectedORED).toHtmlEscaped());
    }
    if(!ui->radioButtonGroupByYear->isChecked())
    {
        items.append(QString::number(selectedYear));
    }

    caption.append(items.join(", "));

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


    QPixmap pixmap=ui->treeMapWidget->grab();
    QPixmap withLegend(pixmap.width(),pixmap.height()+40);
    withLegend.fill(Qt::white);
    QPainter p(&withLegend);
    p.drawPixmap(0,0,pixmap);
    QRect rect(0,pixmap.height(),pixmap.width(),40);

    p.drawText(rect,Qt::AlignCenter,caption);

    p.end();




    if(withLegend.save(fileName))
        QMessageBox::information(this,tr("Information"),tr("Image enregistrée"));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Image non enregistrée !"));
}

void FormAuditTreemap::on_radioButtonGroupByMode_toggled(bool checked)
{
    if(checked)
    {
        updateTreeMapWidget();
        ui->comboBoxYear->setEnabled(true);
        ui->comboBoxMode->setEnabled(false);
        ui->comboBoxORED->setEnabled(true);
    }
}

void FormAuditTreemap::on_radioButtonGroupByORED_toggled(bool checked)
{
    if(checked)
    {
        ui->comboBoxYear->setEnabled(true);
        ui->comboBoxMode->setEnabled(true);
        ui->comboBoxORED->setEnabled(false);
        updateTreeMapWidget();
    }
}

void FormAuditTreemap::on_radioButtonGroupByYear_toggled(bool checked)
{
    if(checked)
    {
        ui->comboBoxYear->setEnabled(false);
        ui->comboBoxMode->setEnabled(true);
        ui->comboBoxORED->setEnabled(true);
        updateTreeMapWidget();
    }
}

void FormAuditTreemap::on_radioButtonGroupByNode_toggled(bool checked)
{
    if(checked)
    {
        ui->comboBoxYear->setEnabled(true);
        ui->comboBoxMode->setEnabled(true);
        ui->comboBoxORED->setEnabled(true);
        updateTreeMapWidget();
    }

}
