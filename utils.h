#ifndef UTILS_H
#define UTILS_H

#include <QSqlDatabase>
#include <QString>


void die(int retcode=1);

QString newDb();
bool loadDb(const QString &databaseName);
void initCurrentDb(void);
QString formatDouble(double num,unsigned int decimals=2);




#endif // UTILS_H
