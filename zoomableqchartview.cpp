/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le
 * Développement Local (2006--)
 *
 * labo@gipilab.org
 *
 * Ce logiciel est un programme informatique servant à cerner l'ensemble des
 * données budgétaires de la collectivité territoriale (audit, reporting
 * infra-annuel, prévision des dépenses à venir)
 *
 *
 * Ce logiciel est régi par la licence CeCILL soumise au droit français et
 * respectant les principes de diffusion des logiciels libres. Vous pouvez
 * utiliser, modifier et/ou redistribuer ce programme sous les conditions
 * de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA
 * sur le site "http://www.cecill.info".
 *
 * En contrepartie de l'accessibilité au code source et des droits de copie,
 * de modification et de redistribution accordés par cette licence, il n'est
 * offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
 * seule une responsabilité restreinte pèse sur l'auteur du programme, le
 * titulaire des droits patrimoniaux et les concédants successifs.
 *
 * A cet égard l'attention de l'utilisateur est attirée sur les risques
 * associés au chargement, à l'utilisation, à la modification et/ou au
 * développement et à la reproduction du logiciel par l'utilisateur étant
 * donné sa spécificité de logiciel libre, qui peut le rendre complexe à
 * manipuler et qui le réserve donc à des développeurs et des professionnels
 * avertis possédant des connaissances informatiques approfondies. Les
 * utilisateurs sont donc invités à charger et tester l'adéquation du
 * logiciel à leurs besoins dans des conditions permettant d'assurer la
 * sécurité de leurs systèmes et ou de leurs données et, plus généralement,
 * à l'utiliser et l'exploiter dans les mêmes conditions de sécurité.
 *
 * Le fait que vous puissiez accéder à cet en-tête signifie que vous avez
 * pris connaissance de la licence CeCILL, et que vous en avez accepté les
 * termes.
 *
 */

#include "zoomableqchartview.h"

ZoomableQChartView::ZoomableQChartView(QWidget *parent) : QChartView(parent) {}

void ZoomableQChartView::wheelEvent(QWheelEvent *event) {
  qreal factor;
  factor = event->angleDelta().y() > 0 ? 2 : 0.5;

  // Reset zoomFactor in case of chart change
  if (!chart()->isZoomed()) {
    zoomFactor = 1.0;
  }

  if (zoomFactor * factor > 8 || zoomFactor * factor < 0.5) {
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

  /* QValueAxis *yAxis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());
   if (yAxis != nullptr) {
     yAxis->applyNiceNumbers();
   }*/

  // NOTE : ugly hack to deal with pointLabels remaining bug
  resize(size() + QSize(1, 1));
  resize(size() - QSize(1, 1));
  QChartView::wheelEvent(event);
}

void ZoomableQChartView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    moveChart = true;
    lastPosX = event->position().x();
    lastPosY = event->position().y();
  }

  QChartView::mousePressEvent(event);
}

void ZoomableQChartView::mouseMoveEvent(QMouseEvent *event) {

  if (event->type() == QEvent::Leave) {
    moveChart = false;
    QChartView::mouseMoveEvent(event);
    return;
  }

  if (event->position().x() < 0 || event->position().y() < 0 || event->position().x() > size().width() ||
      event->position().y() > size().height()) {
    moveChart = false;
    QChartView::mouseMoveEvent(event);
    return;
  }

  if (moveChart) {

    chart()->scroll(lastPosX - event->position().x(), event->position().y() - lastPosY);

    lastPosX = event->position().x();
    lastPosY = event->position().y();
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
