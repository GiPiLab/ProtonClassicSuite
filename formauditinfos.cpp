#include "formauditinfos.h"
#include "ui_formauditinfos.h"
#include "pcx_report.h"


FormAuditInfos::FormAuditInfos(unsigned int auditId,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditInfos),auditId(auditId)
{
    infos=new PCx_Audit(auditId);
    ui->setupUi(this);
    QString out=infos->generateHTMLHeader();
    out.append(infos->generateHTMLAuditTitle());

    out.append(infos->getHTMLAuditStatistics());
    out.append("</body></html>");
    ui->textEdit->setHtml(out);
}

FormAuditInfos::~FormAuditInfos()
{
    delete infos;
    delete ui;
}

QSize FormAuditInfos::sizeHint() const
{
    return QSize(650,450);
}

void FormAuditInfos::on_pushButton_clicked()
{
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

    QString output=infos->generateHTMLHeader();
    output.append(infos->generateHTMLAuditTitle());
    output.append(infos->getHTMLAuditStatistics());
    output.append("</body></html>");

    QString settingStyle=settings.value("output/style","CSS").toString();
    if(settingStyle=="INLINE")
    {
        //Pass HTML through a temp QTextDocument to reinject css into tags (more compatible with text editors)
        QTextDocument formattedOut;
        formattedOut.setHtml(output);
        output=formattedOut.toHtml("utf-8");

        //Cleanup the output a bit
        output.replace(" -qt-block-indent:0;","");
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<output;
    stream.flush();
    file.close();

    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName().toHtmlEscaped()));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));

}
