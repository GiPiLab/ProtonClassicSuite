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
    QList<unsigned int> leaves=audit->getAttachedTree()->getLeavesId();
    for(unsigned int mode=MODES::DFRFDIRI::DF;mode<sizeof(MODES::DFRFDIRI);mode++)
    {
        for(int year=firstYear;year<=lastYear;year++)
        {
            foreach(unsigned int leaf,leaves)
            {
                audit->setLeafValues(leaf,(MODES::DFRFDIRI)mode,year,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,(double)year*(leaf+1.4+mode)},
                                                                                                   {PCx_Audit::ORED::REALISES,(double)year/(2.5+leaf)},
                                                                                                   {PCx_Audit::ORED::ENGAGES,(double)year/(3.2+mode)}}
                                     );
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
    QByteArray expectedT1="3d34111681350ebe0a2f387bf9e37f4e3a97a3a0710f278a1de4c6d7e38c5345";
    QByteArray expectedT2="ad91a8f3f54ff4f1487d286015cac140fe4649191671bc5d0d93988ef8b0ce96";
    QByteArray expectedT2bis="a964ac7812e4718bed48719e9c801851a8869420b67f48e3aa7448a44374e84d";
    QByteArray expectedT3="4d62fbef1a2c35ebd2863e0bde2c30f3b30236f49f2ceff0ca2cbeeebe8a3c7b";
    QByteArray expectedT3bis="6e7572381f39e5967e7170a50bd5c063ea201035c8e06c57488f77a8f56d4ecd";
    QByteArray expectedT4="3d8b3d3045de367b902b6400769fc18070581d132924d16137f55acbda0db794";
    QByteArray expectedT5="76773e173a8e6e40b59ac0f94b9eb21824ca9b0fed29557fd0bc0eef82e0eea4";
    QByteArray expectedT6="b0a9ff485dd9e0d9861dfddea580e096bd023bcbe480473f319974fd47a39bda";
    QByteArray expectedT7="b440a9bb0a57fb861728c0370303ede9f4918cd730ce0b58c5ae2f0fb06cd3a3";
    QByteArray expectedT8="d3046bc0f6c16f1041066b6ec000aa1b0d2914d3d5177be3117f78b3d99b454c";
    QByteArray expectedT9="e151e33a9a425f0fa77ec467e58a0b296958412d212668df0f2ba8a2e13d468a";
    QByteArray expectedT10="a72eb92fb8923ba0d5c2c917b80b540d0f111f02c399aa94b660c5413179ad4f";
    QByteArray expectedT11="54c56bdd60299dc8837013474865a530e8f25edbfd19b6da27014d6067e06b0a";;
    QByteArray expectedT12="eefb7453d9f260091041ce7aa77f8e9df71262597d293b212779b25acb6d9152";
    QByteArray expectedTRaw="c26bb85a7ac3ae4cd0d7e12adc029d68b0edd20f7d84c708f9f86b133a37765f";

    QByteArray hash=QCryptographicHash::hash(tables.getPCAT1(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT1);

    hash=QCryptographicHash::hash(tables.getPCAT2(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT2);

    hash=QCryptographicHash::hash(tables.getPCAT2bis(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT2bis);

    hash=QCryptographicHash::hash(tables.getPCAT3(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT3);

    hash=QCryptographicHash::hash(tables.getPCAT3bis(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT3bis);

    hash=QCryptographicHash::hash(tables.getPCAT4(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT4);

    hash=QCryptographicHash::hash(tables.getPCAT5(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT5);

    hash=QCryptographicHash::hash(tables.getPCAT6(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT6);

    hash=QCryptographicHash::hash(tables.getPCAT7(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT7);

    hash=QCryptographicHash::hash(tables.getPCAT8(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT8);

    hash=QCryptographicHash::hash(tables.getPCAT9(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT9);

    hash=QCryptographicHash::hash(tables.getPCAT10(1).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT10);

    hash=QCryptographicHash::hash(tables.getPCAT11(1).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT11);

    hash=QCryptographicHash::hash(tables.getPCAT12(1).toUtf8(),QCryptographicHash::Sha256);
    QCOMPARE(hash.toHex(),expectedT12);

    hash=QCryptographicHash::hash(tables.getPCARawData(1,MODES::DFRFDIRI::DF).toUtf8(),QCryptographicHash::Sha256);
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

    unsigned int auditId=PCx_Audit::addNewAudit("AUDITFORTEST",QList<unsigned int>{2010,2011,2012},treeId);

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
    graphics.getPCAHistory(nodeA,MODES::DFRFDIRI::DF,QList<PCx_Audit::ORED>{PCx_Audit::OUVERTS,PCx_Audit::REALISES,PCx_Audit::ENGAGES,PCx_Audit::DISPONIBLES});
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    //qDebug()<<"GHist"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedGHist);


    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}
