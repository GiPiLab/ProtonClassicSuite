#include "zoomableqchartview.h"

ZoomableQChartView::ZoomableQChartView(QWidget *parent) : QChartView(parent) {}

void ZoomableQChartView::wheelEvent(QWheelEvent *event) {
  qreal factor;
  factor = event->angleDelta().y() > 0 ? 2.0 : 0.5;

  // Reset zoomFactor in case of chart change
  if (!chart()->isZoomed()) {
    zoomFactor = 1.0;
  }

  if (zoomFactor * factor >= 17 || zoomFactor * factor <= 0.124) {
    event->ignore();
    return;
  }
  zoomFactor *= factor;
  QRectF rect = chart()->plotArea();
  rect.setWidth(rect.width() / factor);
  rect.setHeight(rect.height() / factor);
  QPointF mousePos = mapFromGlobal(QCursor::pos());
  rect.moveCenter(mousePos);
  chart()->zoomIn(rect);
  // NOTE : ugly hack to deal with pointLabels remaining bug
  resize(size() + QSize(1, 1));
  resize(size() - QSize(1, 1));
  event->accept();
}
