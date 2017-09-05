/*
* ProtonClassicSuite
* 
* Copyright Thibault Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
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

#ifndef PCX_PREVISIONITEMTABLEMODEL_H
#define PCX_PREVISIONITEMTABLEMODEL_H

#include <QAbstractTableModel>
#include "pcx_previsionitem.h"

class PCx_PrevisionItemTableModel : public QAbstractTableModel
{

    Q_OBJECT

public:
    explicit PCx_PrevisionItemTableModel(PCx_PrevisionItem *previsionItem,QObject *parent=0);

    ~PCx_PrevisionItemTableModel();


    PCx_PrevisionItem *getPrevisionItem()const{return previsionItem;}
    void setPrevisionItem(PCx_PrevisionItem *previsionItem);


    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void resetModel();

private:
    PCx_PrevisionItem *previsionItem;

};

#endif // PCX_PREVISIONITEMTABLEMODEL_H
