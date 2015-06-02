#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QCryptographicHash>
#include <QCustomPlot/qcustomplot.h>
#include "utils.h"
#include "pcx_tables.h"
#include "pcx_graphics.h"
#include "tst_protonclassicsuiteunittests.h"

void ProtonClassicSuiteUnitTests::fillTestAudit(PCx_Audit *audit)
{
    int firstYear=audit->getYears().first();
    int lastYear=audit->getYears().last();
    QList<double> values={65678.3, 66154.3, 394754, 68857.1, 218632, 369859, 82991.4, 363930, 129854,
                           292819, 118571, 319187, 38998.5, 243909, 189486, 343779, 22629.2, 351890,
                           31705.7, 113993, 423967, 88494.2, 392966, 341903, 55329.6, 295115, 6070.43,
                           187548, 320275, 161991, 18543.9, 238280, 250449, 322051, 205258, 77598.7, 105687,
                           411057, 253461, 374204, 145066, 283387, 356531, 80491.4, 147056, 308305, 414776,
                           275012, 75619.3, 177614, 182726, 341583, 1785.75, 81796.9, 146423, 278263, 190203,
                           36165.6, 202805, 45837.1, 398973, 97304.4, 282173, 424908, 247909, 363903, 83313.5,
                           422401, 214320, 42290, 409648, 271988, 334211, 370082, 143964, 413732, 417126, 416604,
                           249822, 46883.6, 98482.3, 405190, 35378.7, 276694, 5857.75, 325860, 237603, 191510,
                           138822, 196374, 13648.5, 83803, 171284, 207096, 82751.2, 12422.7, 234194, 77392.1,
                           394402, 86692.1};


    int count=0;
    QList<unsigned int> leaves=audit->getAttachedTree()->getLeavesId();
    for(unsigned int mode=MODES::DFRFDIRI::DF;mode<sizeof(MODES::DFRFDIRI);mode++)
    {
        for(int year=firstYear;year<=lastYear;year++)
        {
            foreach(unsigned int leaf,leaves)
            {

                audit->setLeafValues(leaf,(MODES::DFRFDIRI)mode,year,QMap<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,values.at(count)},
                                                                                                   {PCx_Audit::ORED::REALISES,values.at(count+1)},
                                                                                                   {PCx_Audit::ORED::ENGAGES,values.at(count+2)}});


                if(count==values.count()-4)
                    count=0;
                else
                    count++;
            }
        }
    }
}


