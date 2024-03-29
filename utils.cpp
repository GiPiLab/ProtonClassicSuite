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

#include "utils.h"
#include "qfiledialog.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QSharedMemory>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>
#include <QtGlobal>
#include <QtMath>
#include <cstdio>
#ifndef Q_OS_ANDROID
#include <graphviz/gvc.h>
#endif

[[noreturn]] void die(int retcode) {
  QSqlDatabase::database().rollback();
  QSqlDatabase::database().close();

  QApplication::instance()->exit(retcode);
  exit(retcode);
}

void initializeNewDb() {
  QSqlQuery query;

  query.exec("create table if not exists index_arbres(id integer primary key "
             "autoincrement, nom text unique not null, termine integer not "
             "null default 0, le_timestamp text default current_timestamp)");
  if (query.numRowsAffected() == -1) {
    qCritical() << query.lastError();
    die();
  }

  if (!query.exec("create table if not exists index_audits(id integer primary key "
                  "autoincrement, nom text unique not null, id_arbre integer not null, "
                  "annees text not null, termine integer not null default 0, "
                  "le_timestamp text default current_timestamp)")) {
    qCritical() << query.lastError();
    die();
  }
  if (query.numRowsAffected() == -1) {
    qCritical() << query.lastError();
    die();
  }
  if (!query.exec("create table if not exists index_reportings(id integer primary key "
                  "autoincrement, nom text unique not null, id_arbre integer not null, "
                  "le_timestamp text default current_timestamp)")) {
    qCritical() << query.lastError();
    die();
  }
  if (query.numRowsAffected() == -1) {
    qCritical() << query.lastError();
    die();
  }
  if (!query.exec("create table if not exists index_previsions(id integer primary key "
                  "autoincrement, nom text unique not null, id_audit integer not null, "
                  "le_timestamp text default current_timestamp)")) {
    qCritical() << query.lastError();
    die();
  }
  if (query.numRowsAffected() == -1) {
    qCritical() << query.lastError();
    die();
  }
}

bool loadDb(const QString &databaseName) {
  QSqlDatabase db = QSqlDatabase::database();
  if (!databaseName.isNull()) {
    db.setDatabaseName(databaseName);
  }

  if (!db.open()) {
    qCritical() << db.lastError();
    die();
  }

  /*QSqlQuery("PRAGMA journal_mode=OFF");
QSqlQuery("PRAGMA locking_mode = EXCLUSIVE");
QSqlQuery("PRAGMA synchronous=OFF");
QSqlQuery("PRAGMA page_size = 4096");
QSqlQuery("PRAGMA cache_size = 16384");
QSqlQuery("PRAGMA temp_store = MEMORY");
*/
  QSqlQuery("PRAGMA foreign_keys=ON");
  initializeNewDb();

  return true;
}

QString generateUniqueFileName(const QString &suffix) {
  QString uniqueName = QUuid::createUuid().toString();
  uniqueName.chop(1);
  uniqueName = uniqueName.remove(0, 1).append(suffix);
  return uniqueName;
}

