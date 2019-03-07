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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTableView>

#define VERSION "1.1.0"

#define MAXOBJECTNAMELENGTH                                                                                            \
  64 ///< Maximum length of audits, reportings, trees, queries and previsions
     ///< names

#define FIXEDPOINTCOEFF 1000 ///< Currencies are stored as fixed point, multiplied with this coeff
#define DEFAULTNUMDECIMALS                                                                                             \
  2 ///< The default number of decimals to display. Must not be greater than
    ///< log10(FIXEDPOINTCOEFF)
#define MAX_NUM                                                                                                        \
  Q_INT64_C(4294967295) ///< Maximum acceptable number. Will be stored
                        ///< multiplied by FIXEDPOINTCOEFF into an int64. Must
                        ///< be < MAX_DBL to fit in QDoubleSpinBox
#define MAX_RANDOM_NUM_FOR_A_LEAF 90000
// For mingw
#ifndef INT64_MAX
#if __WORDSIZE == 64
#define __INT64_C(c) c##L
#else
#define __INT64_C(c) c##LL
#endif
#define INT64_MAX (__INT64_C(9223372036854775807))
#endif

/**
 * @brief qTableViewToHtml converts a QTableView to HTML
 * @param tableView the view to display in HTML
 * @return an HTML table of the view
 */
QString qTableViewToHtml(QTableView *tableView);

/**
 * @brief die exits the program properly by issuing a rollback on the last
 * non-committed transaction, closing the db
 * @param retcode the return value
 */
[[noreturn]] void die(int retcode = EXIT_FAILURE);

/**
 * @brief loadDb opens and load the SQLITE3 database file
 * @param databaseName the name of the database file
 * @return true on success
 */
bool loadDb(const QString &databaseName);

/**
 * @brief initializeNewDb creates index tables for trees and audits on a new
 * database
 */
void initializeNewDb(void);

/**
 * @brief generateUniqueFileName generates an unique, UUID based file name, with
 * an optional extension
 * @param suffix the optional file extension that will be append to the
 * generated file name
 * @return an unique name based on UUID
 */
QString generateUniqueFileName(const QString &suffix = 0);

/**
 * @brief dotToPdf renders a graph in DOT format to a pdf file
 * @param dot the graph in DOT format
 * @param outputFileName the name of the output PDF file
 * @return true on success, false on error (mostly invalid dot format)
 */
bool dotToPdf(const QByteArray &dot, const QString &outputFileName);

/**
 * @brief the NUMBERSFORMAT namespace regroups functions to deal with input and
 * output of numbers between UI and database
 *
 * The numbers are stored in the database as 64 bits integer in fixed point
 * arithmetic, in order to avoid floating point rounding errors Each double is
 * multiplied by FIXEDPOINTCOEFF before beeing stored.
 *
 */

int question(const QString &text, QWidget *parent = 0);

/**
 * @brief yearToMsSinceEpoch converts a year to ms since epoch
 * @param year
 * @return ms since epoch, 0 in case of invalid date
 */
qint64 yearToMsSinceEpoch(int year);

namespace MODES {
/**
 * @brief The DFRFDIRI enum describes available spendings and receipts modes
 *
 * Each mode corresponds to a table in the database. Each of them contains data
 * about nodes and years, and columns are described by the ORED enum.
 */
enum class DFRFDIRI {
  DF,     ///< dépenses de fonctionnement (spendings)
  RF,     ///< recettes de fonctionnement (receipts)
  DI,     ///< dépenses d'investissement (spendings)
  RI,     ///< recettes d'investissement (receipts)
  GLOBAL, ///< a meta-mode not stored in database
};

/**
 * @brief modeToCompactString converts a mode to its compact textual representation
 * @param mode the mode to convert
 * @return the compact text representation of the mode
 */
QString modeToCompactString(DFRFDIRI mode);

/**
 * @brief modeToCompleteString converts a mode to its textual representation
 * @param mode the mode to convert
 * @return the text representation of the mode
 */
QString modeToCompleteString(DFRFDIRI mode);

/**
 * @brief modeToTableString converts a mode to its database table name
 * @param mode the mode to convert
 * @return the suffix of the table for the specified mode
 */
QString modeToTableString(DFRFDIRI mode);

} // namespace MODES

namespace NUMBERSFORMAT {
/**
 * @brief The FORMATMODE enum represents the display mode of numbers
 */
enum class FORMATMODE {
  FORMATMODENORMAL,    ///< numbers are displayed in units
  FORMATMODETHOUSANDS, ///< numbers are divided by 1000 before beeing displayed
  FORMATMODEMILLIONS   ///< numbers are divided by 1000000 before beeing displayed
};

/**
 * @brief getFormatMode get current format mode
 * @return FORMATMODE
 */
FORMATMODE getFormatMode();

/**
 * @brief doubleToDoubleRoundedByNumbersOfDecimals rounds a double tu currentNumDecimals decimals
 * @param number the number to round
 * @return the number rounded to currentNumDecimals
 */
double doubleToDoubleRoundedByNumbersOfDecimals(double number);

/**
 * @brief getNumDecimals returns the numbers of decimals from settings
 * @return the numbers of decimals from settings
 */
int getNumDecimals();

/**
 * @brief updateFormatModeAndDecimals retrieves QSettings user properties about
 * number of decimals and number format mode
 */
void updateFormatModeAndDecimals();

/**
 * @brief getFormatModeSuffix returns suffix to append respecting current format mode
 * @return empty, k or m
 */
const QString getFormatModeSuffix();

/**
 * @brief fixedDividedByFormatMode simply converts a number to kilo or mega
 * @param num the number to divide
 * @return num divided by 1000 or 1000000
 */
qint64 fixedDividedByFormatMode(qint64 num);

/**
 * @brief formatDouble formats a double according to the current locale, the
 * number of decimals and the format mode
 * @param num the double to format
 * @param decimals the number of decimals, to override user preferences of
 * getCurrentNumDecimals
 * @param forcedUnits if true, overrides FORMATMODE setting and uses
 * FORMATMODENORMAL
 * @return a string representing num
 */
QString formatDouble(double num, int decimals = -1, bool forcedUnits = false);

/**
 * @brief formatFixedPoint formats a fixed-point arithmetic number
 *
 * It interprets a fixed point number in qint64 as stored in database, according
 * to the current locale, the number of decimals and the format mode.
 *
 * @param num the number to format
 * @param decimals the number of decimals, to override user preferences of
 * getCurrentNumDecimals
 * @param forcedUnits if true, overrides FORMATMODE setting and uses
 * FORMATMODENORMAL
 * @return a string representing num
 */
QString formatFixedPoint(qint64 num, int decimals = -1, bool forcedUnits = false);

/**
 * @brief doubleToFixedPoint converts a double to a qint64 fixed-point
 * arithmetic number
 * @param num the number to convert
 * @return the number converted to fixed-point arithmetic using FIXEDPOINTCOEFF
 */
qint64 doubleToFixedPoint(double num);

/**
 * @brief fixedPointToDouble converts a qint64 fixed-point arithmetic number to
 * a double
 * @param num the number to convert
 * @return num converted to a double using FIXEDPOINTCOEFF
 */
double fixedPointToDouble(qint64 num);
} // namespace NUMBERSFORMAT

#endif // UTILS_H
