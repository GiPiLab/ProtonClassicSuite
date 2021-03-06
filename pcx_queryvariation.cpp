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

#include "pcx_queryvariation.h"
#include "utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

using namespace NUMBERSFORMAT;

PCx_QueryVariation::PCx_QueryVariation(PCx_Audit *model, unsigned int queryId) : PCx_Query(model) { load(queryId); }

PCx_QueryVariation::PCx_QueryVariation(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored,
                                       MODES::DFRFDIRI dfrfdiri, INCREASEDECREASE increase, PERCENTORPOINTS percent,
                                       OPERATORS op, qint64 val, int year1, int year2, const QString &name)
    : PCx_Query(model, typeId, ored, dfrfdiri, year1, year2, name), incDec(increase), percentOrPoints(percent), op(op),
      val(val) {
  setYears(year1, year2);
}

bool PCx_QueryVariation::load(unsigned int queryId) {
  QSqlQuery q;
  q.prepare(QString("select * from audit_queries_%1 where id=:qid").arg(model->getAuditId()));
  q.bindValue(":qid", queryId);
  q.exec();

  if (!q.next()) {
    qCritical() << "Invalid PCx_query specified";
    return false;
  } else {
    if (static_cast<PCx_Query::QUERIESTYPES>(q.value("query_mode").toUInt()) != PCx_Query::QUERIESTYPES::VARIATION) {
      qCritical() << "Invalid PCx_query mode !";
      return false;
    }
    name = q.value("name").toString();
    typeId = q.value("target_type").toUInt();
    ored = static_cast<PCx_Audit::ORED>(q.value("ored").toUInt());
    dfrfdiri = static_cast<MODES::DFRFDIRI>(q.value("dfrfdiri").toUInt());
    op = static_cast<OPERATORS>(q.value("oper").toUInt());
    val = q.value("val1").toLongLong();

    percentOrPoints = static_cast<PERCENTORPOINTS>(q.value("percent_or_point").toUInt());
    incDec = static_cast<INCREASEDECREASE>(q.value("increase_decrease").toUInt());
    setYears(q.value("year1").toInt(), q.value("year2").toInt());
  }
  this->queryId = queryId;
  return true;
}

unsigned int PCx_QueryVariation::save(const QString &name) const {
  if (name.isEmpty() || name.size() > MAXOBJECTNAMELENGTH) {
    qFatal("Assertion failed");
  }

  QSqlQuery q;
  q.prepare(QString("insert into audit_queries_%1 "
                    "(name,query_mode,target_type,ored,dfrfdiri,oper,percent_or_point,"
                    "increase_decrease,val1,year1,year2) values "
                    "(:name,:qmode,:type,:ored,:dfrfdiri,:oper,:pop,"
                    ":incdec,:val1,:y1,:y2)")
                .arg(model->getAuditId()));
  q.bindValue(":name", name);
  q.bindValue(":qmode", static_cast<int>(PCx_Query::QUERIESTYPES::VARIATION));
  q.bindValue(":type", typeId);
  q.bindValue(":ored", static_cast<int>(ored));
  q.bindValue(":dfrfdiri", static_cast<int>(dfrfdiri));
  q.bindValue(":oper", static_cast<int>(op));
  q.bindValue(":pop", static_cast<int>(percentOrPoints));
  q.bindValue(":incdec", static_cast<int>(incDec));
  q.bindValue(":val1", val);
  q.bindValue(":y1", year1);
  q.bindValue(":y2", year2);
  q.exec();

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    return 0;
  }

  return q.lastInsertId().toUInt();
}

bool PCx_QueryVariation::canSave(const QString &name) const {
  if (name.isEmpty()) {
    return false;
  }

  QSqlQuery q;
  q.prepare(QString("select * from audit_queries_%1 where name=:name and "
                    "query_mode=:qmode")
                .arg(model->getAuditId()));
  q.bindValue(":name", name);
  q.bindValue(":qmode", static_cast<int>(PCx_Query::QUERIESTYPES::VARIATION));
  q.exec();

  if (q.next()) {
    return false;
  }

  return true;
}

