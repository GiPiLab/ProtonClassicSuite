#include "formreportingsupervision.h"
#include "ui_formreportingsupervision.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QTextDocument>
#include <QMessageBox>

FormReportingSupervision::FormReportingSupervision(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingSupervision)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    model=nullptr;
    proxyModel=nullptr;
    updateListOfReportings();
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP,true);
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS,true);
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES,true);
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES,true);
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES,true);
}

FormReportingSupervision::~FormReportingSupervision()
{
    delete ui;
    if(proxyModel!=nullptr)
        delete proxyModel;
    if(selectedReporting!=nullptr)
        delete selectedReporting;
    if(model!=nullptr)
        delete model;
}

void FormReportingSupervision::updateListOfReportings()
{
    ui->comboListReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> p;
    if(listOfReportings.count()==0)
    {
        setEnabled(false);
        return;
    }
    else
        setEnabled(true);

    foreach(p,listOfReportings)
    {
        ui->comboListReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListReportings->setCurrentIndex(0);
    this->on_comboListReportings_activated(0);
}

void FormReportingSupervision::onListOfReportingsChanged()
{
    updateListOfReportings();
}
void FormReportingSupervision::on_comboListReportings_activated(int index)
{
    if(index==-1 || ui->comboListReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }

    if(model!=nullptr)
    {
        delete model;
        model=nullptr;
    }

    if(proxyModel!=nullptr)
    {
        delete proxyModel;
        proxyModel=nullptr;
    }

    selectedReporting=new PCx_Reporting(selectedReportingId);
    model=new PCx_ReportingTableSupervisionModel(selectedReporting,getSelectedMode());
    proxyModel=new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    ui->tableView->setModel(proxyModel);
    ui->tableView->resizeColumnsToContents();
}

MODES::DFRFDIRI FormReportingSupervision::getSelectedMode() const
{
    if(ui->radioButtonDF->isChecked())
        return MODES::DFRFDIRI::DF;
    if(ui->radioButtonRF->isChecked())
        return MODES::DFRFDIRI::RF;
    if(ui->radioButtonDI->isChecked())
        return MODES::DFRFDIRI::DI;
    if(ui->radioButtonRI->isChecked())
        return MODES::DFRFDIRI::RI;

    else
    {
        qDebug()<<"Invalid selection";
    }
    return MODES::DFRFDIRI::DF;
}

void FormReportingSupervision::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        model->setMode(MODES::DFRFDIRI::DF);
}

void FormReportingSupervision::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        model->setMode(MODES::DFRFDIRI::RF);
}

void FormReportingSupervision::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        model->setMode(MODES::DFRFDIRI::DI);
}

void FormReportingSupervision::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        model->setMode(MODES::DFRFDIRI::RI);
}

void FormReportingSupervision::on_checkBoxBP_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxOuverts_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxRealises_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxEngages_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxDisponibles_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxTauxReal_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTREALISES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxTauxEcart_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::TAUXECART))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxVariationBP_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTBP))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxTauxEngage_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTENGAGES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxTauxDisponible_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_pushButtonExportHTML_clicked()
{
    QString out=selectedReporting->generateHTMLReportingTitle();

    out.append(QString("<p>%1</p>").arg(MODES::modeToCompleteString(getSelectedMode())));

    out.append(qTableViewToHtml(ui->tableView));

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

    QTextDocument doc;
    doc.setHtml(out);

    QString output=doc.toHtml("utf-8");
    output.replace(" -qt-block-indent:0;","");

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<output;
    stream.flush();
    file.close();
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName()));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));
}

QSize FormReportingSupervision::sizeHint() const
{
    return QSize(1050,500);
}

void FormReportingSupervision::on_checkBoxRealisePredit_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISESPREDITS))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }

}

void FormReportingSupervision::on_checkBoxDifference_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}



void FormReportingSupervision::on_checkBoxDECICO_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ECICO))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxERO2_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ERO2))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxRAC_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::RAC))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBox15NRest_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::NB15NRESTANTES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingSupervision::on_checkBoxCPP15N_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==model->getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::CPP15NR))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}
