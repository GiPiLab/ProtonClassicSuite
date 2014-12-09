#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QDate>
#include "utils.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->label->setText(QString("ProtonClassicSuite release %1<br>©2006-%2 Laboratoire de Recherche pour le Développement Local<br><a href='http://gipilab.org'>http://gipilab.org</a>").arg(VERSION).arg(QDate::currentDate().year()));
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButton_clicked()
{
    accept();
}
