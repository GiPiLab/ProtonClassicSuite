#ifndef UTILS_H
#define UTILS_H

#include <QString>


void die(int retcode=1);

QString newDb();
bool loadDb(const QString &databaseName);
void initCurrentDb(void);
QString formatDouble(double num,unsigned int decimals=2);
QString generateUniqueFileName(const QString &suffix="");




#endif // UTILS_H
