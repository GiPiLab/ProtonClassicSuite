#include <QCoreApplication>
#include <QTimer>
#include <QStringList>
#include <QDebug>
#include "m14_to_sql.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(a.arguments().size()!=2)
    {
        qCritical()<<"Passez en argument un fichier texte contenant la nomenclature M14";
        return -1;
    }


    m14_to_sql app(a.arguments().at(1));
    QTimer::singleShot(0,&app,SLOT(run()));

    return a.exec();
}
