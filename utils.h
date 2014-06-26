#ifndef UTILS_H
#define UTILS_H

#include <QString>

//Currencies are stored as fixed point, multiplied with this coeff
#define FIXEDPOINTCOEFF 1000

//Maximum acceptable number
//Will be stored multiplied by FIXEDPOINTCOEFF into an int64
//Must be < MAX_DBL to fit in QDoubleSpinBox
#define MAX_NUM 4294967295

void die(int retcode=1);

QString newDb();
bool loadDb(const QString &databaseName);
void initCurrentDb(void);
QString formatDouble(double num,unsigned int decimals=2);
QString formatCurrency(qint64 num);
QString generateUniqueFileName(const QString &suffix="");




#endif // UTILS_H
