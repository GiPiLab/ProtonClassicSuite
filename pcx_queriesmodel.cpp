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

#include "pcx_queriesmodel.h"
#include "pcx_query.h"
#include "pcx_queryminmax.h"
#include "pcx_queryrank.h"
#include "pcx_queryvariation.h"
#include <QBrush>
#include <QDebug>
#include <QSqlField>
#include <QSqlRecord>

PCx_QueriesModel::PCx_QueriesModel(QObject *parent) : QSqlQueryModel(parent) {
  auditId = 0;
  auditModel = nullptr;
}

PCx_QueriesModel::PCx_QueriesModel(PCx_Audit *auditModel, QObject *parent)
    : QSqlQueryModel(parent), auditModel(auditModel) {
  auditId = auditModel->getAuditId();
  update();
}

QVariant PCx_QueriesModel::data(const QModelIndex &item, int role) const {
  if (!item.isValid())
    return QVariant();

  if (role == Qt::BackgroundRole) {
    PCx_Query::QUERIESTYPES queryType =
        static_cast<PCx_Query::QUERIESTYPES>(record(item.row()).value("query_mode").toUInt());

    switch (queryType) {
    case PCx_Query::QUERIESTYPES::VARIATION:
      return QVariant(QBrush(PCx_QueryVariation::getColor()));
    case PCx_Query::QUERIESTYPES::RANK:
      return QVariant(QBrush(PCx_QueryRank::getColor()));
    case PCx_Query::QUERIESTYPES::MINMAX:
      return QVariant(QBrush(PCx_QueryMinMax::getColor()));
    }
    Q_UNREACHABLE();
    return QSqlQueryModel::data(item, role);

  } else if (role == Qt::ToolTipRole) {
    PCx_Query::QUERIESTYPES queryType =
        static_cast<PCx_Query::QUERIESTYPES>(record(item.row()).value("query_mode").toUInt());
    unsigned int queryId = record(item.row()).value("id").toUInt();

    switch (queryType) {
    case PCx_Query::QUERIESTYPES::VARIATION: {
      PCx_QueryVariation q(auditModel, queryId);
      return q.getDescription().toHtmlEscaped();
    }
    case PCx_Query::QUERIESTYPES::RANK: {
      PCx_QueryRank q(auditModel, queryId);
      return q.getDescription().toHtmlEscaped();
    }

    case PCx_Query::QUERIESTYPES::MINMAX: {
      PCx_QueryMinMax q(auditModel, queryId);
      return q.getDescription().toHtmlEscaped();
    }
    }
    qDebug() << "Unsupported query type";
    return QVariant();

  } else
    return QSqlQueryModel::data(item, role);
}

void PCx_QueriesModel::update() {
  if (auditId == 0) {
    qFatal("Assertion failed");
  }
  setQuery(QString("select name,id,query_mode from audit_queries_%1").arg(auditId));
}
