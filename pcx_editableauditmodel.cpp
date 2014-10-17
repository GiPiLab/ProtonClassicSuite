#include "pcx_editableauditmodel.h"
#include <QDebug>

PCx_EditableAuditModel::PCx_EditableAuditModel(unsigned int auditId,QObject *parent) :
    QObject(parent),PCx_AuditWithTreeModel(auditId)
{
    modelDF=nullptr;
    modelDI=nullptr;
    modelRI=nullptr;
    modelRF=nullptr;
    modelDF=new QSqlTableModel();
    modelDF->setTable(QString("audit_DF_%1").arg(auditId));
    modelDF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelDF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    modelDF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    modelDF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
    modelDF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
    modelDF->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelDF->select();

    modelDI=new QSqlTableModel();
    modelDI->setTable(QString("audit_DI_%1").arg(auditId));
    modelDI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelDI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    modelDI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    modelDI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
    modelDI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
    modelDI->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelDI->select();

    modelRI=new QSqlTableModel();
    modelRI->setTable(QString("audit_RI_%1").arg(auditId));
    modelRI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelRI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    modelRI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    modelRI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
    modelRI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
    modelRI->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelRI->select();

    modelRF=new QSqlTableModel();
    modelRF->setTable(QString("audit_RF_%1").arg(auditId));
    modelRF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelRF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    modelRF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    modelRF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
    modelRF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));
    modelRF->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelRF->select();

    connect(modelDF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelRF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelDI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelRI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
}

PCx_EditableAuditModel::~PCx_EditableAuditModel()
{
    if(modelDF!=nullptr)
    {
        modelDF->clear();
        delete modelDF;
    }
    if(modelRF!=nullptr)
    {
        modelRF->clear();
        delete modelRF;
    }
    if(modelDI!=nullptr)
    {
        modelDI->clear();
        delete modelDI;
    }
    if(modelRI!=nullptr)
    {
        modelRI->clear();
        delete modelRI;
    }
}


//Warning, be called twice (see isDirty)
void PCx_EditableAuditModel::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(bottomRight);
    QSqlTableModel *model=(QSqlTableModel *)topLeft.model();

    qDebug()<<"Audit Data changed for model "<<model->tableName()<<": topleft column = "<<topLeft.column()<<" topleft row = "<<topLeft.row()<<"bottomRight column = "<<bottomRight.column()<<" bottomRight row = "<<bottomRight.row();
    qDebug()<<"Model dirty : "<<model->isDirty();
    int row=topLeft.row();

    QVariant vOuverts=model->index(row,COL_OUVERTS).data();
    QVariant vRealises=model->index(row,COL_REALISES).data();
    QVariant vEngages=model->index(row,COL_ENGAGES).data();

    qDebug()<<"VOUVERTS = "<<vOuverts;

    qint64 ouverts=vOuverts.toLongLong();
    qint64 realises=vRealises.toLongLong();
    qint64 engages=vEngages.toLongLong();

    if(!vRealises.isNull() && !vOuverts.isNull() && !vEngages.isNull())
    {
        QVariant disponibles=ouverts-(realises+engages);
        QModelIndex indexDispo=model->index(row,COL_DISPONIBLES);
        model->setData(indexDispo,disponibles);
    }

    //Only propagate after the database has been updated (called for refreshing the view, see https://bugreports.qt-project.org/browse/QTBUG-30672
    if(!model->isDirty())
    {
        propagateToAncestors(topLeft);
    }

}

bool PCx_EditableAuditModel::propagateToAncestors(const QModelIndex &node)
{
    QSqlTableModel *model=(QSqlTableModel *)node.model();
    int row=node.row();
    unsigned int nodeId=model->index(row,COL_IDNODE).data().toUInt();
    unsigned int annee=model->index(row,COL_ANNEE).data().toUInt();
    QString tableName=model->tableName();
    qDebug()<<"Propagate from node "<<nodeId<<" in "<<annee<<" on "<<tableName;

    QSqlDatabase::database().transaction();
    if(updateParent(tableName,annee,nodeId))
    {
        QSqlDatabase::database().commit();
        return true;
    }
    else
    {
        QSqlDatabase::database().rollback();
        qWarning()<<"ERROR DURING PROPAGATING VALUES TO ROOTS, CANCELLING";
        return false;
    }
}

QSqlTableModel *PCx_EditableAuditModel::getTableModel(const QString &mode) const
{
    if(0==mode.compare("DF",Qt::CaseInsensitive))
    {
        return modelDF;
    }
    if(0==mode.compare("RF",Qt::CaseInsensitive))
    {
        return modelRF;
    }
    if(0==mode.compare("DI",Qt::CaseInsensitive))
    {
        return modelDI;
    }
    if(0==mode.compare("RI",Qt::CaseInsensitive))
    {
        return modelRI;
    }
    return nullptr;
}

QSqlTableModel *PCx_EditableAuditModel::getTableModel(PCx_Audit::DFRFDIRI mode) const
{
    switch(mode)
    {
    case PCx_Audit::DFRFDIRI::DF:return modelDF;
    case PCx_Audit::DFRFDIRI::RF:return modelRF;
    case PCx_Audit::DFRFDIRI::DI:return modelDI;
    case PCx_Audit::DFRFDIRI::RI:return modelRI;
    case PCx_Audit::DFRFDIRI::GLOBAL:return nullptr;
    }
    return nullptr;
}

bool PCx_EditableAuditModel::setLeafValues(unsigned int leafId, PCx_Audit::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED, double> vals, bool fastMode)
{
    if(PCx_Audit::setLeafValues(leafId,mode,year,vals,fastMode)==false)
        return false;

    //If we are in fast mode, do not refresh the table model after each iteration
    if(!fastMode)
    {
        QSqlTableModel *tblModel=getTableModel(mode);
        if(tblModel!=nullptr)
            tblModel->select();
    }
    return true;
}

bool PCx_EditableAuditModel::clearAllData(PCx_Audit::DFRFDIRI mode)
{
    PCx_Audit::clearAllData(mode);
    QSqlTableModel *tblModel=getTableModel(mode);
    if(tblModel!=nullptr)
        tblModel->select();

    return true;
}

