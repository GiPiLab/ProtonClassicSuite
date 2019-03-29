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

#include "pcx_graphics.h"
#include "utils.h"
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <cfloat>

using namespace NUMBERSFORMAT;

PCx_Graphics::PCx_Graphics(PCx_Audit *model, QCustomPlot *plot, int graphicsWidth, int graphicsHeight)
    : auditModel(model) {
  if (model == nullptr) {
    qFatal("Assertion failed");
  }
  reportingModel = nullptr;
  setGraphicsWidth(graphicsWidth);
  setGraphicsHeight(graphicsHeight);

  if (plot == nullptr) {
    this->plot = new QCustomPlot();
    ownPlot = true;
  } else {
    this->plot = plot;
    ownPlot = false;
  }
}

PCx_Graphics::PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot, int graphicsWidth, int graphicsHeight)
    : reportingModel(reportingModel) {
  if (reportingModel == nullptr) {
    qFatal("Assertion failed");
  }
  auditModel = nullptr;
  setGraphicsWidth(graphicsWidth);
  setGraphicsHeight(graphicsHeight);

  if (plot == nullptr) {
    this->plot = new QCustomPlot();
    ownPlot = true;
  } else {
    this->plot = plot;
    ownPlot = false;
  }
}

PCx_Graphics::~PCx_Graphics() {
  if (ownPlot) {
    delete plot;
  }
}

