#ifndef TST_PROTONCLASSICSUITEUNITTESTS_H
#define TST_PROTONCLASSICSUITEUNITTESTS_H

#include "pcx_audit.h"
#include <QtTest>


class ProtonClassicSuiteUnitTests : public QObject
{
    Q_OBJECT

    void fillTestAudit(PCx_Audit *audit);

public:
    ProtonClassicSuiteUnitTests();



private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCaseForTreeManagement();
    void testCaseForTreeNodes();
    void testCaseForTreeNodesOrder();
    void testCaseForTreeDuplication();
    void testCaseForTreeImportExport();
    void testCaseForTreeToDot();

    void testCaseForNumbersConversion();

    void testCaseForAuditManagement();
    void testCaseForAuditNodes();
    void testCaseForAuditDuplication();
    void testCaseForAuditImportExport();


    void testCaseForPCATables();
    void testCaseForPCAGraphics();
};



#endif // TST_PROTONCLASSICSUITEUNITTESTS_H

