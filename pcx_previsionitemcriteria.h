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

#ifndef PCX_PREVISIONITEMCRITERIA_H
#define PCX_PREVISIONITEMCRITERIA_H

#include "pcx_audit.h"
#include <QAbstractListModel>
#include <QComboBox>

class PCx_PrevisionItemCriteria {
public:
  enum class PREVISIONOPERATOR {
    MINIMUM,
    MAXIMUM,
    AVERAGE,
    REGLIN,
    LASTVALUE,
    PERCENT,
    FIXEDVALUE,
    FIXEDVALUEFROMPROPORTIONALREPARTITION
  };

  PCx_PrevisionItemCriteria();
  PCx_PrevisionItemCriteria(const QString &serializedCriteria);
  PCx_PrevisionItemCriteria(PREVISIONOPERATOR previsionOperator, PCx_Audit::ORED ored, qint64 previsionOperand = 0);
  ~PCx_PrevisionItemCriteria();

  qint64 getOperand() const { return previsionOperand; }
  PCx_Audit::ORED getOredTarget() const { return previsionOredTarget; }
  PREVISIONOPERATOR getOperator() const { return previsionOperator; }

  qint64 compute(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;

  QString getCriteriaLongDescription() const;

  QString serialize() const;
  bool unserialize(const QString &criteriaString);

  static void fillComboBoxWithOperators(QComboBox *combo);

private:
  qint64 getMinimumOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getMaximumOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getAverageOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getReglinOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getLastValueOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getPercentOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const;
  qint64 getFixedValue() const { return previsionOperand; }

  PREVISIONOPERATOR previsionOperator;
  PCx_Audit::ORED previsionOredTarget;
  qint64 previsionOperand;
};

#endif // PCX_PREVISIONITEMCRITERIA_H
