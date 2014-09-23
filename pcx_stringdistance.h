#ifndef PCX_STRINGDISTANCE_H
#define PCX_STRINGDISTANCE_H

#include <QString>

class PCx_StringDistance
{
public:

    enum SIMILARITYMETRIC
    {
        NORMALIZEDLEVENSHTEIN,
        NORMALIZEDLEVENSHTEINCI,
        JAROWINKLER,
        JAROWINKLERCI
    };

    PCx_StringDistance(const QString &str, const QString &str2, SIMILARITYMETRIC similarityMode=JAROWINKLERCI);

    const QString &getStr1() const {return str1;}
    const QString &getStr2() const {return str2;}
    double getNormalizedSimilarity() const{return normalizedSimilarity;}



private:
    double normalizedLevenshteinDistance(const QString &sa, const QString &ta, bool caseInsensitive=false);
    unsigned int minimum(unsigned int a, unsigned int b, unsigned int c);
    double jaroWinklerDistance(const QString &str1, const QString &str2, bool caseInsensitive=false);

    QString str1,str2;
    double normalizedSimilarity;

};

#endif // PCX_STRINGDISTANCE_H
