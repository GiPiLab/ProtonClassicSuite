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

#ifndef PCX_REPORTING_H
#define PCX_REPORTING_H

#include "pcx_audit.h"
#include "pcx_tree.h"
#include "utils.h"
#include <QComboBox>
#include <QDateTime>

/**
 * @brief The PCx_Reporting class represents a reporting in the database
 *
 * A reporting contains values about spendings and receipts for each nodes of a
 * tree, for several dates inside a year A reporting is associated with a tree.
 * It consists of four tables DF,RF,DI,RI which contains values for each node of
 * the tree.
 */
class PCx_Reporting {

public:
  /**
   * @brief The OREDPCR enum describes the available data columns in each table
   * of a reporting
   */
  enum class OREDPCR {
    OUVERTS = 0, ///< the amount of available money at the beginning of the year
    REALISES,    ///< the amount of used money
    ENGAGES,     ///< the amount of committed money
    DISPONIBLES, ///< OUVERTS-(REALISES+ENGAGES)
    BP,
    REPORTS,
    OCDM,
    VCDM,
    BUDGETVOTE,
    VIREMENTSINTERNES,
    RATTACHENMOINS1,
    NONELAST ///< Must be the last column
  };

  /**
   * @brief PCx_Reporting constructs a reporting from an identifier in the
   * database
   * @param reportingId the identifier of the reporting in the database
   * @param _noLoadAttachedTree if true does not create a tree (a TreeModel will
   * be constructed in derived class)
   */
  explicit PCx_Reporting(unsigned int reportingId, bool _noLoadAttachedTree = false);
  virtual ~PCx_Reporting();

  /**
   * @brief getReportingId gets the reporting identifier
   * @return the identifier of the reporting in the database
   */
  unsigned int getReportingId() const { return reportingId; }

  /**
   * @brief getAttachedTree gets the attached tree
   * @return a pointer to the tree associated with this reporting
   */
  virtual PCx_Tree *getAttachedTree() const { return attachedTree; }

  /**
   * @brief getReportingName gets the name of the reporting
   * @return the name of the reporting
   */
  QString getReportingName() const { return reportingName; }

  /**
   * @brief getAttachedTreeName convenience method equivalent to
   * PCx_Tree::getName()
   * @return the attached tree name
   */
  QString getAttachedTreeName() const { return attachedTreeName; }

  /**
   * @brief getAttachedTreeId convenience method equivalent to
   * PCx_Tree::getTreeId()
   * @return the attached tree identifier
   */
  unsigned int getAttachedTreeId() const { return attachedTreeId; }

  /**
   * @brief getAttachedTreeId static convenience method equivalent to
   * PCx_Tree::getTreeId()
   * @param reportingId the reporting identifier
   * @return the identifier of the tree attached to reportingId
   */
  static unsigned int getAttachedTreeId(unsigned int reportingId);

  /**
   * @brief getCreationTimeUTC gets the creation time/date of the reporting in
   * UTC
   * @return the creation time and date in UTC
   */
  QDateTime getCreationTimeUTC() const { return creationTimeUTC; }

  /**
   * @brief getCreationTimeLocal gets the creation time/date of the reporting in
   * locale DST
   * @return the creation time and date in locale DST
   */
  QDateTime getCreationTimeLocal() const { return creationTimeLocal; }

