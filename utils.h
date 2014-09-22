#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>

//Currencies are stored as fixed point, multiplied with this coeff
#define FIXEDPOINTCOEFF 1000
//In order to avoid rounding errors with large numbers
#define FIXEDPOINTCORRECTION 0.0005
#define DEFAULTNUMDECIMALS 2

//Maximum acceptable number
//Will be stored multiplied by FIXEDPOINTCOEFF into an int64
//Must be < MAX_DBL to fit in QDoubleSpinBox
#define MAX_NUM 4294967295u

enum FORMATMODE
{
    FORMATMODENORMAL,
    FORMATMODETHOUSANDS,
    FORMATMODEMILLIONS
};



void die(int retcode=EXIT_FAILURE);

unsigned int getCurrentNumDecimals();

void updateFormatModeAndDecimals();

QString newDb();
bool loadDb(const QString &databaseName);
void initCurrentDb(void);
QString formatDouble(double num,int decimals=-1, bool forcedUnits=false);
QString formatCurrency(qint64 num, int decimals=-1, bool forcedUnits=false);

qint64 doubleToFixedPoint(double num);
double fixedPointToDouble(qint64 num);


QString generateUniqueFileName(const QString &suffix="");

bool dotToPdf(const QByteArray &dot, const QString &outputFileName);
QStringList yearsListToStringList(QList<unsigned int> years);


#endif // UTILS_H
