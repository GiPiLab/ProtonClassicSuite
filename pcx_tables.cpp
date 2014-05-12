#include "pcx_tables.h"
#include <QtGui>
#include <QtSql>
#include "utils.h"


namespace PCx_Tables
{
//Overview and percents
QString getT1(unsigned int node, const QSqlTableModel *model)
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
        output.append(QString("<span>%1</span>").arg(formatDouble(q.value("ouverts").toDouble())));

        output.append("   ");
    }
    return output;

}

QString getCSS()
{
    QString css;
    css="span {color:red;}";
    return css;
}


QString getTabRecap(unsigned int node, const QSqlTableModel *tableModel)
{
    return getT1(node,tableModel);
}

QString getTabEvolution(unsigned int node, const QSqlTableModel *tableModel)
{
   return "EVOL";
}



QString getTabEvolutionCumul(unsigned int node, const QSqlTableModel *tableModel)
{
   return "EVOLCUMUL";
}


QString getTabBase100(unsigned int node, const QSqlTableModel *tableModel)
{
    return "BASE100";
}


QString getTabJoursAct(unsigned int node, const QSqlTableModel *tableModel)
{
    return "JOURSACT";
}









}
