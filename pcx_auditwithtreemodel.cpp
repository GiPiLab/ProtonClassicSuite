#include "pcx_auditwithtreemodel.h"

PCx_AuditWithTreeModel::PCx_AuditWithTreeModel(unsigned int auditId):PCx_Audit(auditId,true)
{
    attachedTree=new PCx_TreeModel(attachedTreeId);
    attachedTreeName=attachedTree->getName();
}

PCx_AuditWithTreeModel::~PCx_AuditWithTreeModel()
{
    delete attachedTree;
}

