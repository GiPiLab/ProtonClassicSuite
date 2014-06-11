#ifndef PCX_AUDITMODEL_H
#define PCX_AUDITMODEL_H

#include <QtGui>
#include <QtSql>
#include "QCustomPlot/qcustomplot.h"
#include "pcx_auditinfos.h"
#include "pcx_treemodel.h"

//Column indexes in sql tables
#define COL_ID 0
#define COL_IDNODE 1
#define COL_ANNEE 2
#define COL_OUVERTS 3
#define COL_REALISES 4
#define COL_ENGAGES 5
#define COL_DISPONIBLES 6

//MASKS FOR TABLE AND GRAPHICS BITSETS
#define TABRESULTS 1
#define TABRECAP 2
#define TABEVOLUTION 4
#define TABEVOLUTIONCUMUL 8
#define TABBASE100 16
#define TABJOURSACT 32

#define T1 1
#define T2 2
#define T2BIS 4
#define T3 8
#define T3BIS 16
#define T4 32
#define T5 64
#define T6 128
#define T7 256
#define T8 512
#define T9 1024
#define T10 2048
#define T11 4096
#define T12 8192

#define G1 1
#define G2 2
#define G3 4
#define G4 8
#define G5 16
#define G6 32
#define G7 64
#define G8 128
#define G9 256

enum DFRFDIRI
{
    DF,
    RF,
    DI,
    RI,
    GLOBAL
};

enum ORED
{
    ouverts,
    realises,
    engages,
    disponibles
};

enum ListAuditsMode
{
    FinishedAuditsOnly,
    UnFinishedAuditsOnly,
    AllAudits
};


class
        PCx_AuditModel : public QObject
{
    Q_OBJECT

public:

    explicit PCx_AuditModel(unsigned int auditId,QObject *parent = 0,bool readOnly=true);
    virtual ~PCx_AuditModel();

    static bool addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);

    unsigned int getAuditId() const{return auditId;}
    const PCx_AuditInfos &getAuditInfos() const{return auditInfos;}

    bool finishAudit();
    static bool finishAudit(unsigned int id);

    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    QSqlTableModel *getTableModel(const QString &mode) const;
    QSqlTableModel *getTableModel(DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);

    QString getTabJoursAct(unsigned int node, DFRFDIRI mode) const;
    QString getTabBase100(unsigned int node, DFRFDIRI mode) const;
    QString getTabEvolutionCumul(unsigned int node, DFRFDIRI mode) const;
    QString getTabEvolution(unsigned int node, DFRFDIRI mode) const;
    QString getTabRecap(unsigned int node, DFRFDIRI mode) const;
    QString getTabResults(unsigned int node) const;

    QString getCSS() const;

    QString modetoTableString(DFRFDIRI mode) const;
    QString modeToCompleteString(DFRFDIRI mode) const;
    QString OREDtoCompleteString(ORED ored) const;
    QString OREDtoTableString(ORED ored) const;

    //T1,T2... are named as in the original PCA version
    //Content of tab "Recap"
    QString getT1(unsigned int node, DFRFDIRI mode) const;
    QString getT4(unsigned int node, DFRFDIRI mode) const;
    QString getT8(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Evolution"
    QString getT2bis(unsigned int node, DFRFDIRI mode) const;
    QString getT3bis(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "EvolutionCumul"
    QString getT2(unsigned int node, DFRFDIRI mode) const;
    QString getT3(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Base100"
    QString getT5(unsigned int node, DFRFDIRI mode) const;
    QString getT6(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Jours act."
    QString getT7(unsigned int node, DFRFDIRI mode) const;
    QString getT9(unsigned int node, DFRFDIRI mode) const;

    //Content of "Resultats" mode (without tabs)
    QString getT10(unsigned int node) const;
    QString getT11(unsigned int node) const;
    QString getT12(unsigned int node) const;

    QString getG1(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,ouverts,false,plot);}
    QString getG3(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,realises,false,plot);}
    QString getG5(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,engages,false,plot);}
    QString getG7(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,disponibles,false,plot);}

    QString getG2(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,ouverts,true,plot);}
    QString getG4(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,realises,true,plot);}
    QString getG6(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,engages,true,plot);}
    QString getG8(unsigned int node,DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,disponibles,true,plot);}

    QString getG9(unsigned int node, QCustomPlot *plot) const;

    QString generateHTMLReportForNode(quint8 bitFieldPagesOfTables, quint16 bitFieldTables, quint16 bitFieldGraphics, unsigned int selectedNode, DFRFDIRI mode,
                                      QCustomPlot *plot, unsigned int favoriteGraphicsWidth, unsigned int favoriteGraphicsHeight, double scale=1.0, QTextDocument *document=NULL,
                                      const QString &absoluteImagePath="", const QString &relativeImagePath="",QProgressDialog *progress=NULL) const;

signals:

public slots:

private:
    unsigned int auditId;

    PCx_AuditInfos auditInfos;
    PCx_TreeModel *attachedTree;

    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    bool loadFromDb(unsigned int auditId, bool readOnly=true);
    bool propagateToAncestors(const QModelIndex &node);
    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);

    //Returns the title of the graphic in html
    QString getG1G8(unsigned int node, DFRFDIRI mode, ORED modeORED, bool cumule, QCustomPlot *plot) const;


private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};




#endif // PCX_AUDITMODEL_H
