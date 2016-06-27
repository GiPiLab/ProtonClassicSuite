#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QCryptographicHash>
#include <QCustomPlot/qcustomplot.h>
#include "utils.h"
#include "pcx_tables.h"
#include "pcx_graphics.h"
#include "pcx_report.h"
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
    QByteArray expectedT1="2af41b9ce43db8ff515a73ab58667ba92ffb3c550611a83e09ca7bc48593dbcd";
    QByteArray expectedT2="a77797447b71e6db72d019d7b6c8b799adde6e4b0a7df0ce873cc28dac364c0b";
    QByteArray expectedT2bis="fe03fa9ea1b65a559902db310fc4694c27a340ad7ab280dfbc37674a545cbc32";
    QByteArray expectedT3="51cbc76c1139dc03d33a1ffd794c1213ebab6cb89794c86554f9f2d2f92bce4b";
    QByteArray expectedT3bis="fe45f39e0bf7858a8fb79b9d14c0a40adf345dd637517ad2c3927b694a7e5940";
    QByteArray expectedT4="2630b27c4aebc757a9f8ebd080fca66f6986525fc8ee7785d2da0f12caa195a0";
    QByteArray expectedT5="4aaa92f2445aaf45902d00ec11b4242e201115ae05f3b5559c71ed1e1004070a";
    QByteArray expectedT6="2cec8ab80eddd9807d41052c05abdaa817f71ec5cb66c4f4e703a02fbfe5a4f8";
    QByteArray expectedT7="2402b6f94ff60d72b081759a80792babcc6c8d8ee8aeafc6bc46b4d33dfc6a77";
    QByteArray expectedT8="d605901779be1524daae86fe984d1bd4dd1f1af9602c9f9f49b8ba1cd110f310";
    QByteArray expectedT9="23af270e93ce378762eae89d8be49801f6d56ef548567327d2d0c5eff5534384";
    QByteArray expectedT10="7edb34e219369fc926d04bf02ff7b3cc6c3bdd39c6879a16e3921436e6b07a76";
    QByteArray expectedT11="4b5e33b4056cf122ba663da9ce30821fc17787c5c64c42c4d38e387bb2b91708";;
    QByteArray expectedT12="827149eb884222bd07d81b3cc35c6841b5bf9b2cebe4a6afe5b53acd87d5a8bc";
    QByteArray expectedTRaw="e91be458468ece1c29ea9357d575831860ffd65616cbed361254997c397794ce";

    QByteArray hash=hashString(tables.getPCAT1(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT1);

    hash=hashString(tables.getPCAT2(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT2);

    hash=hashString(tables.getPCAT2bis(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT2bis);

    hash=hashString(tables.getPCAT3(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT3);

    hash=hashString(tables.getPCAT3bis(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT3bis);

    hash=hashString(tables.getPCAT4(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT4);

    hash=hashString(tables.getPCAT5(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT5);

    hash=hashString(tables.getPCAT6(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT6);

    hash=hashString(tables.getPCAT7(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT7);

    hash=hashString(tables.getPCAT8(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT8);

    hash=hashString(tables.getPCAT9(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT9);

    hash=hashString(tables.getPCAT10(1));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT10);

    hash=hashString(tables.getPCAT11(1));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT11);

    hash=hashString(tables.getPCAT12(1));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedT12);

    hash=hashString(tables.getPCARawData(1,MODES::DFRFDIRI::DF));
    qDebug()<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedTRaw);


    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::benchmarkReport()
{
    unsigned int treeId=PCx_Tree::createRandomTree("RANDOM",10);
    PCx_Tree tree(treeId);
    tree.finishTree();
    QTextDocument doc;

    unsigned int auditId=PCx_Audit::addNewAudit("RANDOMAUDIT",QList<unsigned int>{2000,2001,2002,2003,2004,2005},treeId);
    PCx_Audit audit(auditId);

    audit.fillWithRandomData(MODES::DFRFDIRI::DF,false);
    audit.fillWithRandomData(MODES::DFRFDIRI::RF,false);
    audit.fillWithRandomData(MODES::DFRFDIRI::DI,false);
    audit.fillWithRandomData(MODES::DFRFDIRI::RI,false);
    audit.finishAudit();

    PCx_Report report(&audit);


    QBENCHMARK(
    report.generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS>{},QList<PCx_Tables::PCATABLES>{
                                              PCx_Tables::PCATABLES::PCAT1,
                                              PCx_Tables::PCATABLES::PCAT2,
                                              PCx_Tables::PCATABLES::PCAT3,
                                              PCx_Tables::PCATABLES::PCAT4,
                                              PCx_Tables::PCATABLES::PCAT5,
                                              PCx_Tables::PCATABLES::PCAT6,
                                              PCx_Tables::PCATABLES::PCAT7,
                                              PCx_Tables::PCATABLES::PCAT8,
                                              PCx_Tables::PCATABLES::PCAT9,
                                              PCx_Tables::PCATABLES::PCAT10,
                                              PCx_Tables::PCATABLES::PCAT11,
                                              PCx_Tables::PCATABLES::PCAT12},
                                          QList<PCx_Graphics::PCAGRAPHICS>{
                                              PCx_Graphics::PCAGRAPHICS::PCAG1,
                                              PCx_Graphics::PCAGRAPHICS::PCAG2,
                                              PCx_Graphics::PCAGRAPHICS::PCAG3,
                                              PCx_Graphics::PCAGRAPHICS::PCAG4,
                                              PCx_Graphics::PCAGRAPHICS::PCAG5,
                                              PCx_Graphics::PCAGRAPHICS::PCAG6,
                                              PCx_Graphics::PCAGRAPHICS::PCAG7,
                                              PCx_Graphics::PCAGRAPHICS::PCAG8,
                                              PCx_Graphics::PCAGRAPHICS::PCAG9,
                                              PCx_Graphics::PCAGRAPHICS::PCAHISTORY
                                          },(qrand()%10)+1,MODES::DFRFDIRI::DF,1,&doc,nullptr,nullptr,nullptr,nullptr));


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
    QByteArray expectedG1="7ee0a8e2b2b96016df3ffff714a339280dfe03b48acbe0904c6acc44eb5c811a";
    QByteArray expectedG2="65bbe405b7e59da62c127753e881487d4b8c8d1190a82793405d269658f18867";
    QByteArray expectedG3="fb9d6fc80576cbbe77a1abe834fe5a3242ad9b454ff983d37a347751e6ec5a74";
    QByteArray expectedG4="3c7d06df4b840d3558199181badbb7809ed90c3bf89bb113c1321f7f3a2a0531";
    QByteArray expectedG5="11bc031f32b353fdaf54a30443d52873e37af19dd71e240b7ff5d7d735d9f8f1";
    QByteArray expectedG6="248cf07f3444fedae490770344783dabbcff237e2f45d563015d7a65d4713f44";
    QByteArray expectedG7="2f917402ce4c5f230515dd620f1c540ad3630dd67c22bb541096001f128f381c";
    QByteArray expectedG8="ea992f330fa8fbf48816f56e1c0d81bf549f1d16058fedee9a1816089faddcce";
    QByteArray expectedG9="e4d6139fcef9c4b2bc1a2ec91196b81b12a674d6e69fa800b33e681a7465712a";
    QByteArray expectedGHist="e3f18690788437b36ba0eb7cc34d62347381d3d5b907348863e6a3d150e434cf";
#elif defined(Q_OS_WIN32)
    //Not up to date
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
    qDebug()<<"G2"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG2);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG3(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G3"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG3);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG4(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G4"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG4);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG5(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G5"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG5);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG6(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G6"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG6);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG7(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G7"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG7);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG8(nodeA,MODES::DFRFDIRI::DF);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G8"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG8);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAG9(nodeA);
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"G9"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedG9);

    ba.clear();
    buffer.open(QIODevice::WriteOnly);
    graphics.getPCAHistory(nodeA,MODES::DFRFDIRI::DF,QList<PCx_Audit::ORED>{PCx_Audit::ORED::OUVERTS,PCx_Audit::ORED::REALISES,PCx_Audit::ORED::ENGAGES,PCx_Audit::ORED::DISPONIBLES});
    plot.toPixmap(PCx_Graphics::DEFAULTWIDTH,PCx_Graphics::DEFAULTHEIGHT,PCx_Graphics::DEFAULTSCALE).save(&buffer,"PNG");
    buffer.close();
    hash=QCryptographicHash::hash(ba,QCryptographicHash::Sha256);
    qDebug()<<"GHist"<<hash.toHex();
    QCOMPARE(hash.toHex(),expectedGHist);

    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}
