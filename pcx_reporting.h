#ifndef PCX_REPORTING_H
#define PCX_REPORTING_H


#include "pcx_tree.h"
#include "utils.h"
#include <QDateTime>


/**
 * @brief The PCx_Reporting class represents a reporting in the database
 *
 * A reporting contains values about spendings and receipts for each nodes of a tree, for several dates inside a year
 * A reporting is associated with a tree. It consists of four tables DF,RF,DI,RI which contains values
 * for each node of the tree.
 */
class PCx_Reporting
{

public:


    /**
     * @brief The OREDPCR enum describes the available data columns in each table of a reporting
     */
    enum OREDPCR
    {
        OUVERTS, ///<the amount of available money at the beginning of the year
        REALISES, ///<the amount of used money
        ENGAGES, ///<the amount of committed money
        DISPONIBLES, ///<OUVERTS-(REALISES+ENGAGES)
        BP,
        REPORTS,
        OCDM,
        VCDM,
        BUDGETVOTE,
        VIREMENTSINTERNES,
        RATTACHENMOINS1
    };


    /**
     * @brief PCx_Reporting constructs a reporting from an identifier in the database
     * @param auditId the identifier of the reporting in the database
     */
    explicit PCx_Reporting(unsigned int reportingId);
    virtual ~PCx_Reporting();

    /**
     * @brief getReportingId gets the reporting identifier
     * @return the identifier of the reporting in the database
     */
    unsigned int getReportingId() const{return reportingId;}

    /**
     * @brief getAttachedTree gets the attached tree
     * @return a pointer to the tree associated with this audit
     */
    virtual PCx_Tree *getAttachedTree() const{return attachedTree;}

    /**
     * @brief getReportingName gets the name of the reporting
     * @return the name of the reporting
     */
    QString getReportingName() const{return reportingName;}

    /**
     * @brief getAttachedTreeName convenience method equivalent to PCx_Tree::getName()
     * @return the attached tree name
     */
    QString getAttachedTreeName() const{return attachedTreeName;}

    /**
     * @brief getAttachedTreeId convenience method equivalent to PCx_Tree::getTreeId()
     * @return the attached tree identifier
     */
    unsigned int getAttachedTreeId() const{return attachedTreeId;}

    /**
     * @brief getAttachedTreeId static convenience method equivalent to PCx_Tree::getTreeId()
     * @param reportingId the reporting identifier
     * @return the identifier of the tree attached to reportingId
     */
    static unsigned int getAttachedTreeId(unsigned int reportingId);


