#include "pcx_editableauditmodel.h"
#include <QDebug>

PCx_EditableAuditModel::PCx_EditableAuditModel(unsigned int auditId,QObject *parent) :
    QObject(parent),PCx_Audit(auditId)
{
    modelDF=NULL;
    modelDI=NULL;
    modelRI=NULL;
    modelRF=NULL;
    modelDF=new QSqlTableModel();
    modelDF->setTable(QString("audit_DF_%1").arg(auditId));
    modelDF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelDF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
    modelDF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
    modelDF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
    modelDF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
    modelDF->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelDF->select();

    modelDI=new QSqlTableModel();
    modelDI->setTable(QString("audit_DI_%1").arg(auditId));
    modelDI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelDI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
    modelDI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
    modelDI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
    modelDI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
    modelDI->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelDI->select();

    modelRI=new QSqlTableModel();
    modelRI->setTable(QString("audit_RI_%1").arg(auditId));
    modelRI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelRI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
    modelRI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
    modelRI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
    modelRI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
    modelRI->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelRI->select();

    modelRF=new QSqlTableModel();
    modelRF->setTable(QString("audit_RF_%1").arg(auditId));
    modelRF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
    modelRF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
    modelRF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
    modelRF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
    modelRF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
    modelRF->setEditStrategy(QSqlTableModel::OnFieldChange);
    modelRF->select();

    connect(modelDF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelRF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelDI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(modelRI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
}

PCx_EditableAuditModel::~PCx_EditableAuditModel()
{
    if(modelDF!=NULL)
    {
        modelDF->clear();
        delete modelDF;
    }
    if(modelRF!=NULL)
    {
        modelRF->clear();
        delete modelRF;
    }
    if(modelDI!=NULL)
    {
        modelDI->clear();
        delete modelDI;
    }
    if(modelRI!=NULL)
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
    return NULL;
}

QSqlTableModel *PCx_EditableAuditModel::getTableModel(DFRFDIRI mode) const
{
    switch(mode)
    {
    case DF:return modelDF;
    case RF:return modelRF;
    case DI:return modelDI;
    case RI:return modelRI;
    case GLOBAL:return NULL;
    }
    return NULL;
}

bool PCx_EditableAuditModel::setLeafValues(unsigned int leafId, PCx_Audit::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED, double> vals)
{
    if(PCx_Audit::setLeafValues(leafId,mode,year,vals)==false)
        return false;
    QSqlTableModel *tblModel=getTableModel(mode);
    if(tblModel!=NULL)
        tblModel->select();
    return true;
}

bool PCx_EditableAuditModel::clearAllData(DFRFDIRI mode)
{
    PCx_Audit::clearAllData(mode);
    QSqlTableModel *tblModel=getTableModel(mode);
    if(tblModel!=NULL)
        tblModel->select();

    return true;
}

