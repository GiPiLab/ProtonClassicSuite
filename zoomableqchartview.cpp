#include "zoomableqchartview.h"

ZoomableQChartView::ZoomableQChartView(QWidget *parent) : QChartView(parent) {}

void ZoomableQChartView::wheelEvent(QWheelEvent *event) {
  qreal factor;
  factor = event->angleDelta().y() > 0 ? 1.5 : 0.6;

  // Reset zoomFactor in case of chart change
  if (!chart()->isZoomed()) {
    zoomFactor = 1.0;
  }

  if (zoomFactor * factor >= 17 || zoomFactor * factor <= 0.124) {
    QChartView::wheelEvent(event);
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
  QChartView::wheelEvent(event);
}

void ZoomableQChartView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    moveChart = true;
    lastPosX = event->x();
    lastPosY = event->y();
  }

  QChartView::mousePressEvent(event);
}

void ZoomableQChartView::mouseMoveEvent(QMouseEvent *event) {

  if (event->type() == QEvent::Leave) {
    moveChart = false;
    QChartView::mouseMoveEvent(event);
    return;
  }

  if (event->x() < 0 || event->y() < 0 || event->x() > size().width() || event->y() > size().height()) {
    moveChart = false;
    QChartView::mouseMoveEvent(event);
    return;
  }

  if (moveChart) {

    chart()->scroll(lastPosX - event->x(), event->y() - lastPosY);
    lastPosX = event->x();
    lastPosY = event->y();
    // NOTE : ugly hack to deal with pointLabels remaining bug
    resize(size() + QSize(1, 1));
    resize(size() - QSize(1, 1));
  }

  QChartView::mouseMoveEvent(event);
}

void ZoomableQChartView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    moveChart = false;
    // NOTE : ugly hack to deal with pointLabels remaining bug
    resize(size() + QSize(1, 1));
    resize(size() - QSize(1, 1));
  }
  QChartView::mouseReleaseEvent(event);
}
