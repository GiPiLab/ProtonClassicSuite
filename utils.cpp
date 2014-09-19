#include "utils.h"

#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QtGlobal>
#include <QSettings>
#include <cstdio>
#include <QDebug>

#include <graphviz/gvc.h>


//To speedup formatCurrency and formatDouble
FORMATMODE currentFormatMode=FORMATMODENORMAL;
unsigned int currentNumDecimals=DEFAULTNUMDECIMALS;

unsigned int getCurrentNumDecimals()
{
    return currentNumDecimals;
}


void updateFormatModeAndDecimals()
{
    QSettings settings;
    currentFormatMode=(FORMATMODE)settings.value("format/formatMode",FORMATMODENORMAL).toUInt();
    currentNumDecimals=settings.value("format/numdecimals",DEFAULTNUMDECIMALS).toUInt();
}

void die(int retcode)
{
    QSqlDatabase::database().close();
    QApplication::instance()->exit(retcode);
    exit(retcode);
}

QString formatCurrency(qint64 num, int decimals, bool forcedUnits)
{
    QLocale locale;
    QString out;

    if(forcedUnits==false)
    {
        switch(currentFormatMode)
        {
        case FORMATMODENORMAL:
            break;
        case FORMATMODETHOUSANDS:
            out="k";
            num/=1000;
            break;
        case FORMATMODEMILLIONS:
            out="M";
            num/=1000000;
        }
    }

    //Forced decimals mode
    if(decimals>=0)
        out.prepend(locale.toString(fixedPointToDouble(num),'f',decimals));
    else
        out.prepend(locale.toString(fixedPointToDouble(num),'f',currentNumDecimals));
    return out;
}

QString formatDouble(double num, int decimals, bool forcedUnits)
{
    QLocale locale;
    QString out;

    if(forcedUnits==false)
    {
        switch(currentFormatMode)
        {
        case FORMATMODENORMAL:
            break;
        case FORMATMODETHOUSANDS:
            out="k";
            num/=1000;
            break;
        case FORMATMODEMILLIONS:
            out="M";
            num/=1000000;
        }
    }
    //Forced decimals mode
    if(decimals>=0)
        out.prepend(locale.toString(num,'f',decimals));
    else
        out.prepend(locale.toString(num,'f',currentNumDecimals));

    return out;
}

void initCurrentDb(void)
{
    QSqlQuery query;

    query.exec("create table if not exists index_arbres(id integer primary key autoincrement, nom text unique not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        qFatal(query.lastError().text().toLocal8Bit());
        exit(-1);
    }

    query.exec("create table if not exists index_audits(id integer primary key autoincrement, nom text unique not null, id_arbre integer not null, annees text not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        qFatal(query.lastError().text().toLocal8Bit());
        exit(-1);
    }
}

bool loadDb(const QString &databaseName)
{
    QSqlDatabase db=QSqlDatabase::database();
    if(!databaseName.isNull())
        db.setDatabaseName(databaseName);

    if(!db.open())
    {
        qFatal(db.lastError().text().toLocal8Bit());
        exit(-1);
    }

    /*QSqlQuery("PRAGMA journal_mode=OFF");
    QSqlQuery("PRAGMA locking_mode = EXCLUSIVE");
    QSqlQuery("PRAGMA synchronous=OFF");
    QSqlQuery("PRAGMA page_size = 4096");
    QSqlQuery("PRAGMA cache_size = 16384");
    QSqlQuery("PRAGMA temp_store = MEMORY");*/


    return true;
}


QString newDb()
{
    QString tmpDbName=QUuid::createUuid().toString();
    tmpDbName.chop(1);
    tmpDbName=tmpDbName.remove(0,1);
    tmpDbName.append(".pcxdb");

    tmpDbName.prepend(QDir::separator());
    tmpDbName.prepend(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    return tmpDbName;
}

QString generateUniqueFileName(const QString &suffix)
{
    QString imageName=QUuid::createUuid().toString();
    imageName.chop(1);
    imageName=imageName.remove(0,1).append(suffix);
    return imageName;
}


bool dotToPdf(const QByteArray &dot, const QString &outputFileName)
{
    Q_ASSERT(!outputFileName.isEmpty());

    QFile output(outputFileName);

    const char *dotData=dot.constData();

    GVC_t *gvc;
    graph_t *g;

    gvc = gvContext();

    g = agmemread(dotData);

    if(g==NULL)
    {
        gvFreeContext(gvc);
        qCritical()<<"Error while reading DOT data !";
        return false;
    }

    if(gvLayout(gvc, g, "dot")!=0)
    {
        gvFreeLayout(gvc, g);
        agclose(g);
        gvFreeContext(gvc);
        qCritical()<<"Error during graph layout !";
        return false;
    }

    char *outputData;
    unsigned int length=0;

    gvRenderData(gvc, g, "pdf", &outputData,&length);

    if(length==0)
    {
        gvFreeRenderData(outputData);
        gvFreeLayout(gvc, g);
        agclose(g);
        gvFreeContext(gvc);
        qCritical()<<"Error while rendering graph !";
        return false;
    }

    if(!output.open(QIODevice::WriteOnly))
    {
        gvFreeRenderData(outputData);
        gvFreeLayout(gvc, g);
        agclose(g);
        gvFreeContext(gvc);
        qCritical()<<"Unable to open output file :"<<output.errorString();
        return false;
    }
    QDataStream outputStream(&output);
    outputStream.writeBytes(outputData,length);
    output.close();

    gvFreeRenderData(outputData);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    return true;
}


QStringList yearsListToStringList(QList<unsigned int> years)
{
    //De-duplicate and sort
    QSet<unsigned int> yearsTemp;
    QList<unsigned int> years2;
    yearsTemp=years.toSet();
    years2=yearsTemp.toList();
    qSort(years2);

    QStringList yearsStringList;

    foreach(unsigned int annee,years2)
    {
        yearsStringList.append(QString::number(annee));
    }
    return yearsStringList;
}



qint64 doubleToFixedPoint(double num)
{
    return (qint64)((num+FIXEDPOINTCORRECTION)*FIXEDPOINTCOEFF);
}

double fixedPointToDouble(qint64 num)
{
    return (double)((double)num/(double)FIXEDPOINTCOEFF);
}