QChart *PCx_Graphics::getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule,
                                 const PCx_PrevisionItem *prevItem, unsigned int referenceNode) const {

  if (node == 0 || plot == nullptr || auditModel == nullptr || referenceNode == 0) {
    qFatal("Assertion failed");
  }

  QString tableName = MODES::modeToTableString(mode);
  QString oredName = PCx_Audit::OREDtoTableString(modeORED);

  QSqlQuery q;

  // Will contain data read from db

  QMap<int, qint64> dataRoot, dataNode;

  q.prepare(QString("select * from audit_%1_%2 where id_node=:id or "
                    "id_node=:refNode order by annee")
                .arg(tableName)
                .arg(auditModel->getAuditId()));
  q.bindValue(":id", node);
  q.bindValue(":refNode", referenceNode);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  int firstYear = 0;

  while (q.next()) {
    int annee = q.value("annee").toInt();
    if (firstYear == 0) {
      firstYear = annee;
    }

    qint64 data = q.value(oredName).toLongLong();

    if (q.value("id_node").toUInt() == referenceNode) {
      dataRoot.insert(annee, data);
      if (node == referenceNode) {
        dataNode.insert(annee, data);
      }
    } else {
      dataNode.insert(annee, data);
    }
  }

  if (prevItem != nullptr) {
    int year = prevItem->getYear();
    dataNode.insert(year, prevItem->getSummedPrevisionItemValue());
    PCx_PrevisionItem tmpPrevRootItem = PCx_PrevisionItem(prevItem->getPrevision(), mode, referenceNode, year);
    tmpPrevRootItem.loadFromDb();
    dataRoot.insert(year, tmpPrevRootItem.getSummedPrevisionItemValue());
  }

  qint64 firstYearDataRoot = dataRoot.value(firstYear);
  qint64 firstYearDataNode = dataNode.value(firstYear);
  qint64 diffRootNode1 = firstYearDataRoot - firstYearDataNode;

  // Will contains computed data for Root, Node
  QList<QPointF> dataPlotRoot, dataPlotNode;

  double minYRange = DBL_MAX - 1, maxYRange = DBL_MIN + 1;
  int minYear = 10000, maxYear = -10000;

  for (auto it = dataRoot.constBegin(), end = dataRoot.constEnd(); it != end; ++it) {
    qint64 diffRootNode2 = 0, diffRootNode = 0, diffNode = 0;

    int year = it.key();
    // Skip the first year
    if (year > firstYear) {
      double percentRoot = 0.0, percentNode = 0.0;
      diffRootNode2 = it.value() - dataNode.value(year);

      diffRootNode = diffRootNode2 - diffRootNode1;
      diffNode = dataNode.value(year) - firstYearDataNode;

      if (diffRootNode1 != 0) {
        percentRoot = diffRootNode * 100.0 / diffRootNode1;
      }
      if (percentRoot < minYRange) {
        minYRange = percentRoot;
      }
      if (percentRoot > maxYRange) {
        maxYRange = percentRoot;
      }

      // NOTE: we round the number to the selected number of decimal as we cannot format point label explicitely
      dataPlotRoot.append(
          QPointF(yearToMsSinceEpoch(year), NUMBERSFORMAT::doubleToDoubleRoundedByNumbersOfDecimals(percentRoot)));

      if (firstYearDataNode != 0) {
        percentNode = diffNode * 100.0 / firstYearDataNode;
      }
      if (percentNode < minYRange) {
        minYRange = percentNode;
      }
      if (percentNode > maxYRange) {
        maxYRange = percentNode;
      }
      if (year > maxYear) {
        maxYear = year;
      }
      if (year < minYear) {
        minYear = year;
      }
      // NOTE: we round the number to the selected number of decimal as we cannot format point label explicitely
      dataPlotNode.append(
          QPointF(yearToMsSinceEpoch(year), NUMBERSFORMAT::doubleToDoubleRoundedByNumbersOfDecimals(percentNode)));

      // cumule==false => G1, G3, G5, G7, otherwise G2, G4, G6, G8
      if (!cumule) {
        diffRootNode1 = diffRootNode2;
        firstYearDataNode = dataNode.value(year);
      }
    }
  }

  QChart *chart = new QChart();

  QLineSeries *serie1 = new QLineSeries();
  QLineSeries *serie2 = new QLineSeries();

  serie1->append(dataPlotNode);
  serie2->append(dataPlotRoot);

  serie1->setPointsVisible(true);
  serie2->setPointsVisible(true);
  QColor c = getColorPen1();

  QPen pen(c);
  pen.setWidth(2);
  serie1->setPen(pen);

  c = getColorPen2();

  pen.setColor(c);
  serie2->setPen(pen);

  chart->addSeries(serie1);
  chart->addSeries(serie2);

  // Legend
  QString nodeName = auditModel->getAttachedTree()->getNodeName(node);
  QString refNodeName = auditModel->getAttachedTree()->getNodeName(referenceNode);
  serie1->setName(nodeName);
  serie2->setName(QString("%2 - %1").arg(nodeName, refNodeName));
  chart->setLocalizeNumbers(true);

  QDateTimeAxis *xAxis = new QDateTimeAxis;
  xAxis->setFormat("yyyy");
  xAxis->setRange(QDateTime(QDate(minYear - 1, 6, 1)), QDateTime(QDate(maxYear + 1, 6, 1)));
  xAxis->setTickCount(maxYear - minYear + 3);
  xAxis->setGridLineVisible(false);

  QValueAxis *yAxis = new QValueAxis();

  yAxis->setMin(minYRange - (qAbs(minYRange) * 0.2));
  yAxis->setMax(maxYRange + (qAbs(maxYRange) * 0.1));
  yAxis->applyNiceNumbers();
  yAxis->setLabelFormat("%.0f%");

  chart->addAxis(xAxis, Qt::AlignBottom);
  chart->addAxis(yAxis, Qt::AlignLeft);

  serie1->attachAxis(xAxis);
  serie1->attachAxis(yAxis);
  serie2->attachAxis(xAxis);
  serie2->attachAxis(yAxis);

  serie1->setPointLabelsVisible(true);
  serie1->setPointLabelsFormat("@yPoint%");
  serie2->setPointLabelsVisible(true);
  serie2->setPointLabelsFormat("@yPoint%");

  QString plotTitle;
  if (!cumule) {
    plotTitle =
        QObject::tr("<div style='text-align:center'><b>&Eacute;volution comparée des %1 de<br>%4 <i>hormis %2</i> et "
                    "de %2</b><br>(%3)</div>")
            .arg(PCx_Audit::OREDtoCompleteString(modeORED, true), nodeName.toHtmlEscaped(),
                 MODES::modeToCompleteString(mode), refNodeName.toHtmlEscaped());
  }

  else {
    plotTitle = QObject::tr("<div style='text-align:center'><b>&Eacute;volution comparée du cumulé des %1 de %4<br>"
                            "<i>hormis %2</i> et de %2</b><br>(%3)</div>")
                    .arg(PCx_Audit::OREDtoCompleteString(modeORED, true), nodeName.toHtmlEscaped(),
                         MODES::modeToCompleteString(mode), refNodeName.toHtmlEscaped());
  }

  chart->setTitle(plotTitle);

  // NOTE: Remove border
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chart->setBackgroundRoundness(0);

  return chart;
}

