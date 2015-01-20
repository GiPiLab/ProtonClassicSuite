#ifndef PCX_PREVISION_H
#define PCX_PREVISION_H

#include "pcx_audit.h"
#include "pcx_previsioncriteria.h"
#include <QDateTime>

class PCx_Prevision
{
public:

    PCx_Prevision(PCx_Audit *audit);




    qint64 computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionCriteria> criteriaToAdd,
                            QList<PCx_PrevisionCriteria> criteriaToSubstract) const;

    qint64 computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode) const;




    ~PCx_Prevision();


    static void createTablePrevisions(unsigned int auditId);





private:
    PCx_Audit *audit;
    unsigned int previsionId;
    QDateTime creationTimeLocal;
    QDateTime creationTimeUTC;
    QString name;



    PCx_Prevision(const PCx_Prevision &c);
    PCx_Prevision &operator=(const PCx_Prevision &);


};



#endif // PCX_PREVISION_H
