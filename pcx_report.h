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

#ifndef PCX_REPORT_H
#define PCX_REPORT_H

#include "pcx_audit.h"
#include "pcx_graphics.h"
#include "pcx_reporting.h"
#include "pcx_tables.h"

class PCx_Report {
public:
  // Presets for PCR Reports, each of them except the last one contains tables
  // and graphics
  enum class PCRPRESETS { PCRPRESET_A, PCRPRESET_B, PCRPRESET_C, PCRPRESET_D, PCRPRESET_S };

  explicit PCx_Report(PCx_Audit *auditModel, int graphicsWidth = 650, int graphicsHeight = 400);

  explicit PCx_Report(PCx_Reporting *reportingModel, int graphicsWidth = 650, int graphicsHeight = 400);

  QString generateHTMLAuditReportForNode(const QList<PCx_Tables::PCAPRESETS> &listOfTabs,
                                         const QList<PCx_Tables::PCATABLES> &listOfTables,
                                         const QList<PCx_Graphics::PCAGRAPHICS> &listOfGraphics, unsigned int selectedNode,
                                         MODES::DFRFDIRI mode, unsigned int referenceNode = 1,
                                         QTextDocument *document = nullptr, QProgressDialog *progress = nullptr,
                                         const PCx_PrevisionItem *prevItem = nullptr);

  QString generateHTMLReportingReportForNode(QList<PCx_Report::PCRPRESETS> listOfPresets, unsigned int selectedNode,
                                             MODES::DFRFDIRI mode, bool includeGraphics = true,
                                             QTextDocument *document = nullptr, QProgressDialog *progress = nullptr);

  QString generateHTMLTOC(QList<unsigned int> nodes) const;


  /**
   * @brief generateSVGTOC generates a graphviz svg clickable tree to insert in html file as a table of content
   * The tree contains all nodes, nodes in nodeToFileName are not grayed and will contain an href
   * @param nodeToFileName a map to a node id and an href
   * @return the svg
   */
  QString generateSVGTOC(QMap<unsigned int,QUrl> nodeToFileName) const;

  const PCx_Tables &getTables() const { return tables; }
  PCx_Graphics &getGraphics() { return graphics; }

  /**
   * @brief generateMainHTMLHeader gets the header of an html document, with css
   * @return the HTML header including the opening "<body>"
   */
  static QString generateMainHTMLHeader();

  /**
   * @brief generateNodeHTMLHeader gets the header of an html document specific for a node, with css
   * @return the HTML header including the opening "<body>"
   */
  QString generateNodeHTMLHeader(unsigned int node) const;

  /**
   * @brief getCSS gets the css to display an reporting in HTML
   * @return a string with CSS rules
   */
  static QString getCSS();

private:
  PCx_Audit *auditModel;
  PCx_Reporting *reportingModel;
  PCx_Tables tables;
  PCx_Graphics graphics;

  PCx_Report(const PCx_Report &c);
  PCx_Report &operator=(const PCx_Report &);
};

#endif // PCX_REPORT_H