QChart *PCx_Graphics::getPCAG9(unsigned int node, int year) const {
  if (node == 0 || auditModel == nullptr || !auditModel->getYears().contains(year)) {
    qFatal("Assertion failed");
  }

  QString plotTitle;

  QChart *chart = new QChart();

  QBarSet *dfBar = new QBarSet(MODES::modeToCompactString(MODES::DFRFDIRI::DF));
  QBarSet *rfBar = new QBarSet(MODES::modeToCompactString(MODES::DFRFDIRI::RF));
  QBarSet *diBar = new QBarSet(MODES::modeToCompactString(MODES::DFRFDIRI::DI));
  QBarSet *riBar = new QBarSet(MODES::modeToCompactString(MODES::DFRFDIRI::RI));

  QPen pen;
  pen.setWidth(0);
  QColor c = getColorDFBar();
  int alpha = getAlpha();

  pen.setColor(c);
  dfBar->setPen(pen);
  c.setAlpha(alpha);
  dfBar->setBrush(c);

  c = getColorRFBar();

  pen.setColor(c);
  rfBar->setPen(pen);
  c.setAlpha(alpha);
  rfBar->setBrush(c);

  c = getColorDIBar();

  pen.setColor(c);
  diBar->setPen(pen);
  c.setAlpha(alpha);
  diBar->setBrush(c);

  c = getColorRIBar();

  pen.setColor(c);
  riBar->setPen(pen);
  c.setAlpha(alpha);
  riBar->setBrush(c);

  QSqlQuery q;
  q.prepare(QString("select a.annee, a.ouverts 'ouvertsDF', b.ouverts 'ouvertsRF', "
                    "c.ouverts 'ouvertsDI', d.ouverts 'ouvertsRI',"
                    "a.realises 'realisesDF', b.realises 'realisesRF', c.realises "
                    "'realisesDI', d.realises 'realisesRI',"
                    "a.engages 'engagesDF', b.engages 'engagesRF', c.engages "
                    "'engagesDI', d.engages 'engagesRI' "
                    "from audit_DF_%1 a, audit_RF_%1 b, audit_DI_%1 c,audit_RI_%1 d "
                    "where a.id_node=:id_node and a.id_node=b.id_node and "
                    "b.id_node=c.id_node "
                    "and c.id_node=d.id_node and a.annee=:year and b.annee=:year "
                    "and c.annee=:year and d.annee=:year ")
                .arg(auditModel->getAuditId()));
  q.bindValue(":id_node", node);
  q.bindValue(":year", year);
  q.exec();
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  QVector<QPair<QString, QString>> listModesAndLabels;
  listModesAndLabels.append(QPair<QString, QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS),
                                                    PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS, true)));
  listModesAndLabels.append(QPair<QString, QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES),
                                                    PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES, true)));
  listModesAndLabels.append(QPair<QString, QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES),
                                                    PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES, true)));

  QStringList labels;
  QList<double> valuesDF, valuesRF, valuesDI, valuesRI;

  QPair<QString, QString> mode;

  while (q.next()) {

    foreach (mode, listModesAndLabels) {

      qint64 dataDF = 0, dataRF = 0, dataDI = 0, dataRI = 0, sum = 0;
      double percentDF = 0.0, percentRF = 0.0, percentDI = 0.0, percentRI = 0.0;
      dataDF = q.value(mode.first + "DF").toLongLong();
      dataRF = q.value(mode.first + "RF").toLongLong();
      dataDI = q.value(mode.first + "DI").toLongLong();
      dataRI = q.value(mode.first + "RI").toLongLong();

      // In case of negative disponible
      if (dataDF < 0) {
        dataDF = 0;
      }
      if (dataRF < 0) {
        dataRF = 0;
      }
      if (dataDI < 0) {
        dataDI = 0;
      }
      if (dataRI < 0) {
        dataRI = 0;
      }

      sum = dataDF + dataRF + dataDI + dataRI;
      if (sum > 0) {
        percentDF = 100.0 * dataDF / sum;
        percentRF = 100.0 * dataRF / sum;
        percentDI = 100.0 * dataDI / sum;
        percentRI = 100.0 * dataRI / sum;
      }
      valuesDF.append(percentDF);
      valuesRF.append(percentRF);
      valuesDI.append(percentDI);
      valuesRI.append(percentRI);

      labels.append(mode.second);
    }
  }

  dfBar->append(valuesDF);
  rfBar->append(valuesRF);
  diBar->append(valuesDI);
  riBar->append(valuesRI);

  QPercentBarSeries *series = new QPercentBarSeries();

  series->append(dfBar);
  series->append(diBar);
  series->append(rfBar);
  series->append(riBar);

  series->setLabelsVisible(true);

  chart->addSeries(series);

  QBarCategoryAxis *axisX = new QBarCategoryAxis();
  axisX->append(labels);
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis();
  axisY->setLabelFormat("%d%%");

  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  plotTitle = QString("<div style='text-align:center'><b>D&eacute;penses et recettes de %1</b><br>en %2</div>")
                  .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped())
                  .arg(year);
  chart->setTitle(plotTitle);
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chart->setBackgroundRoundness(0);

  return chart;
}

