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

#include "pcx_previsionitemtablemodel.h"

PCx_PrevisionItemTableModel::PCx_PrevisionItemTableModel(PCx_PrevisionItem *previsionItem, QObject *parent)
    : QAbstractTableModel(parent), previsionItem(previsionItem) {}

PCx_PrevisionItemTableModel::~PCx_PrevisionItemTableModel() = default;

void PCx_PrevisionItemTableModel::setPrevisionItem(PCx_PrevisionItem *previsionItem) {
  beginResetModel();
  this->previsionItem = previsionItem;
  endResetModel();
}

void PCx_PrevisionItemTableModel::resetModel() {
  beginResetModel();
  endResetModel();
}

int PCx_PrevisionItemTableModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2;
}
int PCx_PrevisionItemTableModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return previsionItem->getItemsToAdd().size() + previsionItem->getItemsToSubstract().size();
}

QVariant PCx_PrevisionItemTableModel::data(const QModelIndex &index, int role) const {
  // FIXME : Too much refresh
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    const PCx_PrevisionItemCriteria *trueItem = nullptr;
    if (index.row() < previsionItem->getItemsToAdd().count()) {
      trueItem = &previsionItem->getItemsToAdd().at(index.row());
    } else {
      trueItem = &previsionItem->getItemsToSubstract().at(index.row() - previsionItem->getItemsToAdd().count());
    }

    if (index.column() == 0) {
      return trueItem->getCriteriaLongDescription();
    }
    if (index.column() == 1) {
      return NUMBERSFORMAT::formatFixedPoint(trueItem->compute(previsionItem->getPrevision()->getAttachedAuditId(),
                                                               previsionItem->getMode(), previsionItem->getNodeId()));
    }
  } else if (role == Qt::BackgroundRole) {
    if (index.row() < previsionItem->getItemsToAdd().count()) {
      return QVariant(QBrush(QColor(Qt::green)));
    }
    return QVariant(QBrush(QColor(Qt::red)));

  } else if (role == Qt::TextAlignmentRole) {
    if (index.column() == 1) {
      return Qt::AlignCenter;
    }
  }
  return QVariant();
}

QVariant PCx_PrevisionItemTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      if (section == 0) {
        return QObject::tr("Critère");
      }
      if (section == 1) {
        return QObject::tr("Valeur");
      }
    }
  }
  return QVariant();
}