QString qTableViewToHtml(QTableView *tableView) {
  const int rowCount = tableView->model()->rowCount();
  const int columnCount = tableView->model()->columnCount();
  QString out = "<table cellpadding='2' align='center' border='1'>\n";

  out.append("<thead><tr>");
  for (int column = 0; column < columnCount; column++) {
    if (!tableView->isColumnHidden(column)) {
      out.append(QString("<th valign='middle'>%1</th>")
                     .arg(tableView->model()->headerData(column, Qt::Horizontal).toString().toHtmlEscaped()));
    }
  }
  out.append("</tr></thead>\n");

  for (int row = 0; row < rowCount; row++) {
    out.append("<tr>");
    for (int column = 0; column < columnCount; column++) {
      QString cellStyle;
      QString align;
      QModelIndex index = tableView->model()->index(row, column);
      if (!tableView->isColumnHidden(column)) {
        QVariant color = tableView->model()->data(index, Qt::ForegroundRole);
        QVariant alignment = tableView->model()->data(index, Qt::TextAlignmentRole);
        if (alignment.isValid()) {
          if (alignment.toInt() == Qt::AlignCenter) {
            cellStyle.append("text-align:center;");
            align = "align='center'";
          }
        }
        if (color.isValid()) {
          QColor textColor = color.value<QColor>();
          cellStyle.append(QString("color:%1;").arg(textColor.name()));
        } else {
          cellStyle.append("color:0;");
        }
        QVariant data = tableView->model()->data(index);
        out.append("<td " + align + " style='" + cellStyle + "'>");
        QString dataString;
        QLocale defaultLocale;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (data.typeId() == QMetaType::QDate) {
#else
        if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QDate) {
#endif
          dataString = defaultLocale.toString(data.toDate(), QLocale::ShortFormat);
        } else {
          if (data.toString().isEmpty()) {
            dataString = "&nbsp;";
          } else {
            dataString = data.toString();
          }
        }
        out.append(dataString.toHtmlEscaped() + "</td>");
      }
    }
    out.append("</tr>\n");
  }
  out.append("</table>\n");
  return out;
}

