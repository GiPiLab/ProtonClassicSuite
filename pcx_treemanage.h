#ifndef PCX_TREEMANAGE_H
#define PCX_TREEMANAGE_H

#include <QString>
#include <QPair>

namespace PCx_TreeManage{

    int importTreeFromTSV(const QString &fileName,const QString &treeName);
    int createRandomTree(const QString &name, unsigned int nbNodes);
    int duplicateTree(unsigned int treeId,const QString &newName);
    int addTree(const QString &name, bool createRoot=true, bool addTypes=true);
    int deleteTree(unsigned int treeId);
    QList<QPair<unsigned int,QString> > getListOfTrees(bool finishedOnly=false);
    bool treeNameExists(const QString &name);
    QString idTreeToName(unsigned int treeId);
}



#endif // PCX_TREEMANAGE_H
