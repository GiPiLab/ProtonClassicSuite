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

#ifndef PCX_QUERY_H
#define PCX_QUERY_H

#include <climits>
#include "xlsxdocument.h"
#include "pcx_audit.h"

/**
 * @brief The PCx_Query class is the base class for predefined queries for an audit
 */
class PCx_Query
{
public:

    /**
     * @brief The QUERIESTYPES enum represents available queries
     */
    enum class QUERIESTYPES
    {
        VARIATION, ///< the query "give me nodes that increase or decrease
        MINMAX,
        RANK
    };

    static const unsigned int ALLTYPES=UINT_MAX;

    PCx_Query(PCx_Audit *model);
    PCx_Query(PCx_Audit *model,unsigned int typeId,PCx_Audit::ORED ored,MODES::DFRFDIRI dfrfdiri,
                                      unsigned int year1, unsigned int year2,const QString &name="");

    QPair<unsigned int,unsigned int> getYears() const {return QPair<unsigned int, unsigned int>(year1,year2);}
    virtual void setYears(unsigned int year1, unsigned int year2);

    void setORED(PCx_Audit::ORED ored){this->ored=ored;}
    PCx_Audit::ORED getORED() const{return ored;}

    void setDFRFDIRI(MODES::DFRFDIRI dfrfdiri){this->dfrfdiri=dfrfdiri;}
    MODES::DFRFDIRI getDFRFDIRI() const{return dfrfdiri;}

    unsigned int getTypeId() const{return typeId;}
    void setTypeId(unsigned int typeId){this->typeId=typeId;}

    void setModel(PCx_Audit *model){this->model=model;}
    const PCx_Audit *getModel() const{return model;}

    const QString &getName() const{return name;}
    void setName(const QString &name){this->name=name;}

    bool remove(unsigned int queryId);

    virtual unsigned int save(const QString &name) const =0;
    virtual bool canSave(const QString &name) const =0;
    virtual bool load(unsigned int queryId)=0;
    static bool deleteQuery(unsigned int auditId,unsigned int queryId);

    virtual QString getDescription() const =0;
    virtual QString exec(QXlsx::Document *xlsxDocument=nullptr) const =0;


    static void createTableQueries(unsigned int auditId);

    static QString getCSS();

    static QList<unsigned int>getListOfQueriesId(unsigned int auditId, QUERIESTYPES type);
    static QList<unsigned int>getListOfQueriesId(unsigned int auditId);


protected:
    PCx_Audit *model;
    //If typeId==ALLTYPES, do not filter nodes
    unsigned int typeId;
    PCx_Audit::ORED ored;
    MODES::DFRFDIRI dfrfdiri;
    unsigned int year1,year2;
    QString name;
    unsigned int queryId;

private:
    PCx_Query(const PCx_Query &c);
    PCx_Query &operator=(const PCx_Query &);

};

#endif // PCX_QUERY_H
