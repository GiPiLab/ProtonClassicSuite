#ifndef PCX_REPORTINGWITHTREEMODEL_H
#define PCX_REPORTINGWITHTREEMODEL_H

#include "pcx_treemodel.h"
#include "pcx_reporting.h"

class PCx_ReportingWithTreeModel : public PCx_Reporting
{
public:
    PCx_ReportingWithTreeModel(unsigned int reportingId);
    virtual ~PCx_ReportingWithTreeModel();

    PCx_TreeModel *getAttachedTree() const{return attachedTree;}


private:
    PCx_TreeModel *attachedTree;
    PCx_ReportingWithTreeModel(const PCx_ReportingWithTreeModel &c);
    PCx_ReportingWithTreeModel &operator=(const PCx_ReportingWithTreeModel &);

};

#endif // PCX_REPORTINGWITHTREEMODEL_H
