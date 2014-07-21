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
        out.prepend(locale.toString(((double)num/FIXEDPOINTCOEFF),'f',decimals));
    else
        out.prepend(locale.toString(((double)num/FIXEDPOINTCOEFF),'f',currentNumDecimals));
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


bool dotToPdf(const QString &inputFileName, const QString &outputFileName)
{
    Q_ASSERT(!inputFileName.isEmpty() && !outputFileName.isEmpty());

    //Pass through a QFile to avoid encoding problems in filenames
    QFile input(inputFileName);
    QFile output(outputFileName);

    if(!input.open(QIODevice::ReadOnly))
    {
        qCritical()<<"Unable to open input dot file :"<<input.errorString();
        return false;
    }

    if(!output.open(QIODevice::WriteOnly))
    {
        input.close();
        qCritical()<<"Unable to open output file :"<<output.errorString();
        return false;
    }

    GVC_t *gvc;
    graph_t *g;
    FILE *fileInput,*fileOutput;

    fileInput=fdopen(input.handle(),"r");
    if(fileInput==NULL)
    {
        input.close();
        output.close();
        qCritical()<<"Unable to open input dot file !";
        return false;
    }

    fileOutput=fdopen(output.handle(),"w");
    if(fileOutput==NULL)
    {
        input.close();
        output.close();
        fclose(fileInput);
        qCritical()<<"Unable to open output file !";
        return false;
    }

    gvc = gvContext();
    g = agread(fileInput, 0);

    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "pdf", fileOutput);

    gvFreeLayout(gvc, g);

    agclose(g);

    fclose(fileInput);
    fclose(fileOutput);
    input.close();
    output.close();

    gvFreeContext(gvc);
    return true;
}