QString dotToSvg(const QByteArray &dot){

  const char *dotData = dot.constData();

  GVC_t *gvc;
  graph_t *g;

  gvc = gvContext();

  g = agmemread(dotData);

  if (g == nullptr) {
    gvFreeContext(gvc);
    qCritical() << "Error while reading DOT data !";
    return nullptr;
  }

  if (gvLayout(gvc, g, "dot") != 0) {
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    qCritical() << "Error during graph layout !";
    return nullptr;
  }

  char *outputData;
  unsigned int length = 0;

  gvRenderData(gvc, g, "svg", &outputData, &length);

  if (length == 0) {
    gvFreeRenderData(outputData);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    qCritical() << "Error while rendering graph !";
    return nullptr;
  }

  QString out(outputData);

  gvFreeRenderData(outputData);
  gvFreeLayout(gvc, g);
  agclose(g);
  gvFreeContext(gvc);

  //Remove xml header unneeded to embed into html
  out.remove("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
  out.remove("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"");
  out.remove("\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">");

  //Remove fixed width and height
  out.remove(QRegularExpression("width=.*"));
  return out;
}

bool dotToPdf(const QByteArray &dot, const QString &outputFileName) {
  Q_ASSERT(!outputFileName.isEmpty());

#ifndef Q_OS_ANDROID
  QFile output(outputFileName);

  const char *dotData = dot.constData();

  GVC_t *gvc;
  graph_t *g;

  gvc = gvContext();

  g = agmemread(dotData);

  if (g == nullptr) {
    gvFreeContext(gvc);
    qCritical() << "Error while reading DOT data !";
    return false;
  }

  if (gvLayout(gvc, g, "dot") != 0) {
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    qCritical() << "Error during graph layout !";
    return false;
  }

  char *outputData;
  unsigned int length = 0;

  gvRenderData(gvc, g, "pdf", &outputData, &length);

  if (length == 0) {
    gvFreeRenderData(outputData);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    qCritical() << "Error while rendering graph !";
    return false;
  }

  if (!output.open(QIODevice::WriteOnly)) {
    gvFreeRenderData(outputData);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    qCritical() << "Unable to open output file :" << output.errorString();
    return false;
  }
  QDataStream outputStream(&output);
  outputStream.writeBytes(outputData, length);
  output.close();

  gvFreeRenderData(outputData);
  gvFreeLayout(gvc, g);
  agclose(g);
  gvFreeContext(gvc);

#else
  QMessageBox::warning(nullptr, QObject::tr("Non supporté"),
                       QObject::tr("Fonction non supportée sur cette plateforme"));
#endif

  return true;
}

int question(const QString &text, QWidget *parent) {
  QMessageBox messageBox;
  messageBox.setParent(parent);
  messageBox.setWindowIcon(QIcon(":/icons/icons/logo64_64.png"));
  messageBox.setText(text);
  messageBox.setIcon(QMessageBox::Question);
  messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  return messageBox.exec();
}

qint64 yearToMsSinceEpoch(int year) {
  QDateTime dt(QDate(year, 6, 1), QTime(12, 0));

  if (dt.isValid()) {
    return dt.toMSecsSinceEpoch();
  } else
    return 0;
}

namespace MODES {

QString modeToCompactString(DFRFDIRI mode) {
  switch (mode) {
  case DFRFDIRI::DF:
    return QObject::tr("Dép. de fonct.");
  case DFRFDIRI::RF:
    return QObject::tr("Rec. de fonct.");
  case DFRFDIRI::DI:
    return QObject::tr("Dép. d'invest.");
  case DFRFDIRI::RI:
    return QObject::tr("Rec. d'invest.");
  default:
    qWarning() << "Invalid mode specified !";
  }
  return QString();
}

QString modeToCompleteString(DFRFDIRI mode) {
  switch (mode) {
  case DFRFDIRI::DF:
    return QObject::tr("Dépenses de fonctionnement");
  case DFRFDIRI::RF:
    return QObject::tr("Recettes de fonctionnement");
  case DFRFDIRI::DI:
    return QObject::tr("Dépenses d'investissement");
  case DFRFDIRI::RI:
    return QObject::tr("Recettes d'investissement");
  default:
    qWarning() << "Invalid mode specified !";
  }
  return QString();
}

QString modeToTableString(DFRFDIRI mode) {
  switch (mode) {
  case DFRFDIRI::DF:
    return "DF";
  case DFRFDIRI::RF:
    return "RF";
  case DFRFDIRI::DI:
    return "DI";
  case DFRFDIRI::RI:
    return "RI";
  default:
    qWarning() << "Invalid mode specified !";
  }
  return QString();
}

QStandardItemModel *getListModelOfDFRFDIRI() {
  QStandardItemModel *model = new QStandardItemModel();
  QStandardItem *item;

  item = new QStandardItem(modeToCompleteString(DFRFDIRI::DF));
  item->setData(static_cast<int>(DFRFDIRI::DF), ModeIdUserRole);
  model->appendRow(item);
  item = new QStandardItem(modeToCompleteString(DFRFDIRI::RF));
  item->setData(static_cast<int>(DFRFDIRI::RF), ModeIdUserRole);
  model->appendRow(item);
  item = new QStandardItem(modeToCompleteString(DFRFDIRI::DI));
  item->setData(static_cast<int>(DFRFDIRI::DI), ModeIdUserRole);
  model->appendRow(item);
  item = new QStandardItem(modeToCompleteString(DFRFDIRI::RI));
  item->setData(static_cast<int>(DFRFDIRI::RI), ModeIdUserRole);
  model->appendRow(item);
  return model;
}

} // namespace MODES

namespace NUMBERSFORMAT {
static FORMATMODE currentFormatMode = FORMATMODE::FORMATMODENORMAL;
static int currentNumDecimals = DEFAULTNUMDECIMALS;

int getNumDecimals() {
  QSettings settings;
  return (settings.value("format/numdecimals", DEFAULTNUMDECIMALS).toInt());
}

FORMATMODE getFormatMode() {
  QSettings settings;
  return static_cast<FORMATMODE>(
      settings.value("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODENORMAL)).toUInt());
}

void updateFormatModeAndDecimals() {
  QSettings settings;
  currentFormatMode = static_cast<FORMATMODE>(
      settings.value("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODENORMAL)).toUInt());
  currentNumDecimals = settings.value("format/numdecimals", DEFAULTNUMDECIMALS).toInt();
}

const QString getFormatModeSuffix() {

  switch (currentFormatMode) {
  case FORMATMODE::FORMATMODENORMAL:
    return {};

  case FORMATMODE::FORMATMODETHOUSANDS:
    return ("k");

  case FORMATMODE::FORMATMODEMILLIONS:
    return ("M");
  }
  return {};
}

qint64 fixedDividedByFormatMode(qint64 num) {
  switch (currentFormatMode) {
  case FORMATMODE::FORMATMODENORMAL:
    break;
  case FORMATMODE::FORMATMODETHOUSANDS:
    num /= 1000;
    break;
  case FORMATMODE::FORMATMODEMILLIONS:
    num /= 1000000;
  }
  return num;
}

QString formatFixedPoint(qint64 num, int decimals, bool forcedUnits) {
  QLocale locale;
  QString out;

  if (num == -MAX_NUM) {
    return QString();
  }
  if (!forcedUnits) {
    num = fixedDividedByFormatMode(num);
    out = getFormatModeSuffix();
  }

  // Forced decimals mode
  if (decimals >= 0) {
    out.prepend(locale.toString(fixedPointToDouble(num), 'f', decimals));
  } else {
    out.prepend(locale.toString(fixedPointToDouble(num), 'f', currentNumDecimals));
  }
  return out;
}

QString formatDouble(double num, int decimals, bool forcedUnits) {
  // Convention
  if (qIsNaN(num)) {
    return "DIV0";
  }

  QLocale locale;
  QString out;

  if (!forcedUnits) {
    out = getFormatModeSuffix();
    switch (currentFormatMode) {
    case FORMATMODE::FORMATMODENORMAL:
      break;
    case FORMATMODE::FORMATMODETHOUSANDS:
      num /= 1000;
      break;
    case FORMATMODE::FORMATMODEMILLIONS:
      num /= 1000000;
    }
  }
  // Forced decimals mode
  if (decimals >= 0) {
    out.prepend(locale.toString(num, 'f', decimals));
  } else {
    out.prepend(locale.toString(num, 'f', currentNumDecimals));
  }

  return out;
}

double doubleToDoubleRoundedByNumbersOfDecimals(double number) {
  if (qIsNaN(number) || qIsInf(number) || !qIsFinite(number)) {
    return number;
  }

  double factor = qPow(10, currentNumDecimals);

  return qRound64(number * factor) / factor;
}

qint64 doubleToFixedPoint(double num) {
  if (num >= 0.0) {
    return static_cast<qint64>(((num + 0.0005) * FIXEDPOINTCOEFF));
  }
  return static_cast<qint64>(((num - 0.0005) * FIXEDPOINTCOEFF));
}

double fixedPointToDouble(qint64 num) {
  return static_cast<double>((static_cast<double>(num) / static_cast<double>(FIXEDPOINTCOEFF)));
}
} // namespace NUMBERSFORMAT

QString chooseHTMLFileNameWithDialog()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    QString fileName =
        fileDialog.getSaveFileName(nullptr, QObject::tr("Enregistrer en HTML"), "", QObject::tr("Fichiers HTML (*.html *.htm)"));
    if (fileName.isEmpty()) {
        return nullptr;
    }
    QFileInfo fi(fileName);
    if (fi.suffix().compare("html", Qt::CaseInsensitive) != 0 && fi.suffix().compare("htm", Qt::CaseInsensitive) != 0) {
        fileName.append(".html");
    }
    fi = QFileInfo(fileName);

    if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
        QMessageBox::critical(nullptr, QObject::tr("Attention"), QObject::tr("Fichier non accessible en écriture"));
            return nullptr;
    }
    return fileName;
}

QString choosePNGFileNameWithDialog()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    QString fileName =
        fileDialog.getSaveFileName(nullptr, QObject::tr("Enregistrer en PNG"), "", QObject::tr("Images PNG (*.png)"));
    if (fileName.isEmpty()) {
        return nullptr;
    }
    QFileInfo fi(fileName);
    if (fi.suffix().compare("png", Qt::CaseInsensitive) != 0)  {
        fileName.append(".png");
    }
    fi = QFileInfo(fileName);

    if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
        QMessageBox::critical(nullptr, QObject::tr("Attention"), QObject::tr("Fichier non accessible en écriture"));
            return nullptr;
    }
    return fileName;
}
