#ifndef M14_TO_SQL_H
#define M14_TO_SQL_H

#include <QObject>
#include <QString>

class m14_to_sql : public QObject
{
    Q_OBJECT
public:
    explicit m14_to_sql(const QString &fileName,QObject *parent = 0);

signals:

public slots:
    void run();

private:
    QString file;

};

#endif // M14_TO_SQL_H
