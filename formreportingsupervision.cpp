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
}

void FormReportingSupervision::setColumnVisibility()
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP,!ui->checkBoxBP->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS,!ui->checkBoxOuverts->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES,!ui->checkBoxRealises->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES,!ui->checkBoxEngages->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES,!ui->checkBoxDisponibles->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::CPP15NR,!ui->checkBoxCPP15N->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::RAC,!ui->checkBoxRAC->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::TAUXECART,!ui->checkBoxTauxEcart->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTBP,!ui->checkBoxVariationBP->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES,!ui->checkBoxTauxDisponible->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTENGAGES,!ui->checkBoxTauxEngage->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTREALISES,!ui->checkBoxTauxReal->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::NB15NRESTANTES,!ui->checkBox15NRest->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS,!ui->checkBoxDifference->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ECICO,!ui->checkBoxDECICO->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ERO2,!ui->checkBoxERO2->isChecked());
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISESPREDITS,!ui->checkBoxRealisePredit->isChecked());
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

void FormReportingSupervision::onReportingDataChanged(unsigned int reportingId)
{
    if(selectedReporting->getReportingId()==reportingId)
    {
        model->setMode(getSelectedMode());
    }
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
    setColumnVisibility();
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
        qWarning()<<"Invalid selection";
    }
    return MODES::DFRFDIRI::DF;
}

void FormReportingSupervision::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        model->setMode(MODES::DFRFDIRI::DF);
        ui->tableView->resizeColumnsToContents();
    }

}

void FormReportingSupervision::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        model->setMode(MODES::DFRFDIRI::RF);
        ui->tableView->resizeColumnsToContents();
    }

}

void FormReportingSupervision::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        model->setMode(MODES::DFRFDIRI::DI);
        ui->tableView->resizeColumnsToContents();
    }

}

void FormReportingSupervision::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        model->setMode(MODES::DFRFDIRI::RI);
        ui->tableView->resizeColumnsToContents();
    }
}

void FormReportingSupervision::on_checkBoxBP_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP,!checked);
}

void FormReportingSupervision::on_checkBoxOuverts_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS,!checked);
}

void FormReportingSupervision::on_checkBoxRealises_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES,!checked);
}

void FormReportingSupervision::on_checkBoxEngages_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES,!checked);
}

void FormReportingSupervision::on_checkBoxDisponibles_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES,!checked);
}

void FormReportingSupervision::on_checkBoxTauxReal_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTREALISES,!checked);
}

void FormReportingSupervision::on_checkBoxTauxEcart_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::TAUXECART,!checked);
}

void FormReportingSupervision::on_checkBoxVariationBP_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTBP,!checked);
}

void FormReportingSupervision::on_checkBoxTauxEngage_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTENGAGES,!checked);
}

void FormReportingSupervision::on_checkBoxTauxDisponible_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES,!checked);
}

void FormReportingSupervision::on_checkBoxRealisePredit_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISESPREDITS,!checked);
}

void FormReportingSupervision::on_checkBoxDifference_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS,!checked);
}

void FormReportingSupervision::on_checkBoxDECICO_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ECICO,!checked);
}

void FormReportingSupervision::on_checkBoxERO2_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ERO2,!checked);
}

void FormReportingSupervision::on_checkBoxRAC_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::RAC,!checked);
}

void FormReportingSupervision::on_checkBox15NRest_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::NB15NRESTANTES,!checked);
}

void FormReportingSupervision::on_checkBoxCPP15N_toggled(bool checked)
{
    ui->tableView->setColumnHidden(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::CPP15NR,!checked);
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

void FormReportingSupervision::on_pushButtonSelectNone_clicked()
{
    ui->checkBox15NRest->setChecked(false);
    ui->checkBoxBP->setChecked(false);
    ui->checkBoxCPP15N->setChecked(false);
    ui->checkBoxDECICO->setChecked(false);
    ui->checkBoxDifference->setChecked(false);
    ui->checkBoxDisponibles->setChecked(false);
    ui->checkBoxEngages->setChecked(false);
    ui->checkBoxERO2->setChecked(false);
    ui->checkBoxDECICO->setChecked(false);
    ui->checkBoxTauxDisponible->setChecked(false);
    ui->checkBoxTauxEcart->setChecked(false);
    ui->checkBoxTauxEngage->setChecked(false);
    ui->checkBoxTauxReal->setChecked(false);
    ui->checkBoxRAC->setChecked(false);
    ui->checkBoxRealisePredit->setChecked(false);
    ui->checkBoxRealises->setChecked(false);
    ui->checkBoxOuverts->setChecked(false);
    ui->checkBoxVariationBP->setChecked(false);
}

void FormReportingSupervision::on_pushButtonSelectAll_clicked()
{
    ui->checkBox15NRest->setChecked(true);
    ui->checkBoxBP->setChecked(true);
    ui->checkBoxCPP15N->setChecked(true);
    ui->checkBoxDECICO->setChecked(true);
    ui->checkBoxDifference->setChecked(true);
    ui->checkBoxDisponibles->setChecked(true);
    ui->checkBoxEngages->setChecked(true);
    ui->checkBoxERO2->setChecked(true);
    ui->checkBoxDECICO->setChecked(true);
    ui->checkBoxTauxDisponible->setChecked(true);
    ui->checkBoxTauxEcart->setChecked(true);
    ui->checkBoxTauxEngage->setChecked(true);
    ui->checkBoxTauxReal->setChecked(true);
    ui->checkBoxRAC->setChecked(true);
    ui->checkBoxRealisePredit->setChecked(true);
    ui->checkBoxRealises->setChecked(true);
    ui->checkBoxOuverts->setChecked(true);
    ui->checkBoxVariationBP->setChecked(true);
}
