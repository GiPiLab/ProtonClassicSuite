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

#include "pcx_report.h"
#include "pcx_query.h"
#include "utils.h"
#include <QtSvg/QtSvg>

PCx_Report::PCx_Report(PCx_Audit *model, int graphicsWidth, int graphicsHeight)
    : auditModel(model), tables(model), graphics(model, graphicsWidth, graphicsHeight) {
    if (model == nullptr) {
        qFatal("Assertion failed");
    }
    reportingModel = nullptr;
}

PCx_Report::PCx_Report(PCx_Reporting *reportingModel, int graphicsWidth, int graphicsHeight)
    : reportingModel(reportingModel), tables(reportingModel), graphics(reportingModel, graphicsWidth, graphicsHeight) {
    auditModel = nullptr;
}

QString PCx_Report::generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS> listOfTabs,
                                                   QList<PCx_Tables::PCATABLES> listOfTables,
                                                   QList<PCx_Graphics::PCAGRAPHICS> listOfGraphics,
                                                   unsigned int selectedNode, MODES::DFRFDIRI mode,
                                                   unsigned int referenceNode, QTextDocument *document,
                                                   const QString &absoluteImagePath, const QString &relativeImagePath,
                                                   QProgressDialog *progress, const PCx_PrevisionItem *prevItem) {
  if (selectedNode == 0 || referenceNode == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }
  QElapsedTimer timer;
  timer.start();

  QString output = "\n<div class='auditNodeContainer'>\n";

  // Either group of tables, or individual tables
  if (!listOfTabs.isEmpty()) {
    foreach (PCx_Tables::PCAPRESETS tab, listOfTabs) {
      switch (tab) {
      case PCx_Tables::PCAPRESETS::PCARELATIVEWEIGHTS:
        output.append(tables.getPCAPresetRelativeWeights(selectedNode, mode, referenceNode));
        break;
      case PCx_Tables::PCAPRESETS::PCAEVOLUTION:
        output.append(tables.getPCAPresetEvolution(selectedNode, mode, referenceNode));
        break;
      case PCx_Tables::PCAPRESETS::PCAEVOLUTIONCUMUL:
        output.append(tables.getPCAPresetEvolutionCumul(selectedNode, mode, referenceNode));
        break;
      case PCx_Tables::PCAPRESETS::PCABASE100:
        output.append(tables.getPCAPresetBase100(selectedNode, mode, referenceNode));
        break;
      case PCx_Tables::PCAPRESETS::PCADAYOFWORK:
        output.append(tables.getPCAPresetDayOfWork(selectedNode, mode));
        break;
      case PCx_Tables::PCAPRESETS::PCARESULTS:
        output.append(tables.getPCAPresetResults(selectedNode));
        break;
      }
    }
  } else {
    foreach (PCx_Tables::PCATABLES table, listOfTables) {
      switch (table) {
      case PCx_Tables::PCATABLES::PCARAWDATA:
        output.append(tables.getPCARawData(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT1:
        output.append(tables.getPCAT1(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT2:
        output.append(tables.getPCAT2(selectedNode, mode, referenceNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT2BIS:
        output.append(tables.getPCAT2bis(selectedNode, mode, referenceNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT3:
        output.append(tables.getPCAT3(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT3BIS:
        output.append(tables.getPCAT3bis(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT4:
        output.append(tables.getPCAT4(selectedNode, mode, referenceNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT5:
        output.append(tables.getPCAT5(selectedNode, mode, referenceNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT6:
        output.append(tables.getPCAT6(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT7:
        output.append(tables.getPCAT7(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT8:
        output.append(tables.getPCAT8(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT9:
        output.append(tables.getPCAT9(selectedNode, mode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT10:
        output.append(tables.getPCAT10(selectedNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT11:
        output.append(tables.getPCAT11(selectedNode) + "<br>");
        break;
      case PCx_Tables::PCATABLES::PCAT12:
        output.append(tables.getPCAT12(selectedNode) + "<br>");
        break;
      }
    }
  }

  int graphicsWidth = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::WIDTH).toInt();
  int graphicsHeight = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::HEIGHT).toInt();

  // To save images
  QString encodedRelativeImagePath = QUrl::toPercentEncoding(relativeImagePath);

  QString suffix = ".png";

  int progressValue = 0;
  if (progress != nullptr) {
    progressValue = progress->value();
  }

  QChart *chart = nullptr;

  foreach (PCx_Graphics::PCAGRAPHICS graph, listOfGraphics) {

    switch (graph) {
    case PCx_Graphics::PCAGRAPHICS::PCAHISTORY:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAHistory(selectedNode, mode,
                                     {PCx_Audit::ORED::OUVERTS, PCx_Audit::ORED::REALISES, PCx_Audit::ORED::ENGAGES},
                                     prevItem, false);
      break;

    case PCx_Graphics::PCAGRAPHICS::PCAG1:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG1(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG2:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG2(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG3:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG3(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG4:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG4(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG5:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG5(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG6:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG6(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG7:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG7(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG8:
      output.append("<div align='center' class='g'>");
      chart = graphics.getPCAG8(selectedNode, mode, prevItem, referenceNode);
      break;
    case PCx_Graphics::PCAGRAPHICS::PCAG9:

      // NOTE : PCAG9 is now a chart per year
      foreach (int year, auditModel->getYears()) {
        chart = graphics.getPCAG9(selectedNode, year);

        output.append("<div align='center' class='g'>");

        QPixmap pixmap;
        pixmap = graphics.chartToPixmap(chart);

        if (document != nullptr) {
          QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());
          document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
          output.append(QString("<img alt='GRAPH' src='%1'></div><br>").arg(name));
        } else {
          QString imageName = generateUniqueFileName(suffix);
          QString imageAbsoluteName = imageName;
          imageName.prepend(encodedRelativeImagePath + "/");
          imageAbsoluteName.prepend(absoluteImagePath + "/");
          if (PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName) == false) {
            break;
          }

          output.append(QString("<img width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                            .arg(graphicsWidth)
                            .arg(graphicsHeight)
                            .arg(imageName));
        }
      }
      break;
    }

    // NOTE : PCAG9 needs to be called for each year, done previously
    if (graph != PCx_Graphics::PCAGRAPHICS::PCAG9) {

      QPixmap pixmap;
      pixmap = graphics.chartToPixmap(chart);

      // Inline mode
      if (document != nullptr) {

        QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());

        document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
        output.append(QString("<img alt='GRAPH' src='%1'></div><br>").arg(name));
      } else { // Export mode
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        QString imageName = generateUniqueFileName(suffix);
        QString imageAbsoluteName = imageName;
        imageName.prepend(encodedRelativeImagePath + "/");
        imageAbsoluteName.prepend(absoluteImagePath + "/");

        if (PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName) == false) {
          break;
        }

        output.append(QString("<img width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth)
                          .arg(graphicsHeight)
                          .arg(imageName));
        if (progress != nullptr) {
          progress->setValue(++progressValue);
        }
      }
    }
  }
  output.append("\n</div>\n");
  qDebug() << "Time to render for node" << selectedNode << "=" << timer.elapsed() << "ms";

  return output;
}

QString PCx_Report::generateHTMLReportingReportForNode(QList<PCx_Report::PCRPRESETS> listOfPresets,
                                                       unsigned int selectedNode, MODES::DFRFDIRI mode,
                                                       bool includeGraphics, QTextDocument *document,
                                                       const QString &absoluteImagePath,
                                                       const QString &relativeImagePath, QProgressDialog *progress) {
  if (reportingModel == nullptr) {
    qCritical() << "NULL model error";
    return QString();
  }
  bool inlineImageMode = false;

  QChart *chart = nullptr;
  QPixmap pixmap;

  QString encodedRelativeImagePath = QUrl::toPercentEncoding(relativeImagePath);

  if (document != nullptr) {
    inlineImageMode = true;

  } else if (absoluteImagePath.isEmpty() || encodedRelativeImagePath.isEmpty()) {
    qWarning() << "Please pass an absolute and relative path to store images";
    die();
  }

  QString suffix = ".png";

  int progressValue = 0;
  if (progress != nullptr) {
    progressValue = progress->value();
  }

  int graphicsWidth = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::WIDTH).toInt();
  int graphicsHeight = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::HEIGHT).toInt();

  QString output = "\n<div class='reportingNodeContainer'>\n";

  if (listOfPresets.contains(PCRPRESETS::PCRPRESET_S)) {
    output.append("<h4>Synthèse de fonctionnement (RF-DF)</h4>");
    output.append(tables.getPCRRFDF(selectedNode));
    output.append("<h4>Synthèse d'investissement (RI-DI)</h4>");
    output.append(tables.getPCRRIDI(selectedNode));
    output.append("<h4>Synthèse globale (RF-DF)+(RI-DI)</h4>");
    output.append(tables.getPCRRFDFRIDI(selectedNode));
    listOfPresets.removeAll(PCRPRESETS::PCRPRESET_S);
  }

  if (listOfPresets.isEmpty()) {
    return output;
  }

  QString modeName = MODES::modeToCompleteString(mode);

  output.append(QString("<h2>%1</h2>").arg(modeName));

  output.append(tables.getPCRRatioParents(selectedNode, mode));

  if (listOfPresets.contains(PCRPRESETS::PCRPRESET_A)) {
    output.append("<h4>Provenance des crédits</h4>");
    output.append(tables.getPCRProvenance(selectedNode, mode));

    if (includeGraphics) {
        chart = graphics.getPCRProvenance(selectedNode, mode);
        pixmap = graphics.chartToPixmap(chart);

        if (inlineImageMode) {
          QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());
          document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
          output.append(QString("<br><div class='g' align='center'><img align='center' "
                                "alt='GRAPH' src='%3'></div><br>")
                            .arg(name));

        } else {
            QString imageName = generateUniqueFileName(suffix);
            QString imageAbsoluteName = imageName;
            imageName.prepend(encodedRelativeImagePath + "/");
            imageAbsoluteName.prepend(absoluteImagePath + "/");

            if (!PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName)) {
                die();
            }
            output.append(QString("<br><div align='center' class='g'><img align='center' "
                                  "width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                              .arg(graphicsWidth)
                              .arg(graphicsHeight)
                              .arg(imageName));
            if (progress != nullptr) {
                progress->setValue(++progressValue);
            }
        }
    }
  }

  if (listOfPresets.contains(PCRPRESETS::PCRPRESET_B)) {
    output.append("<h4>Facteurs de variation des crédits</h4>");
    output.append(tables.getPCRVariation(selectedNode, mode));

    if (includeGraphics) {
        chart = graphics.getPCRVariation(selectedNode, mode);
        pixmap = graphics.chartToPixmap(chart);

        if (inlineImageMode) {
          QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());
          document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
          output.append(QString("<br><div class='g' align='center'><img align='center' "
                                "alt='GRAPH' src='%3'></div><br>")
                            .arg(name));

      } else {
        QString imageName = generateUniqueFileName(suffix);
        QString imageAbsoluteName = imageName;
        imageName.prepend(encodedRelativeImagePath + "/");
        imageAbsoluteName.prepend(absoluteImagePath + "/");

        if (!PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName)) {
            die();
        }

        output.append(QString("<br><div align='center' class='g'><img align='center' "
                              "width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth)
                          .arg(graphicsHeight)
                          .arg(imageName));
        if (progress != nullptr) {
          progress->setValue(++progressValue);
        }
      }
    }
  }

  if (listOfPresets.contains(PCRPRESETS::PCRPRESET_C)) {
    output.append("<h4>Utilisation des crédits</h4>");
    output.append(tables.getPCRUtilisation(selectedNode, mode));

    if (includeGraphics) {
        chart = graphics.getPCRUtilisation(selectedNode, mode);
        pixmap = graphics.chartToPixmap(chart);

        if (inlineImageMode) {
          QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());
          document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
          output.append(QString("<br><div class='g' align='center'><img align='center' "
                                "alt='GRAPH' src='%3'></div><br>")
                            .arg(name));

      } else {
        QString imageName = generateUniqueFileName(suffix);
        QString imageAbsoluteName = imageName;
        imageName.prepend(encodedRelativeImagePath + "/");
        imageAbsoluteName.prepend(absoluteImagePath + "/");

        if (!PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName)) {
            die();
        }

        output.append(QString("<br><div align='center' class='g'><img align='center' "
                              "width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth)
                          .arg(graphicsHeight)
                          .arg(imageName));
        if (progress != nullptr) {
          progress->setValue(++progressValue);
        }
      }
    }
  }

  if (listOfPresets.contains(PCRPRESETS::PCRPRESET_D)) {
    output.append("<h4>Détection des cycles et des écarts</h4>");
    output.append(tables.getPCRCycles(selectedNode, mode));

    if (includeGraphics) {
        chart = graphics.getPCRCycles(selectedNode, mode);
        pixmap = graphics.chartToPixmap(chart);

        if (inlineImageMode) {
          QString name = "mydata://" + QString::number(QRandomGenerator::global()->generate());
          document->addResource(QTextDocument::ImageResource, QUrl(name), QVariant(pixmap));
          output.append(QString("<br><div class='g' align='center'><img align='center' "
                                "alt='GRAPH' src='%3'></div><br>")
                            .arg(name));

      } else {
        QString imageName = generateUniqueFileName(suffix);
        QString imageAbsoluteName = imageName;
        imageName.prepend(encodedRelativeImagePath + "/");
        imageAbsoluteName.prepend(absoluteImagePath + "/");

        if (!PCx_Graphics::savePixmapToDisk(pixmap, imageAbsoluteName)) {
            die();
        }

        output.append(QString("<br><div align='center' class='g'><img align='center' "
                              "width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth)
                          .arg(graphicsHeight)
                          .arg(imageName));
        if (progress != nullptr) {
          progress->setValue(++progressValue);
        }
      }
    }
  }

  output.append("\n</div>\n");
  return output;
}

QString PCx_Report::getCSS() {
    QString css = "\nbody{font-family:sans-serif;font-size:9pt;background-color:"
                  "white;color:black;}"
                  "\nh1{color:#A00;}"
                  "\nh2{color:navy;}"
                  "\nh3{color:green;font-size:larger}"
                  "\n.auditNodeContainer{display:flex;flex-wrap:wrap}"
                  "\n.reportingNodeContainer{display:flex;flex-wrap:wrap}";

    css.append(PCx_Query::getCSS());
    css.append(PCx_Tables::getCSS());
    css.append(PCx_Graphics::getCSS());
    return css;
}

QString PCx_Report::generateHTMLHeader() {
    return QString("<!DOCTYPE html>\n<html>\n<head><title>Rapport ProtonClassicSuite"
                   "</title>\n<meta http-equiv='Content-Type' "
                   "content='text/html;charset=utf-8'>\n<style "
                   "type='text/css'>\n%2\n</style>\n</head>\n<body>")
        .arg(getCSS());
}

QString PCx_Report::generateHTMLTOC(QList<unsigned int> nodes) const {
  QString output = "<ul>\n";
  PCx_Tree *tree = nullptr;
  if (auditModel != nullptr) {
    tree = auditModel->getAttachedTree();
  } else if (reportingModel != nullptr) {
    tree = reportingModel->getAttachedTree();
  }

  if (tree == nullptr) {
    qWarning() << "Invalid model used";
    return QString();
  }

  foreach (unsigned int node, nodes) {
    output.append(
        QString("<li><a href='#node%1'>%2</a></li>\n").arg(node).arg(tree->getNodeName(node).toHtmlEscaped()));
  }
  output.append("</ul>\n");
  return output;
}
