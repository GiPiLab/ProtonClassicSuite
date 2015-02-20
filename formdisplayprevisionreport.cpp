#include "formdisplayprevisionreport.h"
#include "ui_formdisplayprevisionreport.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

FormDisplayPrevisionReport::FormDisplayPrevisionReport(PCx_PrevisionItem *previsionItem, QWidget *parent) :
    QWidget(parent),ui(new Ui::FormDisplayPrevisionReport),previsionItem(previsionItem)
{
    ui->setupUi(this);
    doc=new QTextDocument();
    previsionItem->displayPrevisionItemReportInQTextDocument(doc);
    ui->textBrowser->setDocument(doc);
}

FormDisplayPrevisionReport::~FormDisplayPrevisionReport()
{
    delete doc;
    delete ui;
}

void FormDisplayPrevisionReport::on_pushButtonSaveHTML_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    previsionItem->savePrevisionItemReport(fileName,false);
}

QSize FormDisplayPrevisionReport::sizeHint() const
{
    return QSize(800,600);
}
