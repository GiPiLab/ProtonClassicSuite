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

#ifndef PCX_QUERYMINMAX_H
#define PCX_QUERYMINMAX_H

#include "pcx_query.h"

class PCx_QueryMinMax : public PCx_Query {
public:
  static const unsigned int DEFAULTCOlOR = 0xaabbff;

  PCx_QueryMinMax(PCx_Audit *model, unsigned int queryId);

  PCx_QueryMinMax(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri, qint64 val1,
                  qint64 val2, int year1, int year2, const QString &name = "");

  QPair<qint64, qint64> getVals() const { return QPair<qint64, qint64>(val1, val2); }

  void setVals(QPair<qint64, qint64> vals);

  unsigned int save(const QString &name) const;
  QString exec(QXlsx::Document *xlsDoc = nullptr) const;
  bool load(unsigned int queryId);
  bool canSave(const QString &name) const;
  QString getDescription() const;

  static QString getCSS();
  static QColor getColor();

private:
  PCx_QueryMinMax(const PCx_QueryMinMax &c);
  PCx_QueryMinMax &operator=(const PCx_QueryMinMax &);

  qint64 val1{}, val2{};
};

#endif // PCX_QUERYMINMAX_H