  /**
   * @brief setLeafValues sets the values of a leaf
   * @param leafId the identifier of the leaf
   * @param mode the mode to set
   * @param date the date to set
   * @param vals the values of the node for the mode and the year. Non specified
   * values are not updated
   * @param fastMode if true, skip few checks (is the node a leaf) to speedup
   * @return true on success, false if fastMode==false and checks failed
   */
  bool setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, QDate date, QMap<OREDPCR, double> vals,
                     bool fastMode = false);

  /**
   * @brief getNodeValue gets the reporting value of a node
   * @param nodeId the identifier of the node
   * @param mode the mode to read
   * @param ored the field to read
   * @param year the year to read
   * @return the node value in qint64 format (uses formatDouble or
   * formatCurrency to display), or -MAX_NUM on NULL or unavailable node
   */
  qint64 getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, PCx_Reporting::OREDPCR ored, QDate date) const;

  /**
   * @brief clearAllData erases all reporting data for a specific mode
   * @param mode the mode to clear
   */
  void clearAllData(MODES::DFRFDIRI mode);

  /**
   * @brief importDataFromXLSX imports reporting data from an XLSX file
   *
   * @param fileName the name of the file to import
   * @param mode the mode to import
   * @return true on success, false on error (file reading or format error) with
   * a warning displayed
   */
  bool importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode);

  /**
   * @brief exportLeavesDataXLSX exports leaf data in an XSLX file
   *
   * @param mode the mode to export
   * @param fileName the name of the file were data will be saved
   * @return true on success, false otherwise
   */
  bool exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString &fileName) const;

  /**
   * @brief getCSS gets the css to display an reporting in HTML
   * @return a string with CSS rules
   */
  static QString getCSS();

  /**
   * @brief generateHTMLHeader gets the header of an html document, with css and
   * reporting title
   * @return the HTML header including the opening "<body>"
   */
  QString generateHTMLHeader() const;

  /**
   * @brief generateHTMLReportingTitle gets the HTML title for the reporting
   * @return the title of the reporting, to be inserted in the "<body>"
   */
  QString generateHTMLReportingTitle() const;

  QSet<QDate> getDatesForNodeAndMode(unsigned int nodeId, MODES::DFRFDIRI mode) const;

  /**
   * @brief dateExistsForNodeAndMode checks if there are data associated with
   * this date for this node
   * @param date the date to check
   * @param nodeId the identifier of the node
   * @param mode the mode
   * @return true if there are data, false otherwise
   */
  bool dateExistsForNodeAndMode(QDate date, unsigned int nodeId, MODES::DFRFDIRI mode) const;

  bool dateExistsForNodeAndMode(time_t timeT, unsigned int nodeId, MODES::DFRFDIRI mode) const;

  /**
   * @brief addRandomDataForNext15 adds random data for a date 15 day after the
   * last reporting date
   * @param mode the mode to fill
   */
  void addRandomDataForNext15(MODES::DFRFDIRI mode);

  /**
   * @brief OREDPCRToComboBox fills a QComboBox with OREDPCR values
   * @param combo the QComboBox to fill
   */
  static void OREDPCRToComboBox(QComboBox *combo);

  /**
   * @brief OREDPCRtoCompleteString converts an OREDPCR item to its textual
   * representation
   * @param ored the OREDPCR item
   * @param capitalizeFirstLetter if true returns the string with the first
   * letter capitalized
   * @return the QString representation of the OREDPCR
   */
  static QString OREDPCRtoCompleteString(OREDPCR ored, bool capitalizeFirstLetter = false);

  /**
   * @brief OREDPCRtoTableString converts an OREDPCR item to its database column
   * name
   * @param ored the OREDPCR item
   * @return the database column name
   */
  static QString OREDPCRtoTableString(OREDPCR ored);

  /**
   * @brief OREDPCRFromTableString converts a column name to its corresponding
   * ORED item
   * @param ored the column name to convert
   * @return the ORED item, or ORED::OUVERTS in case of invalid name
   */
  static OREDPCR OREDPCRFromTableString(const QString &ored);

  /**
   * @brief addNewReporting creates a new reporting
   * @param name the reporting name
   * @param attachedTreeId the identifier of the tree
   * @return the identifier of the new reporting, or 0 if the reporting name
   * exists, or the tree id does not exists (or is not finished)
   */
  static unsigned int addNewReporting(const QString &name, unsigned int attachedTreeId);

  /**
   * @brief deleteReporting removes an reporting from the database
   * @param reportingId the identifier of the reporting to remove
   * @return true on success, false if the reportingId does not exists
   */
  static bool deleteReporting(unsigned int reportingId);

  /**
   * @brief reportingNameExists checks if the reporting name exists
   * @param reportingName the name to check
   * @return true if the name exists, false otherwise
   */
  static bool reportingNameExists(const QString &reportingName);

  /**
   * @brief getListOfReportings gets a list of reportings to fill a QComboBox
   * @return a list of reportingID, reporting description
   */
  static QList<QPair<unsigned int, QString>> getListOfReportings();

  bool exportLeavesSkeleton(const QString &fileName) const;

  /**
   * @brief getLastReportingDate gets the last reporting date for the specified
   * mode and node
   * @param mode the mode to check
   * @param node the node to check, by default the root
   * @return the last date
   */
  QDate getLastReportingDate(MODES::DFRFDIRI mode, unsigned int node = 1) const;

  /**
   * @brief deleteLastReportingDate deletes data for the last reporting
   * situation
   * @param mode the mode
   * @return true on success, false if no nodes deleted
   */
  bool deleteLastReportingDate(MODES::DFRFDIRI mode);

  /**
   * @brief addLastReportingDateToExistingAudit imports the last reporting date
   * in an audit The last reporting date will be added in the audit as values
   * for the year. For example if the last reporting date is 30/10/2012 a new
   * year will be added to the audit for 2012.
   * - The audit must be on the same tree than the reporting
   * @param audit the audit to extend
   * @return true on success, false on error with an error message box
   */
  bool addLastReportingDateToExistingAudit(PCx_Audit *audit) const;

  /**
   * @brief duplicateReporting duplicates a reporting
   * @param newName the new name of the reporting
   * @return the duplicated reporting id
   */
  unsigned int duplicateReporting(const QString &newName) const;

protected:
  unsigned int reportingId;
  QString reportingName;
  unsigned int attachedTreeId;

  QString attachedTreeName;

  QDateTime creationTimeUTC;
  QDateTime creationTimeLocal;

  void updateParent(const QString &tableName, QDate date, unsigned int nodeId);

  QHash<unsigned int, unsigned int> idToPid;
  QHash<unsigned int, QList<unsigned int>> idToChildren;
  QHash<unsigned int, QString> idToChildrenString;

private:
  /**
   * @brief attachedTree a pointer to the attached tree. Constructed here with
   * the tree identifier.
   *
   * MUST NOT BE USED DIRECTLY AS IT IS INVALID IN DERIVED CLASS, USE
   * getAttachedTree instead
   *
   */
  PCx_Tree *attachedTree;
  PCx_Reporting(const PCx_Reporting &c);
  PCx_Reporting &operator=(const PCx_Reporting &);
};

#endif // PCX_REPORTING_H
