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
#include <QStandardItemModel>
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
  static const unsigned int DEFAULTCOLORDIBAR = 0xaa0000;
  static const unsigned int DEFAULTCOLORRIBAR = 0x00aa00;

  static const int DEFAULTALPHA = 200;

  static const int GraphicIdUserRole = Qt::UserRole + 240;

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

  static const QMap<PCAGRAPHICS, QString> pcaGraphicsDescription();

  enum class PCRGRAPHICS { PCRHISTORY, PCRGPROVENANCE, PCRGVARIATION, PCRGUTILISATION, PCRCYCLES };

  static const QMap<PCRGRAPHICS, QString> pcrGraphicsDescription();

  enum class SETTINGKEY {
    PENCOLOR1,
    PENCOLOR2,
    DFBARCOLOR,
    RFBARCOLOR,
    DIBARCOLOR,
    RIBARCOLOR,
    ALPHA,
    WIDTH,
    HEIGHT,
    SHOWPOINTLABELS
  };

  static const QMap<SETTINGKEY, QString> settingKey();

  PCx_Graphics(PCx_Audit *auditModel, QCustomPlot *plot = nullptr, int graphicsWidth = DEFAULTWIDTH,
               int graphicsHeight = DEFAULTHEIGHT);
  PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot = nullptr, int graphicsWidth = DEFAULTWIDTH,
               int graphicsHeight = DEFAULTHEIGHT);
  virtual ~PCx_Graphics();

  QChart *getPCAG1(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::OUVERTS, false, prevItem, referenceNode);
  }

  QChart *getPCAG3(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::REALISES, false, prevItem, referenceNode);
  }
  QChart *getPCAG5(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::ENGAGES, false, prevItem, referenceNode);
  }
  QChart *getPCAG7(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::DISPONIBLES, false, prevItem, referenceNode);
  }

  QChart *getPCAG2(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::OUVERTS, true, prevItem, referenceNode);
  }
  QChart *getPCAG4(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::REALISES, true, prevItem, referenceNode);
  }
  QChart *getPCAG6(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::ENGAGES, true, prevItem, referenceNode);
  }
  QChart *getPCAG8(unsigned int node, MODES::DFRFDIRI mode, const PCx_PrevisionItem *prevItem = nullptr,
                   unsigned int referenceNode = 1) const {
    return getPCAG1G8(node, mode, PCx_Audit::ORED::DISPONIBLES, true, prevItem, referenceNode);
  }

  QChart *getPCAG9(unsigned int node, int year) const;

  QChart *getPCAHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, const QList<PCx_Audit::ORED> &selectedORED,
                        const PCx_PrevisionItem *prevItem, bool miniMode) const;

  QChart *getChartFromPCAGRAPHICS(PCAGRAPHICS pcaGraphic, unsigned int node, MODES::DFRFDIRI mode,
                                  const PCx_PrevisionItem *prevItem = nullptr, unsigned int referenceNode = 1,
                                  int year = -1) const;

  // History line plot with selected OREDPCR
  QString getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                        const QList<PCx_Reporting::OREDPCR> &selectedOREDPCR);

  QChart *getPCRHistoryChart(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                             const QList<PCx_Reporting::OREDPCR> &selectedOREDPCR) const;

  QChart *getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode);
  QChart *getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode);
  QChart *getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode);
  QChart *getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode);

  QCustomPlot *getPlot() const;

  static QVariant getSettingValue(SETTINGKEY key);

  void setGraphicsWidth(int width);
  void setGraphicsHeight(int height);

  static QString getCSS();

  /**
   * @brief chartToPixmap renders a chart to a pixmap. The chart must not be displayed into a chartview
   * @param chart the chart to render. Its ownership is taken and it will be destroyed after return
   * @return the pixmap of "graphicsWidth" and "graphicsHeight" size
   */
  QPixmap chartToPixmap(QChart *chart) const;

  /**
   * @brief chartViewToPixmap renders a displayed chartView to a pixmap
   * @param chartView the chartView to render. Ownership is not taken
   * @return the pixmap
   */
  static QPixmap chartViewToPixmap(QChartView *chartView);

  bool savePlotToDisk(const QString &imageAbsoluteName) const;

  static bool savePixmapToDisk(const QPixmap &pixmap, const QString &imageAbsoluteName);

  static QStandardItemModel *getListModelOfAvailablePCAGRAPHICS();

private:
  PCx_Audit *auditModel;
  PCx_Reporting *reportingModel;

  QChart *getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule,
                     const PCx_PrevisionItem *prevItem, unsigned int referenceNode) const;

  // Histogram comparing percents of selected OREDs regarding to a reference for
  // the last reporting date
  QString getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                            QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference,
                            const QString &plotTitle, QColor color);

  QChart *getPCRPercentBarsChart(unsigned int selectedNodeId, MODES::DFRFDIRI mode,
                                 QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference,
                                 const QString &plotTitle, QColor color);

  bool ownPlot;

  QCustomPlot *plot;

  PCx_Graphics(const PCx_Graphics &c);
  PCx_Graphics &operator=(const PCx_Graphics &);
};

#endif // PCX_GRAPHICS_H
