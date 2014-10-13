#include "pcx_auditwithtreemodel.h"

PCx_AuditWithTreeModel::PCx_AuditWithTreeModel(unsigned int auditId):PCx_Audit(auditId)
{
    attachedTree=new PCx_TreeModel(attachedTreeId);
}

PCx_AuditWithTreeModel::~PCx_AuditWithTreeModel()
{
    delete attachedTree;
}