QChart *PCx_Graphics::getPCAHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                                    const QList<PCx_Audit::ORED> &selectedORED, const PCx_PrevisionItem *prevItem,
                                    bool miniMode) const {

  if (auditModel == nullptr) {
    qWarning() << "Model error";
    return nullptr;
  }

  QChart *chart = new QChart();

  QString plotTitle;

  // Colors for each graph
  QColor PENCOLORS[4] = {Qt::darkCyan, Qt::red, Qt::gray, Qt::yellow};

  if (miniMode) {
    plotTitle = QObject::tr("<div style='text-align:center'><b>%1</b></div>")
                    .arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped());

  } else {
    if (prevItem != nullptr) {
      plotTitle =
          QObject::tr("<div style='text-align:center'><b>Données historiques et prévision pour %1</b><br>(%2)</div>")
              .arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),
                   MODES::modeToCompleteString(mode));
    } else {
      plotTitle = QObject::tr("<div style='text-align:center'><b>Données historiques pour %1</b><br>(%2)</div>")
                      .arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),
                           MODES::modeToCompleteString(mode));
    }
  }
  chart->setTitle(plotTitle);
  if (selectedORED.isEmpty()) {
    return nullptr;
  }

  QSqlQuery q;

  q.prepare(QString("select * from audit_%1_%2 where id_node=:id order by annee")
                .arg(MODES::modeToTableString(mode))
                .arg(auditModel->getAuditId()));
  q.bindValue(":id", selectedNodeId);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  QList<QPointF> dataSeries[static_cast<int>(PCx_Audit::ORED::NONELAST)];

  double maxYValue = -MAX_NUM;
  double minYValue = MAX_NUM;
  int minYear = 10000;
  int maxYear = -10000;

  while (q.next()) {
    int year = q.value("annee").toInt();

    for (int i = static_cast<int>(PCx_Audit::ORED::OUVERTS); i < static_cast<int>(PCx_Audit::ORED::NONELAST); i++) {
      if (selectedORED.contains(static_cast<PCx_Audit::ORED>(i))) {
        qint64 data = NUMBERSFORMAT::fixedDividedByFormatMode(
            q.value(PCx_Audit::OREDtoTableString(static_cast<PCx_Audit::ORED>(i))).toLongLong());
        double dataF = NUMBERSFORMAT::fixedPointToDouble(data);
        if (dataF > maxYValue) {
          maxYValue = dataF;
        }
        if (year < minYear) {
          minYear = year;
        }
        if (year > maxYear) {
          maxYear = year;
        }
        if (dataF < minYValue) {
          minYValue = dataF;
        }
        dataSeries[i].append(QPointF(yearToMsSinceEpoch(year), dataF));
      }
    }
  }

  if (prevItem != nullptr) {
    qint64 summedPrev = NUMBERSFORMAT::fixedDividedByFormatMode(prevItem->getSummedPrevisionItemValue());
    double summedPrevF = NUMBERSFORMAT::fixedPointToDouble(summedPrev);
    if (summedPrevF > maxYValue) {
      maxYValue = summedPrevF;
    }
    if (summedPrevF < minYValue) {
      minYValue = summedPrevF;
    }
    dataSeries[static_cast<int>(PCx_Audit::ORED::OUVERTS)].append(
        QPointF(yearToMsSinceEpoch(prevItem->getYear()), summedPrevF));
  }

  QLineSeries *series = nullptr;

  QDateTimeAxis *xAxis = new QDateTimeAxis;
  if (prevItem == nullptr) {
    xAxis->setTickCount(maxYear - minYear + 3);
    xAxis->setRange(QDateTime(QDate(minYear - 1, 6, 1)), QDateTime(QDate(maxYear + 1, 6, 1)));

  } else {
    xAxis->setTickCount(maxYear - minYear + 4);
    xAxis->setRange(QDateTime(QDate(minYear - 1, 6, 1)), QDateTime(QDate(maxYear + 2, 6, 1)));
  }

  xAxis->setFormat("yyyy");
  xAxis->setGridLineVisible(false);

  QValueAxis *yAxis = new QValueAxis();

  chart->addAxis(xAxis, Qt::AlignBottom);
  chart->addAxis(yAxis, Qt::AlignLeft);
  yAxis->setGridLineVisible(true);

  for (int i = static_cast<int>(PCx_Audit::ORED::OUVERTS); i < static_cast<int>(PCx_Audit::ORED::NONELAST); i++) {
    if (selectedORED.contains(static_cast<PCx_Audit::ORED>(i))) {
      QPen pen(PENCOLORS[i]);
      pen.setWidth(2);
      series = new QLineSeries();
      series->append(dataSeries[i]);
      series->setName(PCx_Audit::OREDtoCompleteString(static_cast<PCx_Audit::ORED>(i), true));
      series->setPen(pen);
      chart->addSeries(series);
      series->attachAxis(xAxis);
      series->attachAxis(yAxis);
      series->setPointsVisible(true);
    }
  }

  yAxis->setRange(minYValue - (qAbs(minYValue) * 0.05), maxYValue + (qAbs(maxYValue) * 0.1));
  chart->setLocalizeNumbers(true);

  QString formatString;
  switch (NUMBERSFORMAT::getFormatMode()) {
  case NUMBERSFORMAT::FORMATMODE::FORMATMODENORMAL:
    formatString = "%.0f" + NUMBERSFORMAT::getFormatModeSuffix();
    break;
  case NUMBERSFORMAT::FORMATMODE::FORMATMODETHOUSANDS:
    formatString = "%.2f" + NUMBERSFORMAT::getFormatModeSuffix();
    break;
  case NUMBERSFORMAT::FORMATMODE::FORMATMODEMILLIONS:
    formatString = "%.2f" + NUMBERSFORMAT::getFormatModeSuffix();
    break;
  }

  yAxis->setLabelFormat(formatString);

  yAxis->applyNiceNumbers();

  if (miniMode) {
    QFont font("Sans", 6);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->setTitle(QString());
    xAxis->setLabelsFont(font);
    yAxis->setLabelsFont(font);
    xAxis->setLabelsAngle(45);
    xAxis->setFormat("yy");
  }

  // NOTE: Remove border
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chart->setBackgroundRoundness(0);

  return chart;
}

