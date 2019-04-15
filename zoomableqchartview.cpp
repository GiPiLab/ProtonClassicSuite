#include "zoomableqchartview.h"

ZoomableQChartView::ZoomableQChartView(QWidget *parent) : QChartView(parent) {}

void ZoomableQChartView::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Plus:
    chart()->zoomIn();
    break;
  case Qt::Key_Minus:
    chart()->zoomOut();
    break;
  }
}
