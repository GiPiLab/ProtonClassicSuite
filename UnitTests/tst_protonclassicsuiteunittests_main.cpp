#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utils.h"
#include "pcx_tree.h"
#include <iostream>
#include <QDataStream>
#include "pcx_audit.h"
#include "tst_protonclassicsuiteunittests.h"


ProtonClassicSuiteUnitTests::ProtonClassicSuiteUnitTests()
{
}


void ProtonClassicSuiteUnitTests::cleanup()
{
    QList<QPair<unsigned int,QString>> listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::AllAudits);
    QPair<unsigned int,QString> audit;
    foreach(audit,listOfAudits)
    {
        PCx_Audit::deleteAudit(audit.first);
    }

    QList<unsigned int> listOfTrees=PCx_Tree::getListOfTreesId();
    foreach(unsigned int tree,listOfTrees)
    {
        PCx_Tree::deleteTree(tree);
    }
}


QByteArray ProtonClassicSuiteUnitTests::hashString(const QString &str)
{
    QByteArray hash = QCryptographicHash::hash(
        QByteArray::fromRawData((const char*)str.utf16(), str.length()*2),
        QCryptographicHash::Sha256
      );

   /* QDataStream stream(hash);
      quint64 a, b;
      stream >> a >> b;
      return a ^ b;*/
    return hash;
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
