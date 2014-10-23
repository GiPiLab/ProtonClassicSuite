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
    PCx_AuditWithTreeModel(const PCx_AuditWithTreeModel &c);
    PCx_AuditWithTreeModel &operator=(const PCx_AuditWithTreeModel &);
};

#endif // PCX_AUDITWITHTREEMODEL_H
