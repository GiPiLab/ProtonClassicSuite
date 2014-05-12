#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include <QObject>
#include <QSqlTableModel>

namespace PCx_Tables
{
QString getT1(unsigned int node, const QSqlTableModel *model);
QString getT2(unsigned int node, const QSqlTableModel *model);
QString getT3(unsigned int node, const QSqlTableModel *model);
QString getT4(unsigned int node, const QSqlTableModel *model);
QString getT5(unsigned int node, const QSqlTableModel *model);
QString getT6(unsigned int node, const QSqlTableModel *model);
QString getT7(unsigned int node, const QSqlTableModel *model);
QString getT8(unsigned int node, const QSqlTableModel *model);
QString getT9(unsigned int node, const QSqlTableModel *model);
QString getT10(unsigned int node, const QSqlTableModel *model);
QString getT11(unsigned int node, const QSqlTableModel *model);
QString getCSS();


QString getTabRecap(unsigned int node, const QSqlTableModel *tableModel);
QString getTabEvolution(unsigned int node, const QSqlTableModel *tableModel);
QString getTabEvolutionCumul(unsigned int node, const QSqlTableModel *tableModel);
QString getTabBase100(unsigned int node, const QSqlTableModel *tableModel);
QString getTabJoursAct(unsigned int node, const QSqlTableModel *tableModel);


}

#endif // PCX_TABLES_H