QString PCx_QueryVariation::getDescription() const {
  QString out;
  if (typeId == PCx_Query::ALLTYPES) {
    out = QObject::tr("Tous les noeuds");
  } else {
    out = QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTree()->idTypeToName(typeId).toHtmlEscaped());
  }

  out.append(QObject::tr(" dont les %1 des %2 ont connu une %3 %4 %5%6 entre %7 et %8")
                 .arg(PCx_Audit::OREDtoCompleteString(ored, true).toHtmlEscaped(),
                      MODES::modeToCompleteString(dfrfdiri).toLower().toHtmlEscaped(),
                      incDecToString(incDec).toHtmlEscaped(), operatorToString(op).toHtmlEscaped(),
                      formatFixedPoint(val, -1, true), percentOrPointToString(percentOrPoints).toHtmlEscaped())
                 .arg(year1)
                 .arg(year2));
  return out;
}

QString PCx_QueryVariation::exec(QXlsx::Document *xlsDoc) const {
  if (!(year1 > 0 && year2 > 0 && year1 < year2)) {
    qFatal("Assertion failed");
  }
  // QElapsedTimer timer;
  // timer.start();
  QMap<unsigned int, qint64> valuesForYear1, valuesForYear2;
  QMap<unsigned int, qint64> variations, matchingNodes;
  QList<unsigned int> nodesOfThisType, problemNodes;

  QSqlQuery q;
  QString oredString = PCx_Audit::OREDtoTableString(ored);

  if (typeId != ALLTYPES) {
    q.prepare(QString("select id from arbre_%1 where type=:type").arg(model->getAttachedTree()->getTreeId()));
    q.bindValue(":type", typeId);
    q.exec();

    while (q.next()) {
      nodesOfThisType.append(q.value("id").toUInt());
    }
  }

  // qDebug()<<"Nodes of type "<<typeId<<nodesOfThisType;

  q.prepare(QString("select id_node,annee,%3 from audit_%1_%2 where "
                    "annee=:year1 or annee=:year2")
                .arg(MODES::modeToTableString(dfrfdiri))
                .arg(model->getAuditId())
                .arg(oredString));
  q.bindValue(":year1", year1);
  q.bindValue(":year2", year2);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    unsigned int node = q.value("id_node").toUInt();

    if (typeId != ALLTYPES && !nodesOfThisType.contains(node)) {
      continue;
    }

    if (q.value("annee").toInt() == year1) {
      valuesForYear1.insert(node, q.value(oredString).toLongLong());
    } else if (q.value("annee").toInt() == year2) {
      valuesForYear2.insert(node, q.value(oredString).toLongLong());
    } else {
      Q_UNREACHABLE();
      die();
    }
  }

  QMapIterator<unsigned int, qint64> i(valuesForYear1);
  while (i.hasNext()) {
    i.next();
    qint64 val1 = i.value();
    qint64 val2 = valuesForYear2.value(i.key());
    qint64 variation;

    if (val1 != 0) {
      if (percentOrPoints == PERCENTORPOINTS::PERCENT) {
        // Convert to fixed point percents with two decimals
        variation = static_cast<qint64>(10000 * ((static_cast<double>(val2) - val1) / val1));
        //        qDebug()<<"val1 = "<<val1<<"val2 = "<<val2<<"variation =
        //        "<<variation;
      } else {
        variation = val2 - val1;
      }
      variations.insert(i.key(), variation);
    } else {
      problemNodes.append(i.key());
    }
  }

  OPERATORS trueOp = op;

  // qDebug()<<variations;
  QMapIterator<unsigned int, qint64> j(variations);
  while (j.hasNext()) {
    j.next();
    qint64 nodeVariation = j.value();

    if (incDec == INCREASEDECREASE::INCREASE) {
      if (nodeVariation < 0) {
        continue;
      }
    } else if (incDec == INCREASEDECREASE::DECREASE) {
      if (nodeVariation > 0) {
        continue;
      }
      nodeVariation = qAbs(nodeVariation);
    }

    qint64 trueVal = val;

    if (percentOrPoints == PERCENTORPOINTS::PERCENT) {
      // Only compare with two decimals
      if (FIXEDPOINTCOEFF != 100) {
        trueVal /= (FIXEDPOINTCOEFF / 100);
      }
    }

    //        qDebug()<<"Comparing "<<nodeVariation<<" with "<<trueVal;

    switch (trueOp) {
    case OPERATORS::LOWERTHAN:
      if (nodeVariation < trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;

    case OPERATORS::GREATERTHAN:
      if (nodeVariation > trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;

    case OPERATORS::LOWEROREQUAL:
      if (nodeVariation <= trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;

    case OPERATORS::GREATEROREQUAL:
      if (nodeVariation >= trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;

    case OPERATORS::EQUAL:
      if (nodeVariation == trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;

    case OPERATORS::NOTEQUAL:
      if (nodeVariation != trueVal) {
        matchingNodes.insert(j.key(), j.value());
      }
      break;
    }
  }

  QString output = QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());
  output.append("<p><i>" + getDescription() + "</i></p>");

  if (xlsDoc != nullptr) {
    QString sheetName = "ReqVar_" + name;
    if (xlsDoc->sheetNames().contains(sheetName)) {
      sheetName = "ReqVar_" + name + "_" + generateUniqueFileName();
    }
    xlsDoc->addSheet(sheetName);
    xlsDoc->selectSheet(sheetName);
    xlsDoc->write(1, 2, QString("Requête %1").arg(name));
    xlsDoc->write(2, 2, getDescription());
    xlsDoc->write(4, 2, "Type");
    xlsDoc->write(4, 3, "Noeud");
    xlsDoc->write(4, 4, year1);
    xlsDoc->write(4, 5, year2);
    xlsDoc->write(4, 6, QString("%1 (en %2)").arg(incDecToString(incDec), percentOrPointToString(percentOrPoints)));
  }

  output.append(QString("<table class='req1' align='center' cellpadding='5' "
                        "style='margin-left:auto;margin-right:auto'>"
                        "<tr><th>&nbsp;</th><th>%1</th><th>%2</th><th>%3</th>")
                    .arg(year1)
                    .arg(year2)
                    .arg(incDecToString(incDec).toHtmlEscaped()));

  QMapIterator<unsigned int, qint64> matchIter(matchingNodes);
  int currentRow = 5;
  while (matchIter.hasNext()) {
    matchIter.next();
    unsigned int node = matchIter.key();
    qint64 val = matchIter.value();

    if (percentOrPoints == PERCENTORPOINTS::PERCENT) {
      // Add the last decimal to fit FIXEDPOINTCOEFF
      val = val * 10;
    }

    if (incDec != INCREASEDECREASE::VARIATION) {
      val = qAbs(val);
    }
    output.append(QString("<tr><td>%1</td><td align='right'>%2</td><td "
                          "align='right'>%3</td><td align='right'>%4 %5</td></tr>")
                      .arg(model->getAttachedTree()->getNodeName(node).toHtmlEscaped(),
                           formatFixedPoint(valuesForYear1.value(node)), formatFixedPoint(valuesForYear2.value(node)),
                           formatFixedPoint(val, -1, true), percentOrPointToString(percentOrPoints).toHtmlEscaped()));
    if (xlsDoc != nullptr) {
      QPair<QString, QString> typeAndNodeName = model->getAttachedTree()->getTypeNameAndNodeName(node);
      xlsDoc->write(currentRow, 2, typeAndNodeName.first);
      xlsDoc->write(currentRow, 3, typeAndNodeName.second);
      xlsDoc->write(currentRow, 4, fixedPointToDouble(valuesForYear1.value(node)));
      xlsDoc->write(currentRow, 5, fixedPointToDouble(valuesForYear2.value(node)));
      xlsDoc->write(currentRow, 6, fixedPointToDouble(val));
    }
    currentRow++;
  }

  if (!problemNodes.isEmpty()) {
    output.append(QString("<tr><td colspan='4'><i>Non encore pourvus en %1</i></td></tr>").arg(year1));
    if (xlsDoc != nullptr) {
      currentRow += 2;
      xlsDoc->write(currentRow, 2, QString("Non encore pourvus en %1").arg(year1));
      currentRow++;
    }
  }
  foreach (unsigned int probNode, problemNodes) {
    output.append(QString("<tr><td><i>%1</i></td><td><i>%2</i></td><td><i>%3</i></td><td "
                          "align='right'><i>NA</i></td></tr>")
                      .arg(model->getAttachedTree()->getNodeName(probNode).toHtmlEscaped(),
                           formatFixedPoint(valuesForYear1.value(probNode)),
                           formatFixedPoint(valuesForYear2.value(probNode))));
    if (xlsDoc != nullptr) {
      QPair<QString, QString> typeAndNodeName = model->getAttachedTree()->getTypeNameAndNodeName(probNode);
      xlsDoc->write(currentRow, 2, typeAndNodeName.first);
      xlsDoc->write(currentRow, 3, typeAndNodeName.second);
      xlsDoc->write(currentRow, 4, fixedPointToDouble(valuesForYear1.value(probNode)));
      xlsDoc->write(currentRow, 5, fixedPointToDouble(valuesForYear2.value(probNode)));
    }
    currentRow++;
  }

  output.append("</table>");
  // qDebug()<<"ReqVar computed in"<<timer.elapsed()<<"ms";
  return output;
}

const QString PCx_QueryVariation::operatorToString(OPERATORS op) {
  QString output;
  switch (op) {
  case OPERATORS::LOWERTHAN:
    output = QObject::tr("strictement inférieure à");
    break;
  case OPERATORS::LOWEROREQUAL:
    output = QObject::tr("inférieure ou égale à");
    break;
  case OPERATORS::EQUAL:
    output = QObject::tr("égale à");
    break;
  case OPERATORS::NOTEQUAL:
    output = QObject::tr("différente de");
    break;
  case OPERATORS::GREATEROREQUAL:
    output = QObject::tr("supérieure ou égale à");
    break;
  case OPERATORS::GREATERTHAN:
    output = QObject::tr("strictement supérieure à");
    break;
  }
  return output;
}

const QString PCx_QueryVariation::incDecToString(INCREASEDECREASE incDec) {
  QString output;
  switch (incDec) {
  case INCREASEDECREASE::INCREASE:
    output = QObject::tr("augmentation");
    break;
  case INCREASEDECREASE::DECREASE:
    output = QObject::tr("diminution");
    break;
  case INCREASEDECREASE::VARIATION:
    output = QObject::tr("variation");
    break;
  }
  return output;
}

const QString PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::PERCENTORPOINTS pop) {
  QString out;
  switch (pop) {
  case PERCENTORPOINTS::PERCENT:
    out = QObject::tr("%");
    break;

  case PERCENTORPOINTS::POINTS:
    out = QObject::tr("€");
    break;
  }
  return out;
}

void PCx_QueryVariation::setYears(int year1, int year2) {
  // This query needs at least two years, this is checked in ui
  if (year1 == year2) {
    qFatal("Assertion failed");
  }
  PCx_Query::setYears(year1, year2);
}

QString PCx_QueryVariation::getCSS() {
  QColor color = getColor();
  QString css = QString("\n.req1{background-color:%1;padding:5px;}").arg(color.name());
  return css;
}

QColor PCx_QueryVariation::getColor() {
  QSettings settings;
  unsigned int oldcolor = settings.value("queries/penvar", PCx_QueryVariation::DEFAULTCOlOR).toUInt();
  QColor color(oldcolor);
  return color;
}
