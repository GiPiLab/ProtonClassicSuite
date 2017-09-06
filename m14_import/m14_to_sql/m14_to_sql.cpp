/*
* ProtonClassicSuite
* 
* Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/

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
