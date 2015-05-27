#ifndef PCX_EDITABLEAUDITMODEL_H
#define PCX_EDITABLEAUDITMODEL_H

#include "pcx_auditwithtreemodel.h"
#include <QObject>


/**
 * @brief The PCx_EditableAuditModel class extends an audit model to provide editable QSqlTableModels for DF,RF,DI and RI tables
 */
class PCx_EditableAuditModel : public QObject, public PCx_AuditWithTreeModel
{
    Q_OBJECT
public:


    /**
     * @brief The COLINDEXES enum describes the structure of a DFRFDIRI table
     */
    enum class COLINDEXES
    {
        COL_ID=0,///<the identifier of the entry
        COL_IDNODE,///<the node identifier
        COL_ANNEE,///<the year
        COL_OUVERTS,///<ORED::OUVERTS
        COL_REALISES,///<ORED::REALISES
        COL_ENGAGES,///<ORED::ENGAGES
        COL_DISPONIBLES///<ORED::DISPONIBLES
    };

    /**
     * @brief PCx_EditableAuditModel constructs a PCx_AuditWithTreeModel and creates four QSqlTableModels
     * @param auditId the identifier of the audit
     * @param parent the QObject parent
     */
    explicit PCx_EditableAuditModel(unsigned int auditId,QObject *parent = 0);
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
    QSqlTableModel *getTableModelDF() const {return modelDF;}

    /**
     * @brief getTableModelDI gets the QSqlTableModel for DI table
     * @return the QSqlTableModel pointer for DI table
     */
    QSqlTableModel *getTableModelDI() const {return modelDI;}

    /**
     * @brief getTableModelRF gets the QSqlTableModel for RF table
     * @return the QSqlTableModel pointer for RF table
     */
    QSqlTableModel *getTableModelRF() const {return modelRF;}

    /**
     * @brief getTableModelRI gets the QSqlTableModel for RI table
     * @return the QSqlTableModel pointer for RI table
     */
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    /**
     * @brief setLeafValues sets the values of a leaf. Updates QSqlTableModels unless fastMode==true
     * @param leafId the identifier of the leaf
     * @param mode the mode to set
     * @param year the year to set
     * @param vals the values of the node for the mode and the year
     * @param fastMode if true, skip few checks (is the node a leaf, is year valid is the audit finished) to speedup
     * @return true on success, false if fastMode==false and checks failed
     */
    bool setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, unsigned int year, QMap<ORED, double> vals, bool fastMode=false);



    /**
     * @brief clearAllData erases all audit data for a specific mode, and refresh QSqlTableModel
     * @param mode the mode to clear
     */
    void clearAllData(MODES::DFRFDIRI mode);


signals:

public slots:


private:
    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;

    /**
     * @brief propagateToAncestors computes new values from leaf to root. Called when values change in the UI
     * @param node the model index of the leaf
     */
    void propagateToAncestors(const QModelIndex &node);

    PCx_EditableAuditModel(const PCx_EditableAuditModel &c);
    PCx_EditableAuditModel &operator=(const PCx_EditableAuditModel &);


private slots:
    /**
     * @brief onModelDataChanged is called when the user has changed the value of a leaf
     * @param topLeft is the model index of the cell
     * @param bottomRight is not used
     */
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

};

#endif // PCX_EDITABLEAUDITMODEL_H
