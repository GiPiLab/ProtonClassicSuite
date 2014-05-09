#include "pcx_tables.h"
#include <QtGui>
#include <QtSql>



PCx_tables::PCx_tables(unsigned int auditId, QObject *parent) :
    QObject(parent)
{
    currentAudit=new PCx_AuditModel(auditId,parent);
}

PCx_tables::~PCx_tables()
{
    delete currentAudit;
}

//Overview and percents
QString PCx_tables::getT1(unsigned int node, QSqlTableModel *model) const
{
    Q_ASSERT(node>0 && model!=NULL);

    QString tableName=model->tableName();
    QString output;
    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id").arg(tableName));
    q.bindValue(":id",node);
    q.exec();
    while(q.next())
    {
        output.append(QString("<span style='color:red;background-color:blue'>%1</span>").arg(QString::number(q.value("ouverts").toDouble())));
        output.append("   ");
    }
    return output;

}
