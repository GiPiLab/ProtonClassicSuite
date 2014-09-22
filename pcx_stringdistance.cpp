#include "pcx_stringdistance.h"

PCx_StringDistance::PCx_StringDistance(const QString &str1, const QString &str2):str1(str1),str2(str2)
{
    distance=levenshtein(str1,str2);
}


unsigned int PCx_StringDistance::levenshtein(const QString &sa, const QString &ta)
{
    unsigned int k,i,j,n,m,cost,distance;
    const QChar *s,*t;

    if(sa==ta)return 0;
    n=sa.length();
    m=ta.length();

    if(n==0)return(m);
    if(m==0)return(n);

    int maxSize=qMax(m,n);

    unsigned int tabDistances[(maxSize+1)*(maxSize+1)];



    //To speed up data access instead of using 'at'
    s=sa.constData();
    t=ta.constData();

    ++m;
    ++n;

    for(k=0;k<n;k++)
        tabDistances[k]=k;

    for(k=0;k<m;k++)
        tabDistances[k*n]=k;

    for(i=1;i<n;i++)
    {
        for(j=1;j<m;j++)
        {
            if(s[i-1]==t[j-1])
                cost=0;
            else
                cost=1;
            tabDistances[j*n+i]=minimum(tabDistances[(j-1)*n+i]+1,tabDistances[j*n+i-1]+1,tabDistances[(j-1)*n+i-1]+cost);
        }
    }

    distance=tabDistances[n*m-1];
    return distance;
}

inline unsigned int PCx_StringDistance::minimum(unsigned int a, unsigned int b, unsigned int c)
{
    unsigned int min=a;
    if(b<min)
    min=b;
    if(c<min)
    min=c;
    return min;
}
