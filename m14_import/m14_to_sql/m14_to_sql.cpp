#include "m14_to_sql.h"
#include <QCoreApplication>
#include <QTextStream>
#include <iostream>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

using namespace std;


#define ROOTNAME "M14_2014"

m14_to_sql::m14_to_sql(const QString &fileName,QObject *parent) :
    QObject(parent),file(fileName)
{
}

void m14_to_sql::run()
{
    QTextStream out(stdout,QIODevice::WriteOnly|QIODevice::Text);
    QFileInfo fi(file);
    if(!fi.exists()||!fi.isFile()||!fi.isReadable())
    {
        qCritical()<<"Fichier non accessible";
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QFile f(file);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qCritical()<<f.errorString();
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QTextStream stream(&f);
    stream.setCodec("UTF-8");

    QString line;

    QMap<unsigned int,unsigned int> m14IdToDbId,dbIdToDbPid;
    QMap<unsigned int, QString> dbIdToFullName;

    dbIdToFullName.insert(1,ROOTNAME);

    bool ok=false;
    int i=2;

    while(!(line=stream.readLine()).isNull())
    {
        QString num=line.section(" - ",0,0);
        QString label=line.section(" - ",1);
        unsigned int m14Id=num.toUInt(&ok);
        if(ok==false)
        {
            qCritical()<<"Error for "<<num<<label;
            f.close();
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        m14IdToDbId.insert(m14Id,i);
        dbIdToFullName.insert(i,line);
        i++;
    }
    f.close();

    foreach(unsigned int m14Id, m14IdToDbId.keys())
    {
        unsigned int m14Pid;
        if(m14Id<10)
        {
            //First level, PID=root
            dbIdToDbPid.insert(m14IdToDbId.value(m14Id),1);
        }
        else
        {
            m14Pid=m14Id/10;
            if(!m14IdToDbId.contains(m14Pid))
            {
                qCritical()<<"Missing PID for"<<m14Id;
                QCoreApplication::exit(EXIT_FAILURE);
                return;
            }
            else
            {
                dbIdToDbPid.insert(m14IdToDbId.value(m14Id),m14IdToDbId.value(m14Pid));
            }
        }
    }


    out<<"Type noeud\tNom noeud\tType pere\tNom pere\n";
    out<<"_\t"<<ROOTNAME<<"\n";
    foreach(unsigned int dbId,dbIdToDbPid.keys())
    {
        out<<"_\t"<<dbIdToFullName.value(dbId)<<"\t_\t"<<dbIdToFullName.value(dbIdToDbPid.value(dbId))<<endl;
    }


    QCoreApplication::quit();
}
