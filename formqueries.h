#ifndef FORMQUERIES_H
#define FORMQUERIES_H

#include "pcx_auditmodel.h"
#include "pcx_report.h"
#include "pcx_queryvariation.h"
#include "pcx_queriesmodel.h"
#include "pcx_queryrank.h"
#include <QWidget>
#include <QTextDocument>

namespace Ui {
class FormQueries;
}

class FormQueries : public QWidget
{
    Q_OBJECT

public:
    explicit FormQueries(QWidget *parent = 0);
    ~FormQueries();

public slots:
    void onListOfAuditsChanged();
    void onColorChanged();

private slots:
    void on_comboBoxListAudits_activated(int index);

    void on_pushButtonExecReqVariation_clicked();

    void on_comboBoxAugDim_activated(int index);

    void on_pushButtonSaveReqVariation_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonExecFromList_clicked();

    void on_listView_activated(const QModelIndex &index);

    void on_pushButtonSave_clicked();

    void on_pushButtonExecReqRank_clicked();

    void on_pushButtonSaveReqRank_clicked();

    void on_pushButtonExecReq3_clicked();

    void on_pushButtonSaveReq3_clicked();

private:
    Ui::FormQueries *ui;
    void updateListOfAudits();
    PCx_AuditModel *model;
    PCx_Report *report;
    PCx_QueriesModel *queriesModel;
    QTextDocument *doc;
    QString currentHtmlDoc;

    bool getParamsReqVariation(unsigned int &typeId, PCx_AuditModel::ORED &ored, PCx_AuditModel::DFRFDIRI &dfrfdiri,
                       PCx_QueryVariation::INCREASEDECREASE &increase, PCx_QueryVariation::PERCENTORPOINTS &percent,
                       PCx_QueryVariation::OPERATORS &oper, qint64 &val, unsigned int &year1, unsigned int &year2);
    QString execQueries(QModelIndexList items);
    bool getParamsReqRank(unsigned int &typeId, PCx_AuditModel::ORED &ored, PCx_AuditModel::DFRFDIRI &dfrfdiri, unsigned int &number, PCx_QueryRank::GREATERSMALLER &grSm, unsigned int &year1, unsigned int &year2);
    bool getParamsReqMinMax(unsigned int &typeId, PCx_AuditModel::ORED &ored, PCx_AuditModel::DFRFDIRI &dfrfdiri, qint64 &val1, qint64 &val2, unsigned int &year1, unsigned int &year2);
};

#endif // FORMQUERIES_H
