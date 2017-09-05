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

#ifndef PCX_QUERYRANK_H
#define PCX_QUERYRANK_H

#include "pcx_query.h"

class PCx_QueryRank : public PCx_Query
{
public:

    static const unsigned int DEFAULTCOlOR=0xaaffbb;


    enum class GREATERSMALLER
    {
        SMALLER,
        GREATER
    };

    PCx_QueryRank(PCx_Audit *model,unsigned int queryId);

    PCx_QueryRank(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri,
                       GREATERSMALLER greaterOrSmaller, unsigned int number, unsigned int year1,unsigned int year2, const QString &name="");


    unsigned int getNumber()const{return number;}
    GREATERSMALLER getGreaterOrSmaller()const{return grSm;}

    void setNumber(unsigned int num){number=num;}
    void setGreaterOrSmaller(GREATERSMALLER grsm){grSm=grsm;}

    unsigned int save(const QString &name) const;
    QString exec(QXlsx::Document *xlsDoc=nullptr) const;
    bool load(unsigned int queryId);
    bool canSave(const QString &name) const;
    QString getDescription() const;

    static QString greaterSmallerToString(GREATERSMALLER grSm);

    static QString getCSS();
    static QColor getColor();


private:

    GREATERSMALLER grSm;
    unsigned int number;

    PCx_QueryRank(const PCx_QueryRank &c);
    PCx_QueryRank &operator=(const PCx_QueryRank &);

};

#endif // PCX_QUERYRANK_H