    /**
     * @brief getCreationTimeUTC gets the creation time/date of the audit in UTC
     * @return the creation time and date in UTC
     */
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}

    /**
     * @brief getCreationTimeLocal gets the creation time/date of the audit in locale DST
     * @return the creation time and date in locale DST
     */
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}


    /**
     * @brief setLeafValues sets the values of a leaf
     * @param leafId the identifier of the leaf
     * @param mode the mode to set
     * @param date the date to set
     * @param vals the values of the node for the mode and the year
     * @param fastMode if true, skip few checks (is the node a leaf, is year valid is the audit finished) to speedup
     * @return true on success, false if fastMode==false and checks failed
     */
    virtual bool setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, QDate date, QHash<PCx_Reporting::OREDPCR, double> vals, bool fastMode=false);

    /**
     * @brief getNodeValue gets the audit value of a node
     * @param nodeId the identifier of the node
     * @param mode the mode to read
     * @param ored the field to read
     * @param year the year to read
     * @return the node value in qint64 format (uses formatDouble or formatCurrency to display), or -MAX_NUM on NULL or unavailable node
     */
    qint64 getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, PCx_Reporting::OREDPCR ored, unsigned int year) const;


    /**
     * @brief getNodeValues convenience method to gets all value fields of a node for a mode and a year
     * @param nodeId the node identifier
     * @param mode the mode to read
     * @param year the year to read
     * @return node values for each field in qint64 format
     */
    QHash<PCx_Reporting::OREDPCR,qint64> getNodeValues(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year) const;


    /**
     * @brief clearAllData erases all audit data for a specific mode
     * @param mode the mode to clear
     */
    virtual void clearAllData(MODES::DFRFDIRI mode);

    /**
     * @brief duplicateAudit duplicates an audit
     * @param newName the new name of the audit
     * @param years a list of years covered by the audit
     * @param copyDF if true copy nodes data for DF mode
     * @param copyRF if true copy nodes data for RF mode
     * @param copyDI if true copy nodes data for DI mode
     * @param copyRI if true copy nodes data for RI mode
     * @return -1 if the newName exists, the new audit identifier on success
     */
    int duplicateReporting(const QString &newName,
                                bool copyDF=true,bool copyRF=true, bool copyDI=true, bool copyRI=true) const;

    /**
     * @brief getNodesWithNonNullValues gets the nodes that contains at least one value
     * @param mode the mode to analyze
     * @param year the year to analyze
     * @return a list of node identifiers
     */
    QList<unsigned int> getNodesWithNonNullValues(MODES::DFRFDIRI mode, unsigned int year) const;

    /**
     * @brief getNodesWithAllNullValues gets the nodes without any value
     * @param mode the mode to analyze
     * @param year the year to analyze
     * @return a list of node identifiers
     */
    QList<unsigned int> getNodesWithAllNullValues(MODES::DFRFDIRI mode, unsigned int year) const;

    /**
     * @brief getNodesWithAllZeroValues gets the nodes that contains not null but all zero values
     * @param mode the mode to analyze
     * @param year the year to analyze
     * @return a list of node identifiers
     */
    QList<unsigned int> getNodesWithAllZeroValues(MODES::DFRFDIRI mode, unsigned int year) const;


    /**
     * @brief importDataFromXLSX imports audit data from an XLSX file
     *
     * The file must contains data in 6 columns, each row represents a leaf and a year:
     * COLUMN 1 is the type name of the leaf
     * COLUMN 2 is the name of the leaf
     * COLUMN 3 is the year
     * COLUMN 4 is the OUVERTS value, in double
     * COLUMN 5 is the REALISES value, in double
     * COLUMN 6 is the ENGAGES value, in double
     *
     * @param fileName the name of the file to import
     * @param mode the mode to import
     * @return true on success, false on error (file reading or format error) with a warning displayed
     */
    bool importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode);

    /**
     * @brief exportLeavesDataXLSX exports leaf data in an XSLX file
     *
     * The file will contains data in 6 columns, each row represents a leaf and a year:
     * COLUMN 1 is the type name of the leaf
     * COLUMN 2 is the name of the leaf
     * COLUMN 3 is the year
     * COLUMN 4 is the OUVERTS value, in double
     * COLUMN 5 is the REALISES value, in double
     * COLUMN 6 is the ENGAGES value, in double
     *
     * @param mode the mode to export
     * @param fileName the name of the file were data will be saved
     * @return true on success, false otherwise
     */
    bool exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString &fileName) const;

    /**
     * @brief getCSS gets the css to display an audit in HTML
     * @return a string with CSS rules
     */
    static QString getCSS();

    /**
     * @brief generateHTMLHeader gets the header of an html document, with css and audit title
     * @return the HTML header including the opening "<body>"
     */
    QString generateHTMLHeader() const;

    /**
     * @brief generateHTMLAuditTitle gets the HTML title for the audit
     * @return the title of the audit, to be inserted in the "<body>"
     */
    QString generateHTMLReportingTitle() const;

    /**
     * @brief getHTMLAuditStatistics generates some useful statistics about the audit in HTML
     *
     * Mostly a wrapper to getNodesWithAllNullValues(), getNodesWithAllZeroValues() and getNodesWithNonNullValues()
     *
     * @return HTML with statistics
     */
    QString getHTMLAuditStatistics() const;

    /**
     * @brief OREDPCRtoCompleteString converts an OREDPCR item to its textual representation
     * @param ored the OREDPCR item
     * @return the QString representation of the OREDPCR
     */
    static QString OREDPCRtoCompleteString(OREDPCR ored);

    /**
     * @brief OREDPCRtoTableString converts an OREDPCR item to its database column name
     * @param ored the OREDPCR item
     * @return the database column name
     */
    static QString OREDPCRtoTableString(OREDPCR ored);

    /**
     * @brief OREDPCRFromTableString converts a column name to its corresponding ORED item
     * @param ored the column name to convert
     * @return the ORED item, or ORED::OUVERTS in case of invalid name
     */
    static OREDPCR OREDPCRFromTableString(const QString &ored);

    /**
     * @brief addNewAudit creates a new audit
     * @param name the audit name
     * @param attachedTreeId the identifier of the tree
     * @return the identifier of the new audit, or 0 if the audit name exists, or the tree id does not exists (or is not finished)
     */
    static unsigned int addNewReporting(const QString &name, unsigned int attachedTreeId);

    /**
     * @brief deleteAudit removes an audit from the database
     * @param auditId the identifier of the audit to remove
     * @return true on success, false if the auditId does not exists
     */
    static bool deleteReporting(unsigned int reportingId);

    /**
     * @brief reportingNameExists checks if the audit name exists
     * @param reportingName the name to check
     * @return true if the name exists, false otherwise
     */
    static bool reportingNameExists(const QString &reportingName);

    /**
     * @brief getListOfReportings gets a list of audits to fill a QComboBox
     * @return a list of auditID, audit description
     */
    static QList<QPair<unsigned int, QString> > getListOfReportings();

    bool exportLeavesSkeleton(const QString &fileName) const;



protected:

    unsigned int reportingId;
    QString reportingName;
    unsigned int attachedTreeId;

    QString attachedTreeName;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    void updateParent(const QString &tableName, QDate date, unsigned int nodeId);

    /**
     * @brief attachedTree a pointer to the attached tree. Constructed here with the tree identifier
     */
    PCx_Tree *attachedTree;

    QHash<unsigned int,unsigned int> idToPid;
    QHash<unsigned int,QList<unsigned int> >idToChildren;
    QHash<unsigned int,QString> idToChildrenString;

private:

    PCx_Reporting(const PCx_Reporting &c);
    PCx_Reporting &operator=(const PCx_Reporting &);


};


#endif // PCX_REPORTING_H
