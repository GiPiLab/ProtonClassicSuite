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

#ifndef PCX_QUERYVARIATION_H
#define PCX_QUERYVARIATION_H

#include "pcx_audit.h"
#include "pcx_query.h"

class PCx_QueryVariation : public PCx_Query {

public:
  static const unsigned int DEFAULTCOlOR = 0xffaabb;

  enum class OPERATORS { LOWERTHAN, LOWEROREQUAL, EQUAL, NOTEQUAL, GREATEROREQUAL, GREATERTHAN };

  enum class PERCENTORPOINTS { PERCENT, POINTS };

  enum class INCREASEDECREASE { INCREASE, DECREASE, VARIATION };

  PCx_QueryVariation(PCx_Audit *model, unsigned int queryId);

  PCx_QueryVariation(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri,
                     INCREASEDECREASE incDec, PERCENTORPOINTS percentOrPoints, OPERATORS op, qint64 val, int year1,
                     int year2, const QString &name = "");

  bool load(unsigned int queryId);

  unsigned int save(const QString &name) const;

  bool canSave(const QString &name) const;

  QString getDescription() const;

  QString exec(QXlsx::Document *xlsDoc = nullptr) const;

  void setOperator(OPERATORS op) { this->op = op; }
  OPERATORS getOperator() const { return op; }

  static const QString operatorToString(OPERATORS op);
  // static OPERATORS stringToOperator(const QString &s);

  INCREASEDECREASE getIncDec() const { return incDec; }
  void setIncDec(INCREASEDECREASE incDec) { this->incDec = incDec; }
  static const QString incDecToString(INCREASEDECREASE incDec);

  PERCENTORPOINTS getPercentOrPoints() const { return percentOrPoints; }
  void setPercentOrPoints(PERCENTORPOINTS pop) { percentOrPoints = pop; }
  static const QString percentOrPointToString(PERCENTORPOINTS pop);

  qint64 getValue() const { return val; }
  void setValue(qint64 val) { this->val = val; }

  // Forbid same years for this query
  void setYears(int year1, int year2);

  static QString getCSS();
  static QColor getColor();

private:
  INCREASEDECREASE incDec;
  PERCENTORPOINTS percentOrPoints;
  OPERATORS op;
  qint64 val{};

  PCx_QueryVariation(const PCx_QueryVariation &c);
  PCx_QueryVariation &operator=(const PCx_QueryVariation &);
};
#endif // PCX_QUERYVARIATION_H
