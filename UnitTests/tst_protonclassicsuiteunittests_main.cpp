#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utils.h"
#include "pcx_tree.h"
#include "pcx_audit.h"
#include "tst_protonclassicsuiteunittests.h"


ProtonClassicSuiteUnitTests::ProtonClassicSuiteUnitTests()
{
}

void ProtonClassicSuiteUnitTests::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();
    QSqlQuery("PRAGMA foreign_keys=ON");
    initializeNewDb();
}

void ProtonClassicSuiteUnitTests::cleanupTestCase()
{
    QSqlDatabase::database().close();
}





QTEST_MAIN(ProtonClassicSuiteUnitTests)

//#include "tst_protonclassicsuiteunittests_main.moc"