void ProtonClassicSuiteUnitTests::testCaseForPCATables()
{
    unsigned int treeId=PCx_Tree::addTree("TREEFORTEST");
    PCx_Tree tree(treeId);
    unsigned int nodeA=tree.addNode(1,1,"nodeA");
    unsigned int nodeB=tree.addNode(nodeA,1,"nodeB");
    tree.addNode(nodeA,1,"nodeC");
    tree.addNode(1,1,"nodeD");
    tree.addNode(nodeB,1,"nodeE");
    tree.finishTree();

    unsigned int auditId=PCx_Audit::addNewAudit("AUDITFORTEST",QList<unsigned int>{2010,2011,2012},treeId);

    PCx_Audit audit(auditId);

    fillTestAudit(&audit);

    PCx_Tables tables(&audit);


    //SHA-256 hex hashes of expected html outputs for each table
    QByteArray expectedT1="cc34e9319f104b30d01ae6138e19501698195deba0205ab1d9fc2a493570c683";
    QByteArray expectedT2="6ac3b0c1b09497df5c2bc47deede3d1be27d30bd63a449a56ed2926881875f70";
    QByteArray expectedT2bis="cdfa586dc11a79ee3bc86ab497a9de36cd9ae6f8502ea1ae23cff2e046cde119";
    QByteArray expectedT3="b899f6878b5dc2a8a550a124e2564f4d6db4ae8395c1d1c2562c35949de308a4";
    QByteArray expectedT3bis="af9ecc8534e0cf8ae1321a58f626e187c222fc9532a6ad0a4daf3b3f6de2ec63";
    QByteArray expectedT4="8dd0b2a7dcd37186588de12a85d5441f8efd15668afcca72a9c23c50413c91ff";
    QByteArray expectedT5="20d4fdbc516ef746c6bde211a04d517501eb51b9a1fae79c33bd680f75f75996";
    QByteArray expectedT6="084dfea16e3ff8dd3a686b28e5d6965ba9e5b82c7696be96ff91f96216b0bbb1";
    QByteArray expectedT7="ad8cbf1d3e0213d48f411b42ce74abe90080579919d5036d89a3166af342e5a3";
    QByteArray expectedT8="b54f5261d2fc66e5199fc83c35384b705c09aa2c3a5461cbb049d26957819127";
    QByteArray expectedT9="0d127d267f0f52fe1bb6e4896abd40fb6ab6fda603f5e2dffa7f2b9a34be0669";
    QByteArray expectedT10="d4f817d530215e32cf8f6296fadb0c5a01b99b385a752e02704c258d4603f4ee";
    QByteArray expectedT11="c2d71e7da541eaadd31ad5134713612606813fde9ba6428ba735c3354f6d4949";;
    QByteArray expectedT12="75ea64119d3edf9a2df3c6c215077c5e0971b30ea5352e39e47f6b534f4bf72d";
    QByteArray expectedTRaw="a84fdab99b6ff8d8433c26adffaec6a0100b866cb39d6e22e5c2b6bc495aeab3";

    QByteArray hash=hashString(tables.getPCAT1(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
   QCOMPARE(hash.toHex(),expectedT1);

    hash=hashString(tables.getPCAT2(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT2);

    hash=hashString(tables.getPCAT2bis(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT2bis);

    hash=hashString(tables.getPCAT3(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT3);

    hash=hashString(tables.getPCAT3bis(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
   QCOMPARE(hash.toHex(),expectedT3bis);

    hash=hashString(tables.getPCAT4(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT4);

    hash=hashString(tables.getPCAT5(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT5);

    hash=hashString(tables.getPCAT6(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
   QCOMPARE(hash.toHex(),expectedT6);

    hash=hashString(tables.getPCAT7(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT7);

    hash=hashString(tables.getPCAT8(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT8);

    hash=hashString(tables.getPCAT9(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT9);

    hash=hashString(tables.getPCAT10(1));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT10);

    hash=hashString(tables.getPCAT11(1));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT11);

    hash=hashString(tables.getPCAT12(1));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT12);

    hash=hashString(tables.getPCARawData(1,MODES::DFRFDIRI::DF));
    //qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedTRaw);


    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}


void ProtonClassicSuiteUnitTests::testCaseForPCAGraphics()
{
    unsigned int treeId=PCx_Tree::addTree("TREEFORTEST");
    PCx_Tree tree(treeId);
    unsigned int nodeA=tree.addNode(1,1,"nodeA");
    unsigned int nodeB=tree.addNode(nodeA,1,"nodeB");
    tree.addNode(nodeA,1,"nodeC");
    tree.addNode(1,1,"nodeD");
    tree.addNode(nodeB,1,"nodeE");
    tree.finishTree();

    unsigned int auditId=PCx_Audit::addNewAudit("AUDITFORTEST",QList<unsigned int>{2010,2011,2012,2013},treeId);

    PCx_Audit audit(auditId);

    fillTestAudit(&audit);

    QCustomPlot plot;
    QByteArray ba,hash;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    PCx_Graphics graphics(&audit,&plot);

#ifdef Q_OS_LINUX
    QByteArray expectedG1="51695ec51edd0952732f80adf923483d0b8b0f3d1bc1f26682deda255c07ee98";
    QByteArray expectedG2="44636b86c3be6080c0bda664f921ea9140709c6790d214d67b1f2f573a5c3845";
    QByteArray expectedG3="dbc8cfba3aca6c2ec209e66ca362a9e65c4fdbbbd169d578d31b21ba2e83c995";
    QByteArray expectedG4="75032ffc61383f0b3dee442bbb69fb574ea2f64ef08a22188a0286f5973464bd";
    QByteArray expectedG5="d3a18acb27cf3fc4f79b434bd4f61eed0d2f4b17611f2ae23753563867c4cca6";
    QByteArray expectedG6="1af0df4af22f7f9e98cb6d9085405866f5ffd72e48bbd11583e4d55f4d296e22";
    QByteArray expectedG7="b25f5a201d27db6e6c7d6a2327ce4170d63afd0ccd083847092f2ca2a068786b";
    QByteArray expectedG8="3d81ba9f3acb38f9e71af8fade6a94b1bed3c16c3e6dd7d39916d2634f4342c6";
    QByteArray expectedG9="7235f9764d5e15639a0bcb263b8c0dff669d7ac76730611f4c810ac965df5952";
    QByteArray expectedGHist="8ece265dbc16d2ae2bf67794fed582e1b06b6ae19daf326dd8fcb4fa2064b246";
#elif defined(Q_OS_WIN32)
    QByteArray expectedG1="018707e0534e68d317b0b5d483c7e319bf958131e2a6c36807e61b540c1eb947";
    QByteArray expectedG2="7a06df9077ba4114473922cf0face339ba039673aaebbc3e4dd00624311f9740";
    QByteArray expectedG3="171259d76791bdbc4ee3681eb4d34f005b5d28c5dc25f5d38a334ab2ea58e113";
    QByteArray expectedG4="448d3a8d7067f926e14073132306c561f159d1eef85d128ab5509e772ce15260";
    QByteArray expectedG5="04168e8684e49540e7c50014697b80cc828710fa521b131bb27d41c9eea66513";
    QByteArray expectedG6="1275e4068eeb70669303a719af7ba5b3c7e7ab1bedc6235721735f73f84e5a52";
    QByteArray expectedG7="c13524ae23dcbc60be13501fc200f756a35586414ee61d11509157d6b391e1e1";
    QByteArray expectedG8="b3acf91c1435533a71bc0a0a7ee454ab953686e5b6007a85add5c66359ace660";
    QByteArray expectedG9="4377368f5ea6f9596c6a82dc0095d66e1bf71b9e5bba271cbcb206a05592265c";
    QByteArray expectedGHist="1efd28f8f98daea304dae8ca19395212abd4115a4cc7d6d1e7abedb90acec24c";
#endif

    graphics.getPCAG1(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);

    //qDebug()<<"G1"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG1);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG2(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G2"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG2);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG3(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G3"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG3);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG4(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G4"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG4);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG5(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G5"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG5);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG6(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G6"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG6);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG7(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G7"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG7);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG8(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G8"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG8);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG9(nodeA);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G9"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG9);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAHistory(nodeA,MODES::DFRFDIRI::DF,QList<PCx_Audit::ORED>{PCx_Audit::ORED::OUVERTS,PCx_Audit::ORED::REALISES,PCx_Audit::ORED::ENGAGES,PCx_Audit::ORED::DISPONIBLES});
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"GHist"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedGHist);


    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}
