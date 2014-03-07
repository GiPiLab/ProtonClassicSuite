#ifndef DBUTILS_H
#define DBUTILS_H
#include <QtSql/QSqlDatabase>

#define DBNAME "pcxdb.sqlite3"

void emptyDb(void);
QSqlDatabase loadDb(bool addDriver);
void initDb(void);

#endif // DBUTILS_H
