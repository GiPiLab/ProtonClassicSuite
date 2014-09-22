#ifndef PCX_STRINGDISTANCE_H
#define PCX_STRINGDISTANCE_H

#include <QString>

class PCx_StringDistance
{
public:
    PCx_StringDistance(const QString &str, const QString &str2);

    const QString &getStr1() const {return str1;}
    const QString &getStr2() const {return str2;}

    int getDistance() const{return distance;}


private:
    unsigned int levenshtein(const QString &sa, const QString &ta);
    unsigned int minimum(unsigned int a, unsigned int b, unsigned int c);

    QString str1,str2;
    int distance;




};

#endif // PCX_STRINGDISTANCE_H
