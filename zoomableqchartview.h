#ifndef ZOOMABLEQCHARTVIEW_H
#define ZOOMABLEQCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts>

class ZoomableQChartView : public QChartView {
public:
  ZoomableQChartView(QWidget *parent);

protected:
  void keyPressEvent(QKeyEvent *event);
};

#endif // ZOOMABLEQCHARTVIEW_H
