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

#ifndef TREEMAP_H
#define TREEMAP_H

#include <QString>
#include <QList>
#include <QRect>

class TreeMap
{
public:
    TreeMap(TreeMap *parent = nullptr, const QString &name = "", unsigned int id=0, int year=0, double value = 0.0);
    virtual ~TreeMap();

    TreeMap *insert(const QString &name, unsigned int id=0, int year=0, double value=0.0);
    TreeMap *findAt(const QPoint &pos) const;

    void clear(void);
    void sort(void);

    const TreeMap *getParent() const{return parent;}
    const QList<TreeMap *> getChildren() const{return children;}
    const QRect &getRect() const{return rect;}
    const QString &getName() const{return name;}
    int getId() const{return id;}
    int getYear() const{return year;}
    double getValue() const{return value;}

    void layout(const QRect &rect);

private:

    void layout(QList<TreeMap*> items, int start, int end, const QRect &bounds);
    void squarifyLayout(QList<TreeMap *>items,const QRect &bounds);
    double aspect(double _big, double _small, double a, double b) const;
    double normAspect(double _big, double _small, double a, double b) const;
    void slicelayout(QList<TreeMap*> items, int start, int end, const QRect &bounds);

    static bool lessThan(const TreeMap *a,const TreeMap *b);

    TreeMap *parent;
    QString name;
    int id;
    int year;
    double value;
    QList<TreeMap *>children;
    QRect rect;

};

#endif // TREEMAP_H
