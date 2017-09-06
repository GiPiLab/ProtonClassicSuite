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

#include "pcx_nodesimilaritytablemodel.h"
#include "utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QProgressDialog>


PCx_NodeSimilarityTableModel::PCx_NodeSimilarityTableModel(const QStringList strings, PCx_StringDistance::SIMILARITYMETRIC metric, QObject *parent) :
    QAbstractTableModel(parent),listOfStrings(strings),similarityMetric(metric)
{
    computeDistances();
}

int PCx_NodeSimilarityTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return distances.size();
}

int PCx_NodeSimilarityTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant PCx_NodeSimilarityTableModel::data(const QModelIndex &index, int role) const
{
    if(role==Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return distances.at(index.row()).getStr1();

        case 1:
            return distances.at(index.row()).getNormalizedSimilarity();

        case 2:
            return distances.at(index.row()).getStr2();
        }

    }
    return QVariant();

}

QVariant PCx_NodeSimilarityTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return tr("Noeud 1");
        case 1:
            return tr("Similarité");
        case 2:
            return tr("Noeud 2");
        }
    }
    return QVariant();
}

void PCx_NodeSimilarityTableModel::computeDistances()
{
    QHash<QString,bool> notCompute;
    QElapsedTimer timer;

    int maxVal=listOfStrings.size();

    QProgressDialog progress(QObject::tr("Calcul des similarités..."),0,0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);

    timer.start();
    int nbVals=0;

    foreach(const QString & str1, listOfStrings)
    {
        foreach(const QString &str2, listOfStrings)
        {
            if(str1==str2)
                continue;
            if(notCompute.contains(str1+str2))
            {
                continue;
            }
            else
            {
                PCx_StringDistance dist(str1,str2,similarityMetric);
                if(dist.getNormalizedSimilarity()>threshold)
                {
                    distances.append(dist);
                }
                notCompute.insert(str2+str1,true);
            }
        }
        progress.setValue(++nbVals);
    }
    //qDebug()<<distances.size()<<" distances computed in"<<timer.elapsed()<<"ms";
}




