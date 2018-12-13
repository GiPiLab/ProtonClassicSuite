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

#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include "pcx_audit.h"
#include "pcx_reporting.h"

class PCx_Tables {
public:
  enum class PCAPRESETS { PCARESULTS, PCAOVERVIEW, PCAEVOLUTION, PCAEVOLUTIONCUMUL, PCABASE100, PCADAYOFWORK };

  enum class PCATABLES {
    PCARAWDATA,
    PCAT1,
    PCAT2,
    PCAT2BIS,
    PCAT3,
    PCAT3BIS,
    PCAT4,
    PCAT5,
    PCAT6,
    PCAT7,
    PCAT8,
    PCAT9,
    PCAT10,
    PCAT11,
    PCAT12
  };

  enum class PCRTABLES {
    PCRRATIOPARENTS,
    PCRPROVENANCE,
    PCRVARIATION,
    PCRUTILISATION,
    PCRCYCLES,
    PCRRFDF,
    PCRRIDI,
    PCRRFDFRIDI
  };

  PCx_Tables(PCx_Audit *auditModel);
  PCx_Tables(PCx_Reporting *reportingModel);

  QString getPCAPresetDayOfWork(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCAPresetBase100(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAPresetEvolutionCumul(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAPresetEvolution(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAPresetOverview(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAPresetResults(unsigned int node) const;

  static QString getCSS();

  QString getPCARawData(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "OVERVIEW"
  QString getPCAT1(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCAT4(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAT8(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "EVOLUTION"
  QString getPCAT2bis(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAT3bis(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "EVOLUTIONCUMUL"
  QString getPCAT2(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAT3(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "BASE100"
  QString getPCAT5(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode = 1) const;
  QString getPCAT6(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "DAYOFWORK"
  QString getPCAT7(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCAT9(unsigned int node, MODES::DFRFDIRI mode) const;

  // Content of preset "RESULTS"
  QString getPCAT10(unsigned int node) const;
  QString getPCAT11(unsigned int node) const;
  QString getPCAT12(unsigned int node) const;

  QString getPCRRatioParents(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCRProvenance(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCRVariation(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCRUtilisation(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCRCycles(unsigned int node, MODES::DFRFDIRI mode) const;
  QString getPCRRFDF(unsigned int node) const;
  QString getPCRRIDI(unsigned int node) const;
  QString getPCRRFDFRIDI(unsigned int node) const;

private:
  PCx_Audit *auditModel;
  PCx_Reporting *reportingModel;

  PCx_Tables(const PCx_Tables &c);
  PCx_Tables &operator=(const PCx_Tables &);
};

#endif // PCX_TABLES_H
