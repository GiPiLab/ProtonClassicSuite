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

#ifndef PCX_EDITABLEAUDITMODEL_H
#define PCX_EDITABLEAUDITMODEL_H

#include "pcx_auditwithtreemodel.h"
#include <QObject>

/**
 * @brief The PCx_EditableAuditModel class extends an audit model to provide
 * editable QSqlTableModels for DF,RF,DI and RI tables
 */
class PCx_EditableAuditModel : public QObject, public PCx_AuditWithTreeModel {
  Q_OBJECT
public:
  /**
   * @brief The COLINDEXES enum describes the structure of a DFRFDIRI table
   */
  enum class COLINDEXES {
    COL_ID = 0,     ///< the identifier of the entry
    COL_IDNODE,     ///< the node identifier
    COL_ANNEE,      ///< the year
    COL_OUVERTS,    ///< ORED::OUVERTS
    COL_REALISES,   ///< ORED::REALISES
    COL_ENGAGES,    ///< ORED::ENGAGES
    COL_DISPONIBLES ///< ORED::DISPONIBLES
  };

  /**
   * @brief PCx_EditableAuditModel constructs a PCx_AuditWithTreeModel and
   * creates four QSqlTableModels
   * @param auditId the identifier of the audit
   * @param parent the QObject parent
   */
  explicit PCx_EditableAuditModel(unsigned int auditId, QObject *parent = nullptr);
  virtual ~PCx_EditableAuditModel();

  /**
   * @brief getTableModel gets the QSQLTableModel for the "mode" table
   * @param mode a string representation of the mode, for example "DF"
   * @return the table model, or nullptr if the string is not matched
   */
  QSqlTableModel *getTableModel(const QString &mode) const;

  /**
   * @brief getTableModel gets the QSQLTableModel for the DFRFDIRI table
   * @param mode the DFRFDIRI mode
   * @return the table model, or nullptr if mode==DFRFDIRI::GLOBAL
   */
  QSqlTableModel *getTableModel(MODES::DFRFDIRI mode) const;

  /**
   * @brief getTableModelDF gets the QSqlTableModel for DF table
   * @return the QSqlTableModel pointer for DF table
   */
  QSqlTableModel *getTableModelDF() const { return modelDF; }

  /**
   * @brief getTableModelDI gets the QSqlTableModel for DI table
   * @return the QSqlTableModel pointer for DI table
   */
  QSqlTableModel *getTableModelDI() const { return modelDI; }

  /**
   * @brief getTableModelRF gets the QSqlTableModel for RF table
   * @return the QSqlTableModel pointer for RF table
   */
  QSqlTableModel *getTableModelRF() const { return modelRF; }

  /**
   * @brief getTableModelRI gets the QSqlTableModel for RI table
   * @return the QSqlTableModel pointer for RI table
   */
  QSqlTableModel *getTableModelRI() const { return modelRI; }

  /**
   * @brief setLeafValues sets the values of a leaf. Updates QSqlTableModels
   * unless fastMode==true
   * @param leafId the identifier of the leaf
   * @param mode the mode to set
   * @param year the year to set
   * @param vals the values of the node for the mode and the year
   * @param fastMode if true, skip few checks (is the node a leaf, is year valid
   * is the audit finished) to speedup
   * @return true on success, false if fastMode==false and checks failed
   */
  bool setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, int year, QMap<ORED, double> vals,
                     bool fastMode = false);

  /**
   * @brief clearAllData erases all audit data for a specific mode, and refresh
   * QSqlTableModel
   * @param mode the mode to clear
   */
  void clearAllData(MODES::DFRFDIRI mode);

signals:

public slots:

private:
  QSqlTableModel *modelDF, *modelRF, *modelDI, *modelRI;

  /**
   * @brief propagateToAncestors computes new values from leaf to root. Called
   * when values change in the UI
   * @param node the model index of the leaf
   */
  void propagateToAncestors(const QModelIndex &node);

  PCx_EditableAuditModel(const PCx_EditableAuditModel &c);
  PCx_EditableAuditModel &operator=(const PCx_EditableAuditModel &);

private slots:
  /**
   * @brief onModelDataChanged is called when the user has changed the value of
   * a leaf
   * @param topLeft is the model index of the cell
   * @param bottomRight is not used
   */
  void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // PCX_EDITABLEAUDITMODEL_H