QPixmap PCx_Graphics::chartToPixmap(QChart *chart) const {
  QPixmap pixmap;
  QChartView chartView(chart);
  chartView.resize(graphicsWidth, graphicsHeight);
  chartView.setRenderHint(QPainter::Antialiasing, true);
  pixmap = chartView.grab();
  return pixmap;
}

bool PCx_Graphics::saveChartToDisk(QChart *chart, const QString &imageAbsoluteName) const {
  QPixmap pixmap = chartToPixmap(chart);
  return pixmap.save(imageAbsoluteName, "png");
}

QStandardItemModel *PCx_Graphics::getListOfAvailablePCAGraphics() const {}

bool PCx_Graphics::savePlotToDisk(const QString &imageAbsoluteName) const {
  if (!plot->savePng(imageAbsoluteName, graphicsWidth, graphicsHeight)) {
    qCritical() << "Unable to save " << imageAbsoluteName;
    return false;
  }
  return true;
}

QString PCx_Graphics::getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                                    const QList<PCx_Reporting::OREDPCR> &selectedOREDPCR) {
  if (reportingModel == nullptr) {
    qWarning() << "Model error";
    return QString();
  }

  if (ownPlot) {
    delete plot;
    plot = new QCustomPlot();
  } else {
    plot->clearGraphs();
    plot->clearItems();
    plot->clearPlottables();
  }

  QString plotTitle = QObject::tr("%1\n(%2)")
                          .arg(reportingModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),
                               MODES::modeToCompleteString(mode));

  QCPTextElement *title;
  if (plot->plotLayout()->elementCount() == 1) {
    plot->plotLayout()->insertRow(0);
    title = new QCPTextElement(plot, plotTitle);
    title->setFont(QFont("Sans serif", 11));
    plot->plotLayout()->addElement(0, 0, title);
  } else {
    title = dynamic_cast<QCPTextElement *>(plot->plotLayout()->elementAt(0));
    title->setText(plotTitle);
  }

  if (selectedOREDPCR.isEmpty()) {
    plot->replot();
    return QString();
  }

  // Colors for each graph
  Qt::GlobalColor PENCOLORS[static_cast<int>(PCx_Reporting::OREDPCR::NONELAST)] = {
      Qt::blue,    Qt::red,      Qt::yellow,   Qt::green,     Qt::magenta,   Qt::cyan,
      Qt::darkRed, Qt::darkBlue, Qt::darkGray, Qt::darkGreen, Qt::darkYellow};

  QSqlQuery q;

  q.prepare(QString("select * from reporting_%1_%2 where id_node=:id order by date")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":id", selectedNodeId);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }
  QVector<double> dataX, dataY[static_cast<int>(PCx_Reporting::OREDPCR::NONELAST)];

  while (q.next()) {
    unsigned int date = q.value("date").toUInt();
    dataX.append(static_cast<double>(date));

    for (int i = static_cast<int>(PCx_Reporting::OREDPCR::OUVERTS);
         i < static_cast<int>(PCx_Reporting::OREDPCR::NONELAST); i++) {
      if (selectedOREDPCR.contains(static_cast<PCx_Reporting::OREDPCR>(i))) {
        qint64 data = q.value(PCx_Reporting::OREDPCRtoTableString(static_cast<PCx_Reporting::OREDPCR>(i))).toLongLong();
        dataY[i].append(NUMBERSFORMAT::fixedPointToDouble(data));
      }
    }
  }

  if (dataX.isEmpty()) {
    plot->replot();
    return QString();
  }

  bool first = true;
  for (int i = static_cast<int>(PCx_Reporting::OREDPCR::OUVERTS);
       i < static_cast<int>(PCx_Reporting::OREDPCR::NONELAST); i++) {
    if (selectedOREDPCR.contains(static_cast<PCx_Reporting::OREDPCR>(i))) {
      plot->addGraph();
      plot->graph()->setData(dataX, dataY[i]);
      plot->graph()->setName(PCx_Reporting::OREDPCRtoCompleteString(static_cast<PCx_Reporting::OREDPCR>(i)));
      plot->graph()->setPen(QPen(PENCOLORS[i]));
      plot->graph()->setScatterStyle(QCPScatterStyle::ssDisc);
      plot->graph()->rescaleAxes(!first);
      first = false;
    }
  }

  QCPRange range;
  int year = QDateTime::fromTime_t(static_cast<unsigned int>(dataX.first())).date().year();
  QDateTime dtBegin(QDate(year, 1, 1));
  QDateTime dtEnd(QDate(year, 12, 31));
  plot->xAxis->setRangeLower(static_cast<double>(dtBegin.toTime_t()));
  plot->xAxis->setRangeUpper(static_cast<double>(dtEnd.toTime_t()));

  range = plot->yAxis->range();
  range.lower -= (range.lower * 20.0 / 100.0);
  range.upper += (range.upper * 10.0 / 100.0);
  plot->yAxis->setRange(range);

  plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  plot->legend->setVisible(true);
  plot->legend->setFont(QFont("Sans serif", 8));
  plot->legend->setRowSpacing(-5);
  plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

  plot->xAxis->setTickLabelRotation(0);
  plot->yAxis->setLabel("€");

  QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
  dateTimeTicker->setDateTimeFormat("MMM\nyyyy");

  plot->xAxis->setTicker(dateTimeTicker);

  plot->replot();
  return plotTitle;
}

