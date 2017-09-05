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

#ifndef FORMEDITTREE_H
#define FORMEDITTREE_H

#include <QMdiArea>
#include "formdisplaytree.h"
#include "pcx_treemodel.h"

namespace Ui {
class FormEditTree;
}

class FormEditTree : public QWidget
{
    Q_OBJECT

public:
    explicit FormEditTree(QWidget *parent = 0);
    ~FormEditTree();

public slots:
    void updateRandomButtonVisibility();





private slots:

    void on_addTypeButton_clicked();

    void on_deleteTreeButton_clicked();

    void onTypesChanged();

    void on_newTreeButton_clicked();

    void on_deleteTypeButton_clicked();

    void on_addNodeButton_clicked();

    void on_modifyNodeButton_clicked();

    void on_deleteNodeButton_clicked();

    void on_finishTreeButton_clicked();

    void on_viewTreeButton_clicked();

    void on_comboBox_activated(int index);

    void on_duplicateTreeButton_clicked();

    void on_randomTreeButton_clicked();

    void on_pushButtonExpandAll_clicked();

    void on_pushButtonCollapseAll_clicked();

    void on_importTreeButton_clicked();

    void on_consistencyButton_clicked();

    void on_guessHierarchyButton_clicked();

signals:
    void listOfTreeChanged();

private:
    Ui::FormEditTree *ui;
    PCx_TreeModel *model;


    QSize sizeHint() const;
    void updateListOfTree();
    //Disable editing
    void setReadOnly(bool state);
};

#endif // FORMEDITTREE_H
