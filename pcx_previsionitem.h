/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le
 * Développement Local (2006--)
 *
 * labo@gipilab.org
 *
 * Ce logiciel est un programme informatique servant à cerner l'ensemble des
 * données budgétaires de la collectivité territoriale (audit, reporting
 * infra-annuel, prévision des dépenses à venir)
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

#ifndef PCX_PREVISIONITEM_H
#define PCX_PREVISIONITEM_H

#include "pcx_prevision.h"
#include <QTextDocument>

class PCx_PrevisionItem {
public:
  explicit PCx_PrevisionItem(PCx_Prevision *prevision, MODES::DFRFDIRI mode, unsigned int nodeId, int year);
  PCx_Prevision *getPrevision() const { return prevision; }
  MODES::DFRFDIRI getMode() const { return mode; }
  unsigned int getNodeId() const { return nodeId; }

  void setNodeId(unsigned int nodeId) { this->nodeId = nodeId; }
  void setMode(MODES::DFRFDIRI mode) { this->mode = mode; }
  void setYear(int year) { this->year = year; }

  int getYear() const { return year; }
  // TODO label not used yet
  QString getLabel() const { return label; }
  QString getLongDescription() const;
  QList<PCx_PrevisionItemCriteria> getItemsToAdd() const { return itemsToAdd; }
  QList<PCx_PrevisionItemCriteria> getItemsToSubstract() const { return itemsToSubstract; }

  void insertCriteriaToAdd(const PCx_PrevisionItemCriteria &criteria, bool compute = true);
  void insertCriteriaToSub(const PCx_PrevisionItemCriteria &criteria, bool compute = true);
  bool deleteCriteria(QModelIndexList selectedIndexes, bool compute = true);
  void deleteAllCriteria();

  void loadFromDb();
  void saveDataToDb();

  void dispatchCriteriaItemsToChildrenLeaves();
  bool dispatchComputedValueToChildrenLeaves(PCx_Audit::ORED oredReference = PCx_Audit::ORED::REALISES);

  QString getPrevisionItemAsHTML() const;
  QString getPrevisionItemsOfDescendantsAsHTML() const;

  qint64 getSummedPrevisionItemValue() const;
  qint64 getPrevisionItemValue() const;

  ~PCx_PrevisionItem();

  QString displayPrevisionItemReportInQTextDocument(QTextDocument *document, unsigned int referenceNode = 1) const;

  bool savePrevisionItemReport(const QString &fileName, bool showDescendants, unsigned int referenceNode = 1) const;

private:
  PCx_Prevision *prevision;
  MODES::DFRFDIRI mode;
  unsigned int nodeId;
  int year;
  QString label;
  qint64 computedValue;

  QList<PCx_PrevisionItemCriteria> itemsToAdd, itemsToSubstract;
};

#endif // PCX_PREVISIONITEM_H
