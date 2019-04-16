#ifndef ZOOMABLEQCHARTVIEW_H
#define ZOOMABLEQCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts>

class ZoomableQChartView : public QChartView {
public:
  ZoomableQChartView(QWidget *parent);

protected:
  void wheelEvent(QWheelEvent *event);

private:
  qreal zoomFactor = 1.0;
};

#endif // ZOOMABLEQCHARTVIEW_H
