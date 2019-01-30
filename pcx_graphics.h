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

#ifndef PCX_GRAPHICS_H
#define PCX_GRAPHICS_H

#include "QCustomPlot/qcustomplot.h"
#include "pcx_audit.h"
#include "pcx_previsionitem.h"
#include "pcx_reporting.h"
#include <QtCharts/QtCharts>

using namespace QtCharts;

class PCx_Graphics {
public:
  static const int MAXWIDTH = 2048;
  static const int MAXHEIGHT = 2048;
  static const int MINWIDTH = 350;
  static const int MINHEIGHT = 350;
  static const int DEFAULTWIDTH = 650;
  static const int DEFAULTHEIGHT = 400;

  static const unsigned int DEFAULTPENCOLOR1 = 0xff0000;
  static const unsigned int DEFAULTPENCOLOR2 = 0x0000ff;

  static const unsigned int DEFAULTCOLORDFBAR = 0xff0000;
  static const unsigned int DEFAULTCOLORRFBAR = 0x00ff00;
  static const unsigned int DEFAULTCOLORDIBAR = 0x0000ff;
  static const unsigned int DEFAULTCOLORRIBAR = 0xffff00;

  static const int DEFAULTALPHA = 70;

  enum class PCAGRAPHICS {
    PCAG1,
    PCAG2,
    PCAG3,
    PCAG4,
    PCAG5,
    PCAG6,
    PCAG7,
    PCAG8,
    PCAG9,
    PCAHISTORY // Used in PCB but display history in PCA
  };

  enum class PCRGRAPHICS { PCRHISTORY, PCRGPROVENANCE, PCRGVARIATION, PCRGUTILISATION, PCRCYCLES };

  PCx_Graphics(PCx_Audit *auditModel, QCustomPlot *plot = nullptr, int graphicsWidth = DEFAULTWIDTH,
               int graphicsHeight = DEFAULTHEIGHT);
  PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot = nullptr, int graphicsWidth = DEFAULTWIDTH,
               int graphicsHeight = DEFAULTHEIGHT);
  virtual ~PCx_Graphics();

  QString getPCAG1(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::OUVERTS, false, prevItem, referenceNode);
  }
  QString getPCAG3(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::REALISES, false, prevItem, referenceNode);
  }
  QString getPCAG5(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::ENGAGES, false, prevItem, referenceNode);
  }
  QString getPCAG7(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::DISPONIBLES, false, prevItem, referenceNode);
  }

  QString getPCAG2(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::OUVERTS, true, prevItem, referenceNode);
  }
  QString getPCAG4(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::REALISES, true, prevItem, referenceNode);
  }
  QString getPCAG6(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::ENGAGES, true, prevItem, referenceNode);
  }
  QString getPCAG8(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::DISPONIBLES, true, prevItem, referenceNode);
  }

  QString getPCAG9(unsigned int node);

  QString getPCAHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, const QList<PCx_Audit::ORED> &selectedORED,
                        const PCx_PrevisionItem *prevItem = nullptr, bool miniMode = false);

  // History line plot with selected OREDPCR
  QString getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                        const QList<PCx_Reporting::OREDPCR> &selectedOREDPCR);

  QString getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode);
  QString getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode);
  QString getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode);
  QString getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode);

  QCustomPlot *getPlot() const;

  int getGraphicsWidth() const { return graphicsWidth; }
  int getGraphicsHeight() const { return graphicsHeight; }

  void setGraphicsWidth(int width);
  void setGraphicsHeight(int height);

  static QColor getColorPen1();
  static QColor getColorPen2();

  static QColor getColorDFBar();
  static QColor getColorRFBar();
  static QColor getColorDIBar();
  static QColor getColorRIBar();

  static int getAlpha();

  static QString getCSS();

  bool savePlotToDisk(const QString &imageAbsoluteName) const;

  QChart *getPCAHistoryChart(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                             const QList<PCx_Audit::ORED> &selectedORED, const PCx_PrevisionItem *prevItem,
                             bool miniMode);

  bool saveChartToDisk(QChart *chart, const QString &imageAbsoluteName) const;

private:
  PCx_Audit *auditModel;
  PCx_Reporting *reportingModel;

  // Returns the title of the graphic in html
  QString getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule,
                     const PCx_PrevisionItem *prevItem = nullptr, unsigned int referenceNode = 1);

  // Histogram comparing percents of selected OREDs regarding to a reference for
  // the last reporting date
  QString getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                            QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference,
                            const QString &plotTitle, QColor color);
  bool ownPlot;
  int graphicsWidth{}, graphicsHeight{};

  QCustomPlot *plot;

  PCx_Graphics(const PCx_Graphics &c);
  PCx_Graphics &operator=(const PCx_Graphics &);
};

#endif // PCX_GRAPHICS_H
