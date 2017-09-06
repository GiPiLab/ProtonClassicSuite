/*
* ProtonClassicSuite
* 
* Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
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

#include "pcx_stringdistance.h"

PCx_StringDistance::PCx_StringDistance(const QString &str1, const QString &str2, SIMILARITYMETRIC similarityMode):str1(str1),str2(str2),metric(similarityMode)
{
    switch(similarityMode)
    {
    case SIMILARITYMETRIC::JAROWINKLER:
        normalizedSimilarity=jaroWinklerDistance(str1,str2,false);
        break;
    case SIMILARITYMETRIC::NORMALIZEDLEVENSHTEIN:
        normalizedSimilarity=1-normalizedLevenshteinDistance(str1,str2,false);
        break;
    case SIMILARITYMETRIC::JAROWINKLERCI:
        normalizedSimilarity=jaroWinklerDistance(str1,str2,true);
        break;
    case SIMILARITYMETRIC::NORMALIZEDLEVENSHTEINCI:
        normalizedSimilarity=1-normalizedLevenshteinDistance(str1,str2,true);
        break;
    }
}


double PCx_StringDistance::normalizedLevenshteinDistance(const QString &sa, const QString &ta,bool caseInsensitive)
{
    unsigned int k,i,j,n,m,cost,distance;
    const QChar *s,*t;

    QString sal,tal;
    if(caseInsensitive)
    {
        sal=sa.toLower();
        tal=ta.toLower();
    }
    else
    {
        sal=sa;
        tal=ta;
    }

    if(sal==tal)return 0.0;

    n=sal.length();
    m=tal.length();

    if(n==0)return(1.0);
    if(m==0)return(1.0);

    int maxSize=qMax(m,n);

    unsigned int tabDistances[(maxSize+1)*(maxSize+1)];



    //To speed up data access instead of using 'at'
    s=sal.constData();
    t=tal.constData();

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
    return (double)distance/maxSize;
}

unsigned int PCx_StringDistance::minimum(unsigned int a, unsigned int b, unsigned int c)
{
    unsigned int min=a;
    if(b<min)
    min=b;
    if(c<min)
    min=c;
    return min;
}




double PCx_StringDistance::jaroWinklerDistance(const QString &str1, const QString &str2, bool caseInsensitive) {
    int i, j, l;
    int m = 0, t = 0;
    QString str1l,str2l;
    if(caseInsensitive)
    {
        str1l=str1.toLower();
        str2l=str2.toLower();
    }
    else
    {
        str1l=str1;
        str2l=str2;
    }
    const QChar *s=str1l.constData();
    const QChar *a=str2l.constData();
    int sl = str1l.size();
    int al = str2l.size();

    int range = qMax(0, qMax(sl, al) / 2 - 1);
    double dw;

    if (!sl || !al)
        return 0.0;

    int sflags[sl], aflags[al];


    for (i = 0; i < al; i++)
        aflags[i] = 0;

    for (i = 0; i < sl; i++)
        sflags[i] = 0;

    /* calculate matching characters */
    for (i = 0; i < al; i++) {
        for (j = qMax(i - range, 0), l = qMin(i + range + 1, sl); j < l; j++) {
            if (a[i] == s[j] && !sflags[j]) {
                sflags[j] = 1;
                aflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (!m)
        return 0.0;

    /* calculate character transpositions */
    l = 0;
    for (i = 0; i < al; i++) {
        if (aflags[i] == 1) {
            for (j = l; j < sl; j++) {
                if (sflags[j] == 1) {
                    l = j + 1;
                    break;
                }
            }
            if (a[i] != s[j])
                t++;
        }
    }
    t /= 2;

    /* Jaro distance */
    dw = (((double)m / sl) + ((double)m / al) + ((double)(m - t) / m)) / 3.0;

    /* calculate common string prefix up to 4 chars */
    l = 0;
    for (i = 0; i < qMin(qMin(sl, al), 4); i++)
        if (s[i] == a[i])
            l++;

    /* Jaro-Winkler distance */
#define SCALING_FACTOR 0.1

    dw = dw + (l * SCALING_FACTOR * (1 - dw));

    return dw;
}

