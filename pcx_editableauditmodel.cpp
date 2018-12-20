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

#include "pcx_editableauditmodel.h"
#include <QDebug>

PCx_EditableAuditModel::PCx_EditableAuditModel(unsigned int auditId, QObject *parent)
    : QObject(parent), PCx_AuditWithTreeModel(auditId) {
  modelDF = nullptr;
  modelDI = nullptr;
  modelRI = nullptr;
  modelRF = nullptr;
  modelDF = new QSqlTableModel();
  modelDF->setTable(QString("audit_DF_%1").arg(auditId));
  modelDF->setHeaderData(static_cast<int>(COLINDEXES::COL_ANNEE), Qt::Horizontal, "");
  modelDF->setHeaderData(static_cast<int>(COLINDEXES::COL_OUVERTS), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
  modelDF->setHeaderData(static_cast<int>(COLINDEXES::COL_REALISES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::REALISES));
  modelDF->setHeaderData(static_cast<int>(COLINDEXES::COL_ENGAGES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
  modelDF->setHeaderData(static_cast<int>(COLINDEXES::COL_DISPONIBLES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
  modelDF->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelDF->select();

  modelDI = new QSqlTableModel();
  modelDI->setTable(QString("audit_DI_%1").arg(auditId));
  modelDI->setHeaderData(static_cast<int>(COLINDEXES::COL_ANNEE), Qt::Horizontal, "");
  modelDI->setHeaderData(static_cast<int>(COLINDEXES::COL_OUVERTS), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
  modelDI->setHeaderData(static_cast<int>(COLINDEXES::COL_REALISES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::REALISES));
  modelDI->setHeaderData(static_cast<int>(COLINDEXES::COL_ENGAGES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
  modelDI->setHeaderData(static_cast<int>(COLINDEXES::COL_DISPONIBLES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
  modelDI->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelDI->select();

  modelRI = new QSqlTableModel();
  modelRI->setTable(QString("audit_RI_%1").arg(auditId));
  modelRI->setHeaderData(static_cast<int>(COLINDEXES::COL_ANNEE), Qt::Horizontal, "");
  modelRI->setHeaderData(static_cast<int>(COLINDEXES::COL_OUVERTS), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
  modelRI->setHeaderData(static_cast<int>(COLINDEXES::COL_REALISES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::REALISES));
  modelRI->setHeaderData(static_cast<int>(COLINDEXES::COL_ENGAGES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
  modelRI->setHeaderData(static_cast<int>(COLINDEXES::COL_DISPONIBLES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
  modelRI->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelRI->select();

  modelRF = new QSqlTableModel();
  modelRF->setTable(QString("audit_RF_%1").arg(auditId));
  modelRF->setHeaderData(static_cast<int>(COLINDEXES::COL_ANNEE), Qt::Horizontal, "");
  modelRF->setHeaderData(static_cast<int>(COLINDEXES::COL_OUVERTS), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
  modelRF->setHeaderData(static_cast<int>(COLINDEXES::COL_REALISES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::REALISES));
  modelRF->setHeaderData(static_cast<int>(COLINDEXES::COL_ENGAGES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
  modelRF->setHeaderData(static_cast<int>(COLINDEXES::COL_DISPONIBLES), Qt::Horizontal,
                         OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
  modelRF->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelRF->select();

  connect(modelDF, &QAbstractItemModel::dataChanged, this, &PCx_EditableAuditModel::onModelDataChanged);
  connect(modelRF, &QAbstractItemModel::dataChanged, this, &PCx_EditableAuditModel::onModelDataChanged);
  connect(modelDI, &QAbstractItemModel::dataChanged, this, &PCx_EditableAuditModel::onModelDataChanged);
  connect(modelRI, &QAbstractItemModel::dataChanged, this, &PCx_EditableAuditModel::onModelDataChanged);
}

PCx_EditableAuditModel::~PCx_EditableAuditModel() {
  if (modelDF != nullptr) {
    modelDF->clear();
    delete modelDF;
  }
  if (modelRF != nullptr) {
    modelRF->clear();
    delete modelRF;
  }
  if (modelDI != nullptr) {
    modelDI->clear();
    delete modelDI;
  }
  if (modelRI != nullptr) {
    modelRI->clear();
    delete modelRI;
  }
}

// Warning, be called twice (see isDirty)
void PCx_EditableAuditModel::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
  Q_UNUSED(bottomRight);
  auto *model = const_cast<QSqlTableModel *>(dynamic_cast<const QSqlTableModel *>(topLeft.model()));

  // qDebug()<<"Audit Data changed for model "<<model->tableName()<<": topleft
  // column = "<<topLeft.column()<<" topleft row =
  // "<<topLeft.row()<<"bottomRight column = "<<bottomRight.column()<<"
  // bottomRight row = "<<bottomRight.row(); qDebug()<<"Model dirty :
  // "<<model->isDirty();
  int row = topLeft.row();

  QVariant vOuverts = model->index(row, static_cast<int>(COLINDEXES::COL_OUVERTS)).data();
  QVariant vRealises = model->index(row, static_cast<int>(COLINDEXES::COL_REALISES)).data();
  QVariant vEngages = model->index(row, static_cast<int>(COLINDEXES::COL_ENGAGES)).data();

  // qDebug()<<"VOUVERTS = "<<vOuverts;

  qint64 ouverts = vOuverts.toLongLong();
  qint64 realises = vRealises.toLongLong();
  qint64 engages = vEngages.toLongLong();

  if (!vRealises.isNull() && !vOuverts.isNull() && !vEngages.isNull()) {
    QVariant disponibles = ouverts - (realises + engages);
    QModelIndex indexDispo = model->index(row, static_cast<int>(COLINDEXES::COL_DISPONIBLES));
    model->setData(indexDispo, disponibles);
  }

  // Only propagate after the database has been updated (called for refreshing
  // the view, see https://bugreports.qt-project.org/browse/QTBUG-30672
  if (!model->isDirty()) {
    propagateToAncestors(topLeft);
  }
}

void PCx_EditableAuditModel::propagateToAncestors(const QModelIndex &node) {
  auto *model = const_cast<QSqlTableModel *>(dynamic_cast<const QSqlTableModel *>(node.model()));
  int row = node.row();
  unsigned int nodeId = model->index(row, static_cast<int>(COLINDEXES::COL_IDNODE)).data().toUInt();
  int annee = model->index(row, static_cast<int>(COLINDEXES::COL_ANNEE)).data().toInt();
  QString tableName = model->tableName();
  // qDebug()<<"Propagate from node "<<nodeId<<" in "<<annee<<" on "<<tableName;

  QSqlDatabase::database().transaction();
  updateParent(tableName, annee, nodeId);
  // rollback in case of failure is done in updateParent=>die
  QSqlDatabase::database().commit();
}

QSqlTableModel *PCx_EditableAuditModel::getTableModel(const QString &mode) const {
  if (0 == mode.compare("DF", Qt::CaseInsensitive)) {
    return modelDF;
  }
  if (0 == mode.compare("RF", Qt::CaseInsensitive)) {
    return modelRF;
  }
  if (0 == mode.compare("DI", Qt::CaseInsensitive)) {
    return modelDI;
  }
  if (0 == mode.compare("RI", Qt::CaseInsensitive)) {
    return modelRI;
  }
  return nullptr;
}

QSqlTableModel *PCx_EditableAuditModel::getTableModel(MODES::DFRFDIRI mode) const {
  switch (mode) {
  case MODES::DFRFDIRI::DF:
    return modelDF;
  case MODES::DFRFDIRI::RF:
    return modelRF;
  case MODES::DFRFDIRI::DI:
    return modelDI;
  case MODES::DFRFDIRI::RI:
    return modelRI;
  case MODES::DFRFDIRI::GLOBAL:
    return nullptr;
  }
  return nullptr;
}

bool PCx_EditableAuditModel::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, int year,
                                           const QMap<PCx_Audit::ORED, double> &vals, bool fastMode) {
  if (PCx_Audit::setLeafValues(leafId, mode, year, vals, fastMode) == false) {
    return false;
  }

  // If we are in fast mode, do not refresh the table model after each iteration
  if (!fastMode) {
    QSqlTableModel *tblModel = getTableModel(mode);
    if (tblModel != nullptr) {
      tblModel->select();
    }
  }
  return true;
}

void PCx_EditableAuditModel::clearAllData(MODES::DFRFDIRI mode) {
  PCx_Audit::clearAllData(mode);
  QSqlTableModel *tblModel = getTableModel(mode);
  if (tblModel != nullptr) {
    tblModel->select();
  }
}