QString PCx_Graphics::getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode) {
  QString nodeName = reportingModel->getAttachedTree()->getNodeName(nodeId);
  QString modeName = MODES::modeToCompleteString(mode);

  QList<PCx_Reporting::OREDPCR> selectedORED = {PCx_Reporting::OREDPCR::BP, PCx_Reporting::OREDPCR::REPORTS,
                                                PCx_Reporting::OREDPCR::OCDM, PCx_Reporting::OREDPCR::VCDM,
                                                PCx_Reporting::OREDPCR::VIREMENTSINTERNES};
  return getPCRPercentBars(nodeId, mode, selectedORED, PCx_Reporting::OREDPCR::OUVERTS,
                           QString("Provenance des crédits de\n%1\n(%2)").arg(nodeName, modeName), getColorDFBar());
}

QString PCx_Graphics::getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode) {
  QString nodeName = reportingModel->getAttachedTree()->getNodeName(nodeId);
  QString modeName = MODES::modeToCompleteString(mode);

  QList<PCx_Reporting::OREDPCR> selectedORED = {PCx_Reporting::OREDPCR::OCDM, PCx_Reporting::OREDPCR::VCDM,
                                                PCx_Reporting::OREDPCR::VIREMENTSINTERNES};
  return getPCRPercentBars(nodeId, mode, selectedORED, PCx_Reporting::OREDPCR::BP,
                           QString("Facteurs de variation des crédits de\n%1\n(%2)").arg(nodeName, modeName),
                           getColorRFBar());
}

