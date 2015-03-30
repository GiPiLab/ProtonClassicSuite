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

                audit->setLeafValues(leaf,(MODES::DFRFDIRI)mode,year,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,values.at(count)},
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
    QByteArray expectedT1="b226fa1559b31016ad75befcb86a41b75faee59835d369b27927197f11dc4de5";
    QByteArray expectedT2="6ac3b0c1b09497df5c2bc47deede3d1be27d30bd63a449a56ed2926881875f70";
    QByteArray expectedT2bis="cdfa586dc11a79ee3bc86ab497a9de36cd9ae6f8502ea1ae23cff2e046cde119";
    QByteArray expectedT3="119723a0e1d7186b319d16767ed73a41c50bb0cb4cb96be580d3f2f82cf116fb";
    QByteArray expectedT3bis="3b3285dc4ec5f3fb1cd234ce8b1bff8cd8f1997314e9b60aa8616010c203f4d1";
    QByteArray expectedT4="8dd0b2a7dcd37186588de12a85d5441f8efd15668afcca72a9c23c50413c91ff";
    QByteArray expectedT5="20d4fdbc516ef746c6bde211a04d517501eb51b9a1fae79c33bd680f75f75996";
    QByteArray expectedT6="b033807b131dffe4c6f2e8c84c98d9591c0caf30a187767b86093b29cf31f4d1";
    QByteArray expectedT7="dde0b25d1d815aefaec42dbdff1a9f034b03816b355d573480b2e033d9f8f983";
    QByteArray expectedT8="40e625304de181628f9528790bb22661c5d1021f74b3ffa454ec676486eae7ee";
    QByteArray expectedT9="a3d33596fc85bed1fc50416aa708647ee012a367f26aac9f57216d9aa67679d3";
    QByteArray expectedT10="5550c2ca377fed7503f5188ff0cb709eca9f410d8ca3bb0779dc94d5ed5250f9";
    QByteArray expectedT11="705f2c25a9f38cf8957f148bf59fe9d52eb347c3b1497c776c655c5a2c0f379b";;
    QByteArray expectedT12="0612b92cfa5c1ac915e437a2fa848a85a8139492ee78c43f16608779e9eb1be3";
    QByteArray expectedTRaw="ae2aa9937363f1a5c2902729f2736f8048c98e54a41a79dadcfc3a51aa037431";

    QByteArray hash=hashString(tables.getPCAT1(1,MODES::DFRFDIRI::DF));
   // qDebug()<<hash.toHex();
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

    QByteArray expectedG1="2464140d8cdf0d673bf2ccac2959462ffd4a1eea77f3a1126e48b900c8eb3ca1";
    QByteArray expectedG2="68482a48f072204ef9d20086d1ea9e0997598de4c0350719f6a6b3ffa0553d96";
    QByteArray expectedG3="85a378e5acd1a1cf7a886ee41c40d9b44abc4a0341581a84d8c9d7a163c71146";
    QByteArray expectedG4="19c6a1a1691c4779034776f54481aa35e631785d030d868e10704ab0af62b1ad";
    QByteArray expectedG5="ffd46de8c8c2f59de823a88072c310f8afece8b9a935cbf92c80c94d54ec3887";
    QByteArray expectedG6="faef1098b0aebb66160e7f952cf2cc88ac1ca95c17032775deead69bf7a1f40a";
    QByteArray expectedG7="e996a1ceccddac070153e9d06030db9ae46b4510ea94b75d6d98ee48248d46e6";
    QByteArray expectedG8="2b29987da7409f05d0fd880614ca5ad5c1883c323fbfce8e554d8144f4452869";
    QByteArray expectedG9="708ad17991c3767c9314c0a31d191108f319d53450333aeb566afbd458568fb5";
    QByteArray expectedGHist="caaf86f3fc1287ebe8d35bb02ea89ae353808a4609de10f411bb7c7bb238cd63";


    graphics.getPCAG1(nodeA,MODES::DFRFDIRI::DF);

    QPixmap pixmap=plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE);//.save(&buffer,"PNG");
    pixmap.save("/tmp/out.png","PNG");
    qDebug()<<pixmap.cacheKey();
    //buffer.close();

    //hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"G1"<<hash.toHex();
    //QCOMPARE(hash.toHex(),expectedG1);

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
    graphics.getPCAHistory(nodeA,MODES::DFRFDIRI::DF,QList<PCx_Audit::ORED>{PCx_Audit::OUVERTS,PCx_Audit::REALISES,PCx_Audit::ENGAGES,PCx_Audit::DISPONIBLES});
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"GHist"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedGHist);


    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}
