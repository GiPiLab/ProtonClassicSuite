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

#include "mainwindow.h"
#include "utils.h"
#include <QApplication>
#include <QSharedMemory>
#include <QSqlDatabase>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QLocale>
#include <time.h>
#include <iostream>
#include <QSqlQuery>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message=QString("%1 (%2:%3)\n").arg(msg,context.file).arg(context.line);

    //Do not display a message box in case of XCB warning to avoid ui global freeze
    bool noMessageBox=false;

    if(message.contains("XCB",Qt::CaseInsensitive)==true)
    {
        noMessageBox=true;
    }
#ifdef Q_OS_ANDROID
    //Avoid an anoying popup on android and combobox
    if(message.contains("This plugin does not support grabbing the keyboard",Qt::CaseInsensitive)==true)
    {
        noMessageBox=true;
    }
#endif

    switch (type) {

    case QtInfoMsg:
        message.prepend("[I]");
        std::cerr<<qPrintable(message);
        break;
    case QtDebugMsg:
        message.prepend("[D]");
        std::cerr<<qPrintable(message);
        break;
    case QtWarningMsg:
        message.prepend("[W]");
        if(noMessageBox==false)
            QMessageBox::warning(0,"Attention",message);
        std::cerr<<qPrintable(message);
        break;
    case QtCriticalMsg:
        message.prepend("[C]");
        if(noMessageBox==false)
            QMessageBox::critical(0,"Erreur critique",message);
        std::cerr<<qPrintable(message);
        break;
    case QtFatalMsg:
        message.prepend("[F]");
        if(noMessageBox==false)
            QMessageBox::critical(0,"Erreur fatale",message);
        std::cerr<<qPrintable(message);
        die();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Avoid multiple application instances

#ifndef Q_OS_ANDROID
    QSharedMemory sharedMemory;
    sharedMemory.setKey("GIPILABPROTONCLASSICSUITE");
    sharedMemory.attach();


    if (!sharedMemory.create(1)) {
        QMessageBox::information(nullptr,"Attention","Une seule instance de ProtonClassicSuite est autorisée à la fois");
        return EXIT_FAILURE;
    }
#endif

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);


    //qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");

    qInstallMessageHandler(myMessageOutput);

    //qDebug()<<"***ProtonClassicSuite starting***";

/*
    QPixmap pixmap(":/icons/icons/logo.png");
    QSplashScreen splash(pixmap);
    splash.show();


    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
*/


    int retval;

    QSqlDatabase::addDatabase("QSQLITE");

    MainWindow w;
    w.show();
  //  splash.finish(&w);

    retval=a.exec();

    QSqlDatabase::database().close();

#ifndef Q_OS_ANDROID
    sharedMemory.detach();
#endif

    return retval;
}