QString PCx_Graphics::getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode) {
  QString nodeName = reportingModel->getAttachedTree()->getNodeName(nodeId);
  QString modeName = MODES::modeToCompleteString(mode);
  QList<PCx_Reporting::OREDPCR> selectedORED = {PCx_Reporting::OREDPCR::REALISES, PCx_Reporting::OREDPCR::ENGAGES,
                                                PCx_Reporting::OREDPCR::DISPONIBLES};
  return getPCRPercentBars(nodeId, mode, selectedORED, PCx_Reporting::OREDPCR::OUVERTS,
                           QString("Utilisation des crédits de\n%1\n(%2)").arg(nodeName, modeName), getColorDIBar());
}

QString PCx_Graphics::getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode) {
  QList<PCx_Reporting::OREDPCR> oredPCR = {PCx_Reporting::OREDPCR::OUVERTS, PCx_Reporting::OREDPCR::REALISES};
  return getPCRHistory(nodeId, mode, oredPCR);
}

QCustomPlot *PCx_Graphics::getPlot() const { return plot; }

QString PCx_Graphics::getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                                        QList<PCx_Reporting::OREDPCR> selectedOREDPCR,
                                        PCx_Reporting::OREDPCR oredReference, const QString &plotTitle, QColor color) {
  if (reportingModel == nullptr) {
    qWarning() << "Model error";
    return QString();
  }
  if (ownPlot) {
    delete plot;
    plot = new QCustomPlot();
  } else {
    plot->clearGraphs();
    plot->clearItems();
    plot->clearPlottables();
  }

  QCPTextElement *title;
  if (plot->plotLayout()->elementCount() == 1) {
    plot->plotLayout()->insertRow(0);
    title = new QCPTextElement(plot, plotTitle);
    title->setFont(QFont("Sans serif", 11));
    plot->plotLayout()->addElement(0, 0, title);
  } else {
    title = dynamic_cast<QCPTextElement *>(plot->plotLayout()->elementAt(0));
    title->setText(plotTitle);
  }

  auto *bars = new QCPBars(plot->xAxis, plot->yAxis);

  QPen pen;
  pen.setWidth(0);

  int alpha = getAlpha();

  pen.setColor(color);
  bars->setPen(pen);
  color.setAlpha(alpha);
  bars->setBrush(color);

  QSqlQuery q;
  q.prepare(QString("select * from reporting_%1_%2 where id_node=:id_node "
                    "order by date desc limit 1")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":id_node", selectedNodeId);
  q.exec();
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  QVector<double> ticks;
  QVector<double> values;

  int i = 1;

  QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

  plot->xAxis->setTicker(textTicker);

  foreach (PCx_Reporting::OREDPCR ored, selectedOREDPCR) {
    textTicker->addTick(i, PCx_Reporting::OREDPCRtoCompleteString(ored));
    ticks.append(i);
    i++;
  }

  while (q.next()) {
    qint64 refVal = q.value(PCx_Reporting::OREDPCRtoTableString(oredReference)).toLongLong();
    if (refVal == 0) {
      return plotTitle;
    }

    foreach (PCx_Reporting::OREDPCR ored, selectedOREDPCR) {
      qint64 val = q.value(PCx_Reporting::OREDPCRtoTableString(ored)).toLongLong();
      values.append(static_cast<double>(val) / static_cast<double>(refVal) * 100.0);
    }
  }

  bars->setData(ticks, values);

  plot->legend->setVisible(false);

  plot->yAxis->setLabel("%");
  plot->xAxis->setTickLength(0, 0);
  plot->xAxis->grid()->setVisible(false);
  plot->xAxis->setRange(0, ticks.count() + 1);
  plot->yAxis->rescale();

  return plotTitle;
}

