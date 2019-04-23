#ifndef ZOOMABLEQCHARTVIEW_H
#define ZOOMABLEQCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts>

class ZoomableQChartView : public QChartView {
public:
  ZoomableQChartView(QWidget *parent);

protected:
  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private:
  bool moveChart = false;
  qreal zoomFactor = 1.0;
  int lastPosX = 0, lastPosY = 0;
};

#endif // ZOOMABLEQCHARTVIEW_H
