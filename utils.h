#ifndef UTILS_H
#define UTILS_H

#include <QSqlDatabase>
#include <QString>

#define DBNAME "pcxdb.sqlite3"


void die(int retcode=1);

void emptyDb(void);
QSqlDatabase loadDb(bool addDriver);
void initDb(void);
QString formatDouble(double num);




#endif // UTILS_H
