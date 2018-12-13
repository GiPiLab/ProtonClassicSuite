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

#ifndef PCX_REPORTINGTABLESUPERVISIONMODEL_H
#define PCX_REPORTINGTABLESUPERVISIONMODEL_H

#include "pcx_reporting.h"
#include "utils.h"
#include <QAbstractTableModel>
#include <QList>

class PCx_ReportingTableSupervisionModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit PCx_ReportingTableSupervisionModel(PCx_Reporting *reporting, MODES::DFRFDIRI mode,
                                              unsigned int selectedDateTimeT = 0, QObject *parent = nullptr);

  enum class TABLESUPERVISIONCOLUMNS {
    BP = 2,
    OUVERTS,
    REALISES,
    ENGAGES,
    DISPONIBLES,
    PERCENTREALISES,
    PERCENTBP,
    PERCENTENGAGES,
    PERCENTDISPONIBLES,
    REALISESPREDITS,            // Realises prédits à ce rythme
    DIFFREALISESPREDITSOUVERTS, // Difference REALISESPREDITS-OUVERTS
    TAUXECART,
    DECICP,         // Date estimee de consommation integrale des credits
    DERPSUR2,       // Date estimee du realise = ouverts/2
    NB15NRESTANTES, // Nombre de 15N restant dans l'année
    RAC,            // Reste à consommer
    CPP15NR         // Consommé prévisionnel par 15N restante

  };

  MODES::DFRFDIRI getMode() const { return currentMode; }
  void setMode(MODES::DFRFDIRI mode);

  void setDateTimeT(time_t dateTimeT);
  time_t getSelectedDateTimeT() const { return selectedDateTimeT; }

  static QString getColumnName(TABLESUPERVISIONCOLUMNS column);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:
  class Entry {
  public:
    explicit Entry(unsigned int nodeId, unsigned int dateTimeT, qint64 _bp, qint64 ouverts, qint64 realises,
                   qint64 engages, qint64 disponibles);
    unsigned int nodeId;
    QDate date, dECICO, dERO2;
    double bp, ouverts, realises, engages, disponibles;
    double percentReal, tauxEcart, percentEngage, percentDisponible, percentBP;
    double realisesPredits, diffRealisesPreditsOuverts, resteAConsommer;
    unsigned int nb15NRestantes;
    double consommePrevPar15N;
  };

  PCx_Reporting *reporting;
  MODES::DFRFDIRI currentMode;
  void updateQuery();
  unsigned int nbNodes;
  QList<Entry> entries;
  time_t selectedDateTimeT;
};

#endif // PCX_REPORTINGTABLESUPERVISIONMODEL_H
