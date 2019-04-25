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

#include "auditdatadelegate.h"
#include "pcx_editableauditmodel.h"
#include "utils.h"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QPainter>

using namespace NUMBERSFORMAT;

auditDataDelegate::auditDataDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void auditDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  // Add a little horizontal padding
  QRect rect = option.rect;
  rect.setWidth(rect.width() - 5);
  rect.setLeft(rect.left() + 5);
  painter->save();

  if (!index.data().isNull() &&
      (index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_OUVERTS) ||
       index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_REALISES) ||
       index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ENGAGES) ||
       index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_DISPONIBLES))) {
    qint64 data = index.data().toLongLong();
    if (data < 0) {
      painter->setPen(QColor(255, 0, 0));
    }
    QString formattedNum = formatFixedPoint(data);
    painter->drawText(rect, formattedNum, QTextOption(Qt::AlignRight | Qt::AlignVCenter));
  } else if (index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE) ||
             index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ID)) {
    painter->drawText(rect, index.data().toString(), QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
  }

  painter->restore();
}

void auditDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  auto *edit = qobject_cast<QDoubleSpinBox *>(editor);
  // Convert fixed point arithmetics to double for displaying
  edit->setValue(fixedPointToDouble(index.data().toLongLong()));
}

void auditDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  QDoubleSpinBox *edit = qobject_cast<QDoubleSpinBox *>(editor);
  double value = edit->value();
  if (value > MAX_NUM) {
    value = MAX_NUM;
  }
  edit->setValue(doubleToFixedPoint(value));
  QStyledItemDelegate::setModelData(edit, model, index);
}

QWidget *auditDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
  Q_UNUSED(option);

  if (index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_DISPONIBLES) ||
      index.column() == static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE)) {
    return nullptr;
  }
  QDoubleSpinBox *spin = new QDoubleSpinBox(parent);
  spin->setDecimals(3);

  spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
  spin->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);

  spin->setRange(0.0, Q_INFINITY);
  return spin;
}
