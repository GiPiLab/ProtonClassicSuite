#include "utils.h"
#include "pcx_previsionitemcriteria.h"
#include "tst_protonclassicsuiteunittests.h"


void ProtonClassicSuiteUnitTests::testCaseForPCBPrevision()
{

}













void ProtonClassicSuiteUnitTests::testCaseForPCBPrevisionItemCriteria()
{
    unsigned int treeId=PCx_Tree::addTree("TESTTREE");
    PCx_Tree tree(treeId);
    unsigned int node1=tree.addNode(1,1,"NODE1");
    tree.finishTree();
    unsigned int auditId=PCx_Audit::addNewAudit("AUDITTEST",QList<unsigned int>{2010,2011,2012,2013},treeId);

    PCx_Audit audit(auditId);
    audit.setLeafValues(node1,MODES::DFRFDIRI::DF,2010,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,10.0},{PCx_Audit::ORED::REALISES,8.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::DF,2011,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,15.0},{PCx_Audit::ORED::REALISES,14.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::DF,2012,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,16.0},{PCx_Audit::ORED::REALISES,17.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::DF,2013,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,20.0},{PCx_Audit::ORED::REALISES,19.0}});


    qint64 val;
    QString serialized;
    PCx_PrevisionItemCriteria criteria2;

    //MINIMUM
    PCx_PrevisionItemCriteria criteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(10.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(10.0));
    criteria2.unserialize(serialized);
    val=criteria2.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(10.0));

    //MAXIMUM
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(20.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(20.0));

    //LASTVALUE
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::LASTVALUE,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(20.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(20.0));

    //FIXEDVALUE
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUE,PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(13.5));
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(13.5));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);

    //FIXEDVALUE2
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(13.5));
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION,PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(13.5));
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(13.5));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(13.5));

    //AVERAGE
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::AVERAGE,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(15.25));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(15.25));

    //PERCENT
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT,PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(10.0));
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(2.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(2.0));

    //REGLIN
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::REGLIN,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(23.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::DF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(23.0));


    //The same with large values
    audit.setLeafValues(node1,MODES::DFRFDIRI::RF,2010,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,-MAX_NUM},{PCx_Audit::ORED::REALISES,8.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::RF,2011,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,0.0},{PCx_Audit::ORED::REALISES,14.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::RF,2012,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,10.0},{PCx_Audit::ORED::REALISES,17.0}});
    audit.setLeafValues(node1,MODES::DFRFDIRI::RF,2013,QHash<PCx_Audit::ORED,double>{{PCx_Audit::ORED::OUVERTS,MAX_NUM},{PCx_Audit::ORED::REALISES,19.0}});

    //MINIMUM
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));
    criteria2.unserialize(serialized);
    val=criteria2.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));

    //MAXIMUM
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM));

    //AVERAGE
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::AVERAGE,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(2.5));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(2.5));

    //PERCENT
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT,PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(10.0));
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM/10.0));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM/10.0));

    //REGLIN
    criteria=PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::REGLIN,PCx_Audit::ORED::OUVERTS);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(6442450947.5));
    serialized=criteria.serialize();
    criteria=PCx_PrevisionItemCriteria(serialized);
    val=criteria.compute(auditId,MODES::DFRFDIRI::RF,node1);
    QCOMPARE(val,NUMBERSFORMAT::doubleToFixedPoint(6442450947.5));

    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}