void PCx_Graphics::setGraphicsWidth(int width) {
  graphicsWidth = width;
  if (graphicsWidth < MINWIDTH) {
    graphicsWidth = MINWIDTH;
  }
  if (graphicsWidth > MAXWIDTH) {
    graphicsWidth = MAXWIDTH;
  }
}

void PCx_Graphics::setGraphicsHeight(int height) {
  graphicsHeight = height;
  if (graphicsHeight < MINHEIGHT) {
    graphicsHeight = MINHEIGHT;
  }
  if (graphicsHeight > MAXHEIGHT) {
    graphicsHeight = MAXHEIGHT;
  }
}

QColor PCx_Graphics::getColorPen1() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/pen1", PCx_Graphics::DEFAULTPENCOLOR1).toUInt();
  return {oldcolor};
}

QColor PCx_Graphics::getColorPen2() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/pen2", PCx_Graphics::DEFAULTPENCOLOR2).toUInt();
  return {oldcolor};
}

QColor PCx_Graphics::getColorDFBar() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/penDFBar", PCx_Graphics::DEFAULTCOLORDFBAR).toUInt();
  return {oldcolor};
}

QColor PCx_Graphics::getColorRFBar() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/penRFBar", PCx_Graphics::DEFAULTCOLORRFBAR).toUInt();
  return {oldcolor};
}

QColor PCx_Graphics::getColorDIBar() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/penDIBar", PCx_Graphics::DEFAULTCOLORDIBAR).toUInt();
  return {oldcolor};
}

QColor PCx_Graphics::getColorRIBar() {
  QSettings settings;
  unsigned int oldcolor = settings.value("graphics/penRIBar", PCx_Graphics::DEFAULTCOLORRIBAR).toUInt();
  return {oldcolor};
}

int PCx_Graphics::getAlpha() {
  QSettings settings;
  int alpha = settings.value("graphics/alpha", PCx_Graphics::DEFAULTALPHA).toInt();
  return alpha;
}

QString PCx_Graphics::getCSS() {
  return "\ndiv.g{margin-top:2em;margin-left:auto;margin-right:auto;page-break-"
         "inside:avoid;}";
}
