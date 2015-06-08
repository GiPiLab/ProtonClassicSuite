#include "formtutorial.h"
#include "ui_formtutorial.h"
#include <QDebug>
#include <QListWidgetItem>

FormTutorial::FormTutorial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTutorial)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    QString basePath=QCoreApplication::applicationDirPath()+"/screencasts";

    QListWidgetItem *item=new QListWidgetItem(tr("Arbres, les bases"),ui->listWidget);
    item->setData(Qt::UserRole,QUrl::fromLocalFile(basePath+"/scrcast_arbre_1.mkv"));
    item->setData(Qt::UserRole+1,tr("Création manuelle d'un arbre"));

    item=new QListWidgetItem(tr("Importer et exporter un arbre"),ui->listWidget);
    item->setData(Qt::UserRole,QUrl::fromLocalFile(basePath+"/scrcast_arbre_import_export_excel.mkv"));
    item->setData(Qt::UserRole+1,tr("Création d'un arbre à partir d'un fichier tableur, structuration manuelle et export"));

    item=new QListWidgetItem(tr("Structurer un arbre"),ui->listWidget);
    item->setData(Qt::UserRole,QUrl::fromLocalFile(basePath+"/scrcast_arbre_deviner_hierarchie.mkv"));
    item->setData(Qt::UserRole+1,tr("Deviner la hiérarchie d'un arbre à partir du nom de ses noeuds"));

    item=new QListWidgetItem(tr("Audits, les bases"),ui->listWidget);
    item->setData(Qt::UserRole,QUrl::fromLocalFile(basePath+"/scrcast_aleatoires_et_explorateur_audits.mkv"));
    item->setData(Qt::UserRole+1,tr("Création d'un audit à partir de données aléatoires"));


    player=new QMediaPlayer;
    player->setVideoOutput(ui->videoWidget);
    ui->toolButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    connect(player, &QMediaPlayer::stateChanged,
            this, &FormTutorial::mediaStateChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &FormTutorial::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &FormTutorial::durationChanged);
}

FormTutorial::~FormTutorial()
{
    player->stop();
    delete player;
    delete ui;
}

void FormTutorial::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
    case QMediaPlayer::PlayingState:
    case QMediaPlayer::StoppedState:
        ui->toolButtonPause->setChecked(false);
        break;
    case QMediaPlayer::PausedState:
        ui->toolButtonPause->setChecked(true);
        break;
    }
}

void FormTutorial::positionChanged(qint64 position)
{
    ui->horizontalSlider->setValue(position);
}

void FormTutorial::durationChanged(qint64 duration)
{
    ui->horizontalSlider->setRange(0,duration);
}

void FormTutorial::on_listWidget_activated(const QModelIndex &index)
{
    player->setMedia(index.data(Qt::UserRole).toUrl());
    ui->toolButtonPause->setChecked(false);
    player->play();
}

void FormTutorial::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    ui->labelDescription->setText(current->data(Qt::UserRole+1).toString());
}

void FormTutorial::on_toolButtonPause_clicked()
{
    if(player->state()==QMediaPlayer::PlayingState)
    {
        player->pause();
    }
    else if(player->state()==QMediaPlayer::PausedState)
    {
        player->play();
    }
}

void FormTutorial::on_horizontalSlider_valueChanged(int value)
{
    player->setPosition(value);
    if(player->state()==QMediaPlayer::StoppedState)
    {
        player->play();
    }
}

QSize FormTutorial::sizeHint() const
{
    return QSize(700,400);
}
