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

#ifndef PCX_NODESIMILARITYTABLEMODEL_H
#define PCX_NODESIMILARITYTABLEMODEL_H

#include "pcx_stringdistance.h"
#include <QAbstractTableModel>

/**
 * @brief The PCx_NodeSimilarityTableModel class is a TableModel to show
 * similarity between strings, used to compare node names of a Tree
 */
class PCx_NodeSimilarityTableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  /**
   * @brief PCx_NodeSimilarityTableModel constructs a table model with three
   * columns
   *
   * Column 1 is the first string
   * Column 2 is the similarity
   * Column 3 is the second string
   *
   * @param strings a list of strings to compare
   * @param metric the metric to use for similarity
   * @param parent the QObject parent
   */
  explicit PCx_NodeSimilarityTableModel(const QStringList &strings, PCx_StringDistance::SIMILARITYMETRIC metric,
                                        QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:
  /**
   * @brief listOfStrings the list of strings to compare
   */
  QStringList listOfStrings;

  /**
   * @brief distances the list of computed distances
   */
  QList<PCx_StringDistance> distances;

  /**
   * @brief similarityMetric the similarity used
   */
  PCx_StringDistance::SIMILARITYMETRIC similarityMetric;

  /**
   * @brief computeDistances computes the similarities between listOfStrings and
   * fills distances
   */
  void computeDistances(void);

  /**
   * @brief threshold do not display PCx_StringDistance instances that are lower
   * or equal than this threshold
   */
  static constexpr double threshold = 0.9;

  PCx_NodeSimilarityTableModel(const PCx_NodeSimilarityTableModel &c);
  PCx_NodeSimilarityTableModel &operator=(const PCx_NodeSimilarityTableModel &);
};

#endif
