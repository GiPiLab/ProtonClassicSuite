#ifndef FORMTUTORIAL_H
#define FORMTUTORIAL_H

#include <QWidget>
#include <QMediaPlayer>
#include <QListWidgetItem>

namespace Ui {
class FormTutorial;
}

class FormTutorial : public QWidget
{
    Q_OBJECT

public:
    explicit FormTutorial(QWidget *parent = 0);
    ~FormTutorial();

private slots:

    void mediaStateChanged(QMediaPlayer::State);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

    void on_listWidget_activated(const QModelIndex &index);

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);


    void on_toolButtonPause_clicked();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::FormTutorial *ui;
    QSize sizeHint() const;
    QMediaPlayer *player;

};

#endif // FORMTUTORIAL_H
