#include "pcx_reportingwithtreemodel.h"

PCx_ReportingWithTreeModel::PCx_ReportingWithTreeModel(unsigned int reportingId):PCx_Reporting(reportingId,true)
{
    attachedTree=new PCx_TreeModel(attachedTreeId);
    attachedTreeName=attachedTree->getName();
}

PCx_ReportingWithTreeModel::~PCx_ReportingWithTreeModel()
{
    delete attachedTree;
}
