#ifndef PCX_STRINGDISTANCE_H
#define PCX_STRINGDISTANCE_H

#include <QString>

/**
 * @brief The PCx_StringDistance class represents a normalized similarity between two strings
 *
 * 0 : the strings are completly differents
 * 1 : the strings are the same
 *
 * Levenshtein and Jaro-Winkler distances are implemented
 *
 */
class PCx_StringDistance
{
public:

    /**
     * @brief The SIMILARITYMETRIC enum represents the distance metrics that are implemented
     */
    enum class SIMILARITYMETRIC
    {
        NORMALIZEDLEVENSHTEIN, ///< is Levenshtein, normalized
        NORMALIZEDLEVENSHTEINCI, ///< is Levenshtein, normalized, case insensitive
        JAROWINKLER, ///< is Jaro Winkler
        JAROWINKLERCI ///< is Jaro Winkler, case insensitive
    };

    /**
     * @brief PCx_StringDistance is a similarity between two strings
     * @param str1 the first string
     * @param str2 the second string
     * @param similarityMode the metric used to compute the similarity
     */
    PCx_StringDistance(const QString &str1, const QString &str2, SIMILARITYMETRIC similarityMode=SIMILARITYMETRIC::JAROWINKLERCI);

    const QString &getStr1() const {return str1;}
    const QString &getStr2() const {return str2;}

    /**
     * @brief getSimilarityMetric returns the similarity metric that has been used to compute the similarity
     * @return the similarity metric
     */
    SIMILARITYMETRIC getSimilarityMetric() const{return metric;}

    /**
     * @brief getNormalizedSimilarity returns the computed similarity between the strings
     *
     * The computation is done once using the similarity metric provided in the constructor
     *
     * @return the normalized similarity, as a double between 0.0 and 1.0, where 1.0 means identical strings
     */
    double getNormalizedSimilarity() const{return normalizedSimilarity;}






private:

    /**
     * @brief normalizedLevenshteinDistance computes the normalized Levenshthein distance (inverse of the similarity)
     * @param sa the first string
     * @param ta the second string
     * @param caseInsensitive computes the distance in case insensitive mode
     * @return the normalized Levenshtein distance (not similarity) between sa and ta. 0.0 means identical strings
     */
    double normalizedLevenshteinDistance(const QString &sa, const QString &ta, bool caseInsensitive=false);

    static unsigned int minimum(unsigned int a, unsigned int b, unsigned int c);

    /**
     * @brief jaroWinklerDistance computes the Jaro-Winkler similarity
     * @param str1 the first string
     * @param str2 the second string
     * @param caseInsensitive compares the strings in case insensitive mode
     * @return the Jaro-Winkler similarity, between 0.0 and 1.0, 1.0 means identical strings
     */
    double jaroWinklerDistance(const QString &str1, const QString &str2, bool caseInsensitive=false);

    QString str1,str2;
    double normalizedSimilarity;
    SIMILARITYMETRIC metric;

};

#endif // PCX_STRINGDISTANCE_H
