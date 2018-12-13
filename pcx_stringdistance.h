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

#ifndef PCX_STRINGDISTANCE_H
#define PCX_STRINGDISTANCE_H

#include <QString>

/**
 * @brief The PCx_StringDistance class represents a normalized similarity
 * between two strings
 *
 * 0 : the strings are completly differents
 * 1 : the strings are the same
 *
 * Levenshtein and Jaro-Winkler distances are implemented
 *
 */
class PCx_StringDistance {
public:
  /**
   * @brief The SIMILARITYMETRIC enum represents the distance metrics that are
   * implemented
   */
  enum class SIMILARITYMETRIC {
    NORMALIZEDLEVENSHTEIN,   ///< is Levenshtein, normalized
    NORMALIZEDLEVENSHTEINCI, ///< is Levenshtein, normalized, case insensitive
    JAROWINKLER,             ///< is Jaro Winkler
    JAROWINKLERCI            ///< is Jaro Winkler, case insensitive
  };

  /**
   * @brief PCx_StringDistance is a similarity between two strings
   * @param str1 the first string
   * @param str2 the second string
   * @param similarityMode the metric used to compute the similarity
   */
  PCx_StringDistance(const QString &str1, const QString &str2,
                     SIMILARITYMETRIC similarityMode = SIMILARITYMETRIC::JAROWINKLERCI);

  const QString &getStr1() const { return str1; }
  const QString &getStr2() const { return str2; }

  /**
   * @brief getSimilarityMetric returns the similarity metric that has been used
   * to compute the similarity
   * @return the similarity metric
   */
  SIMILARITYMETRIC getSimilarityMetric() const { return metric; }

  /**
   * @brief getNormalizedSimilarity returns the computed similarity between the
   * strings
   *
   * The computation is done once using the similarity metric provided in the
   * constructor
   *
   * @return the normalized similarity, as a double between 0.0 and 1.0,
   * where 1.0 means identical strings
   */
  double getNormalizedSimilarity() const { return normalizedSimilarity; }

private:
  /**
   * @brief normalizedLevenshteinDistance computes the normalized Levenshthein
   * distance (inverse of the similarity)
   * @param sa the first string
   * @param ta the second string
   * @param caseInsensitive computes the distance in case insensitive mode
   * @return the normalized Levenshtein distance (not similarity) between sa and
   * ta. 0.0 means identical strings
   */
  double normalizedLevenshteinDistance(const QString &sa, const QString &ta, bool caseInsensitive = false);

  static int minimum(int a, int b, int c);

  /**
   * @brief jaroWinklerDistance computes the Jaro-Winkler similarity
   * @param str1 the first string
   * @param str2 the second string
   * @param caseInsensitive compares the strings in case insensitive mode
   * @return the Jaro-Winkler similarity, between 0.0 and 1.0, 1.0 means
   * identical strings
   */
  double jaroWinklerDistance(const QString &str1, const QString &str2, bool caseInsensitive = false);

  QString str1, str2;
  double normalizedSimilarity;
  SIMILARITYMETRIC metric;
};

#endif // PCX_STRINGDISTANCE_H
