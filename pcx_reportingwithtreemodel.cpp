#include "pcx_reportingwithtreemodel.h"

PCx_ReportingWithTreeModel::PCx_ReportingWithTreeModel(unsigned int reportingId):PCx_Reporting(reportingId)
{
    attachedTree=new PCx_TreeModel(attachedTreeId);
}

PCx_ReportingWithTreeModel::~PCx_ReportingWithTreeModel()
{
    delete attachedTree;
}
