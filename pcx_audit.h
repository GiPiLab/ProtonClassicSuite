#ifndef PCX_AUDIT_H
#define PCX_AUDIT_H

#include "pcx_tree.h"
#include "utils.h"
#include <QDateTime>


/**
 * @brief The PCx_Audit class represents an audit in the database
 *
 * An audit contains values about spendings and receipts for several years and each nodes of a tree
 * An audit is associated with a tree. It consists of four tables DF,RF,DI,RI which contains values
 * for each node of the tree and each years of the audit. An audit must be finished in order to be
 * usable in tables, graphics and queries forms.
 */
class PCx_Audit
{

public:

    /**
     * @brief The ORED enum describes the available data columns in each table of an audit
     */
    enum ORED
    {
        OUVERTS, ///<the amount of available money at the beginning of the year
        REALISES, ///<the amount of used money
        ENGAGES, ///<the amount of committed money
        DISPONIBLES ///<OUVERTS-(REALISES+ENGAGES)
    };

    /**
     * @brief The ListAuditsMode enum describes which audit will be shown in combobox
     */
    enum ListAuditsMode
    {
        FinishedAuditsOnly,
        UnFinishedAuditsOnly,
        AllAudits
    };

    /**
     * @brief PCx_Audit constructs an audit from an audit identifier in the database
     * @param auditId the identifier of the audit in the database
     * @param _noLoadAttachedTree if true does not create a tree (a TreeModel will be constructed in derived class)
     */
    explicit PCx_Audit(unsigned int auditId, bool _noLoadAttachedTree=false);
    virtual ~PCx_Audit();

    /**
     * @brief getAuditId gets the audit identifier
     * @return the identifier of the audit in the database
     */
    unsigned int getAuditId() const{return auditId;}

    /**
     * @brief getAttachedTree gets the attached tree
     * @return a pointer to the tree associated with this audit
     */
    virtual PCx_Tree *getAttachedTree() const{return attachedTree;}

    /**
     * @brief getAuditName gets the name of the audit
     * @return the name of the audit
     */
    QString getAuditName() const{return auditName;}

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
     * @brief isFinished is this audit finished ?
     * @return true if the audit is finished, ie can be used in computation, or false
     */
    bool isFinished() const{return finished;}

    /**
     * @brief isFinishedString gets a string to indicate if an audit is finished
     * @return a string representation of the boolean value returned by isFinished()
     */
    QString isFinishedString() const{return finishedString;}

    /**
     * @brief getYears gets the years that are covered by this audit
     * @return a list of years
     */
    QList<unsigned int> getYears() const{return years;}

    /**
     * @brief getYearsString gets a string of the years that are covered by this audit
     * @return a string in the FIRSTYEAR-LASTYEAR format
     */
    QString getYearsString() const{return yearsString;}

    /**
     * @brief getYearsStringList convenience method to gets the years as a list of strings
     * @return the years covered by this audit in a QStringList
     */
    QStringList getYearsStringList() const{return yearsStringList;}

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
     * @brief finishAudit marks an audit as finished.
     *
     * A finished audit cannot be modified. Only a finished audit can be used in later computations
     */
    void finishAudit();

    /**
     * @brief unFinishAudit marks an audit as un-finished, in order to modify its data
     */
    void unFinishAudit();

    /**
     * @brief setLeafValues sets the values of a leaf
     * @param leafId the identifier of the leaf
     * @param mode the mode to set
     * @param year the year to set
     * @param vals the values of the node for the mode and the year
     * @param fastMode if true, skip few checks (is the node a leaf, is year valid is the audit finished) to speedup
     * @return true on success, false if fastMode==false and checks failed
     */
    virtual bool setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED, double> vals, bool fastMode=false);

    /**
     * @brief getNodeValue gets the audit value of a node
     * @param nodeId the identifier of the node
     * @param mode the mode to read
     * @param ored the field to read
     * @param year the year to read
     * @return the node value in qint64 format (uses formatDouble or formatCurrency to display), or -MAX_NUM on NULL or unavailable node
     */
    qint64 getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int year) const;

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
    int duplicateAudit(const QString &newName,QList<unsigned int> years,
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
    QString generateHTMLAuditTitle() const;

    /**
     * @brief getHTMLAuditStatistics generates some useful statistics about the audit in HTML
     *
     * Mostly a wrapper to getNodesWithAllNullValues(), getNodesWithAllZeroValues() and getNodesWithNonNullValues()
     *
     * @return HTML with statistics
     */
    QString getHTMLAuditStatistics() const;

    /**
     * @brief OREDtoCompleteString converts an ORED item to its textual representation
     * @param ored the ORED item
     * @return the QString representation of the ORED
     */
    static QString OREDtoCompleteString(ORED ored);

    /**
     * @brief OREDtoTableString converts an ORED item to its database column name
     * @param ored the ORED item
     * @return the database column name
     */
    static QString OREDtoTableString(ORED ored);

    /**
     * @brief addNewAudit creates a new audit
     * @param name the audit name
     * @param years the years that will be covered by the audit
     * @param attachedTreeId the identifier of the tree
     * @return the identifier of the new audit, or 0 if the audit name exists, or the tree id does not exists (or is not finished)
     */
    static unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);

    /**
     * @brief deleteAudit removes an audit from the database
     * @param auditId the identifier of the audit to remove
     * @return true on success, false if the auditId does not exists
     */
    static bool deleteAudit(unsigned int auditId);

    /**
     * @brief auditNameExists checks if the audit name exists
     * @param auditName the name to check
     * @return true if the name exists, false otherwise
     */
    static bool auditNameExists(const QString &auditName);

    /**
     * @brief getListOfAudits gets a list of audits to fill a QComboBox
     * @param mode filter the audits not finished, finished or both
     * @return a list of auditID, audit description
     */
    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);

protected:

    unsigned int auditId;
    QString auditName;
    unsigned int attachedTreeId;

    QString attachedTreeName;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;
    QString finishedString;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    void updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);

    QHash<unsigned int,unsigned int> idToPid;
    QHash<unsigned int,QList<unsigned int> >idToChildren;
    QHash<unsigned int,QString> idToChildrenString;

private:
    /**
     * @brief attachedTree a pointer to the attached tree. Constructed here with the tree identifier
     */
    PCx_Tree *attachedTree;

    PCx_Audit(const PCx_Audit &c);
    PCx_Audit &operator=(const PCx_Audit &);


};

#endif // PCX_AUDIT_H
