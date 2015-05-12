#ifndef TREEMAPWIDGET_H
#define TREEMAPWIDGET_H

#include "treemap.h"
#include "pcx_audit.h"
#include <QWidget>

class TreeMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TreeMapWidget(QWidget *parent = 0);
    ~TreeMapWidget();

    void setDataGroupByNode(PCx_Audit *audit, MODES::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int year, unsigned int nodeId=1);
    void setDataGroupByYears(PCx_Audit *audit, MODES::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int nodeId);
    void setDataGroupByModes(PCx_Audit *audit, PCx_Audit::ORED ored, unsigned int year, unsigned int nodeId);
    void setDataGroupByORED(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int year, unsigned int nodeId);
signals:
    void clicked(const QString &name, int id, int year, double value);
    void highlighted(const QString &name, int id, int year, double value);

public slots:
    bool eventFilter(QObject *object, QEvent *e);

protected:
    virtual void paintEvent(QPaintEvent *paintEvent);
    virtual void resizeEvent(QResizeEvent *resizeEvent);

private:
    TreeMap *root;
    TreeMap *highlight;
};

#endif // TREEMAPWIDGET_H
