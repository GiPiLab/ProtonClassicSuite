#ifndef PCX_AUDITWITHTREEMODEL_H
#define PCX_AUDITWITHTREEMODEL_H

#include "pcx_audit.h"
#include "pcx_treemodel.h"

class PCx_AuditWithTreeModel : public PCx_Audit
{
public:
    PCx_AuditWithTreeModel(unsigned int auditId);
    virtual ~PCx_AuditWithTreeModel();
    PCx_TreeModel *getAttachedTree() const{return attachedTree;}

private:
    PCx_TreeModel *attachedTree;
};

#endif // PCX_AUDITWITHTREEMODEL_H
