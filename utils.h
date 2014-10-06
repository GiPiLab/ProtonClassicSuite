#ifndef UTILS_H
#define UTILS_H

#include <QString>

#define VERSION __DATE__

//Currencies are stored as fixed point, multiplied with this coeff
#define FIXEDPOINTCOEFF 1000
//In order to avoid rounding errors with large numbers
#define FIXEDPOINTCORRECTION 0.0005
#define DEFAULTNUMDECIMALS 2


#ifndef INT64_MAX
# if __WORDSIZE == 64
#  define __INT64_C(c)	c ## L
# else
#  define __INT64_C(c)	c ## LL
# endif
#define INT64_MAX		(__INT64_C(9223372036854775807))
#endif




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


QString generateUniqueFileName(const QString &suffix=0);

bool dotToPdf(const QByteArray &dot, const QString &outputFileName);
QStringList yearsListToStringList(QList<unsigned int> years);


#endif // UTILS_H
