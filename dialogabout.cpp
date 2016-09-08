#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QDate>
#include <QFile>
#include <QTextStream>
#include "utils.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->label->setText(QString("ProtonClassicSuite version du %1<br>©2006-%2 Laboratoire de Recherche pour le Développement Local<br><a href='http://gipilab.org'>http://gipilab.org</a>").arg(VERSION).arg(QDate::currentDate().year()));
    QFile eulaFile(":/EULA/EULA.txt");
    eulaFile.open(QIODevice::ReadOnly);
    QTextStream in(&eulaFile);
    in.setCodec("UTF-8");
    QString line=in.readAll();
    eulaFile.close();
    ui->plainTextEdit->setPlainText(line);

}



DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButton_clicked()
{
    accept();
}
