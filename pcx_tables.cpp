#include "pcx_tables.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PCx_Tables::PCx_Tables(PCx_Audit *model):model(model)
{
    Q_ASSERT(model!=nullptr);
}


QString PCx_Tables::getT1(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table align='center' width='98%' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=8><b>%1 (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th>"
                           "<th>R&eacute;alis&eacute;</th><th>%/pr&eacute;vu</th>"
                           "<th>Engag&eacute;</th><th>%/pr&eacute;vu</th><th>Disponible</th><th>%/pr&eacute;vu</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        qint64 engages=q.value("engages").toLongLong();
        qint64 disponibles=q.value("disponibles").toLongLong();

        double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

        if(ouverts!=0)
        {
            percentRealisesOuverts=realises*100.0/ouverts;
            percentEngagesOuverts=engages*100.0/ouverts;
            percentDisponiblesOuverts=disponibles*100.0/ouverts;
        }
        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>%2</td><td align='right' class='t1valeur'>%3</td>"
                              "<td align='right' class='t1pourcent'>%4\%</td><td align='right' class='t1valeur'>%5</td><td align='right' class='t1pourcent'>%6\%</td>"
                              "<td align='right' class='t1valeur'>%7</td><td align='right' class='t1pourcent'>%8\%</td></tr>").arg(q.value("annee").toUInt())
                      .arg(formatCurrency(ouverts)).arg(formatCurrency(realises)).arg(formatDouble(percentRealisesOuverts,-1,true))
                      .arg(formatCurrency(engages)).arg(formatDouble(percentEngagesOuverts,-1,true))
                      .arg(formatCurrency(disponibles)).arg(formatDouble(percentDisponiblesOuverts,-1,true)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT2(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution cumul&eacute;e du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de [ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));


    QMap<unsigned int,qint64> ouvertsRoot,realisesRoot;
    qint64 firstYearOuvertsRoot=0,firstYearRealisesRoot=0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        ouvertsRoot.insert(q.value("annee").toUInt(),ouverts);
        realisesRoot.insert(q.value("annee").toUInt(),realises);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0;
    qint64 diffFirstYearRootNodeOuverts=0,diffFirstYearRootNodeRealises=0;


    while(q.next())
    {
        qint64 diffRootNodeOuverts=0,diffRootNodeRealises=0;
        qint64 diffCurrentYearFirstYearOuverts=0,diffCurrentYearFirstYearRealises=0;

        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();

        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            output.append(QString("<tr><td class='t2annee'>%1</td><td class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            double percentOuverts=0.0,percentRealises=0.0;
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(diffFirstYearRootNodeOuverts!=0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100.0/diffFirstYearRootNodeOuverts;
            }
            if(diffFirstYearRootNodeRealises!=0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100.0/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,-1,true)).arg(formatDouble(percentRealises,-1,true)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT2bis(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    //The classes "t3xxx" are not a mistake
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de<br>[ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));


    QMap<unsigned int,qint64> ouvertsRoot,realisesRoot;
    qint64 firstYearOuvertsRoot=0,firstYearRealisesRoot=0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        ouvertsRoot.insert(q.value("annee").toUInt(),ouverts);
        realisesRoot.insert(q.value("annee").toUInt(),realises);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0;
    qint64 diffFirstYearRootNodeOuverts=0,diffFirstYearRootNodeRealises=0;


    while(q.next())
    {
        qint64 diffRootNodeOuverts=0,diffRootNodeRealises=0;
        qint64 diffCurrentYearFirstYearOuverts=0,diffCurrentYearFirstYearRealises=0;

        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            //ouverts_1 in PCA
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            output.append(QString("<tr><td class='t3annee'>%1</td><td class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            //ouverts_2
            double percentOuverts=0.0,percentRealises=0.0;
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(diffFirstYearRootNodeOuverts!=0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100.0/diffFirstYearRootNodeOuverts;
            }
            if(diffFirstYearRootNodeRealises!=0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100.0/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,-1,true)).arg(formatDouble(percentRealises,-1,true)));

            //Here is the trick between T2 and T2bis, change the reference each year (the css changes also)
            diffFirstYearRootNodeOuverts=diffRootNodeOuverts;
            diffFirstYearRootNodeRealises=diffRootNodeRealises;
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT3(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    //The classes "t2xxx" are not a mistake
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution cumul&eacute;e du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0;

    while(q.next())
    {
        qint64 diffCurrentYearFirstYearOuverts=0,diffCurrentYearFirstYearRealises=0;

        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t2annee'>%1</td><td class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            double percentOuverts=0.0,percentRealises=0.0;
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(firstYearOuvertsNode!=0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100.0/firstYearOuvertsNode;
            }
            if(firstYearRealisesNode!=0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100.0/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,-1,true)).arg(formatDouble(percentRealises,-1,true)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT3bis(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0;

    while(q.next())
    {
        qint64 diffCurrentYearFirstYearOuverts=0,diffCurrentYearFirstYearRealises=0;

        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t3annee'>%1</td><td class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            double percentOuverts=0.0,percentRealises=0.0;
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(firstYearOuvertsNode!=0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100.0/firstYearOuvertsNode;
            }
            if(firstYearRealisesNode!=0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100.0/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,-1,true)).arg(formatDouble(percentRealises,-1,true)));

            //Here is the only difference between T3 and T3bis (appart the styling of tables)
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT4(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table align='center' width='65%' cellpadding='5'>"
                           "<tr class='t4entete'><td colspan=3 align='center'><b>Poids relatif de [ %1 ]<br>au sein de la Collectivit&eacute; "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t4entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QHash<unsigned int,qint64> ouvertsRoot,realisesRoot;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        ouvertsRoot.insert(q.value("annee").toUInt(),q.value("ouverts").toLongLong());
        realisesRoot.insert(q.value("annee").toUInt(),q.value("realises").toLongLong());
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        unsigned int annee=q.value("annee").toUInt();
        Q_ASSERT(annee>0);

        double percentOuvertsRoot=0.0,percentRealisesRoot=0.0;

        if(ouvertsRoot[annee]!=0)
        {
            percentOuvertsRoot=ouverts*100.0/ouvertsRoot[annee];
        }
        if(realisesRoot[annee]!=0)
        {
            percentRealisesRoot=realises*100.0/realisesRoot[annee];
        }

        output.append(QString("<tr><td class='t4annee'>%1</td><td align='right' class='t4pourcent'>%2\%</td><td align='right' class='t4pourcent'>"
                              "%3\%</td></tr>").arg(annee).arg(formatDouble(percentOuvertsRoot,-1,true)).arg(formatDouble(percentRealisesRoot,-1,true)));

    }

    output.append("</table>");
    return output;
}


QString PCx_Tables::getT5(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t5entete'><td colspan=6 align='center'>"
                           "<b>Analyse en base 100 du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de<br>[ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t5entete'><th valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Pr&eacute;vu</th>"
                           "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr>"
                           "<tr class='t5entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th style='font-weight:normal'>dont<br>disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QMap<unsigned int,qint64> ouvertsRoot,realisesRoot,engagesRoot,disponiblesRoot;
    qint64 firstYearOuvertsRoot=0,firstYearRealisesRoot=0,firstYearEngagesRoot=0,firstYearDisponiblesRoot=0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        qint64 engages=q.value("engages").toLongLong();
        qint64 disponibles=q.value("disponibles").toLongLong();
        unsigned int annee=q.value("annee").toUInt();
        ouvertsRoot.insert(annee,ouverts);
        realisesRoot.insert(annee,realises);
        engagesRoot.insert(annee,engages);
        disponiblesRoot.insert(annee,disponibles);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            firstYearEngagesRoot=engages;
            firstYearDisponiblesRoot=disponibles;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0,firstYearEngagesNode=0,firstYearDisponiblesNode=0;
    qint64 diffFirstYearRootNodeOuverts=0,diffFirstYearRootNodeRealises=0,diffFirstYearRootNodeEngages=0,
            diffFirstYearRootNodeDisponibles=0,diffFirstYearRootNodeNC=0;


    while(q.next())
    {
        qint64 diffRootNodeOuverts=0,diffRootNodeRealises=0,diffRootNodeEngages=0,diffRootNodeDisponibles=0,diffRootNodeNC=0;

        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        qint64 engages=q.value("engages").toLongLong();
        qint64 disponibles=q.value("disponibles").toLongLong();
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            firstYearEngagesNode=engages;
            firstYearDisponiblesNode=disponibles;

            //ouverts_1
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            diffFirstYearRootNodeEngages=firstYearEngagesRoot-firstYearEngagesNode;
            diffFirstYearRootNodeDisponibles=firstYearDisponiblesRoot-firstYearDisponiblesNode;
            diffFirstYearRootNodeNC=diffFirstYearRootNodeEngages+diffFirstYearRootNodeDisponibles;

            output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' class='t5pourcent'>100</td>"
                                  "<td align='right' class='t5pourcent'>100</td><td align='right' class='t5pourcent'>100</td>"
                                  "<td align='right' class='t5valeur'>100</td><td align='right' class='t5valeur'>100</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            //ouverts_2
            double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0,percentNC=0.0;
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffRootNodeEngages=engagesRoot[annee]-engages;
            diffRootNodeDisponibles=disponiblesRoot[annee]-disponibles;
            diffRootNodeNC=diffRootNodeEngages+diffRootNodeDisponibles;

            if(diffFirstYearRootNodeOuverts*diffRootNodeOuverts!=0)
                percentOuverts=100.0/diffFirstYearRootNodeOuverts*diffRootNodeOuverts;

            if(diffFirstYearRootNodeRealises*diffRootNodeRealises!=0)
                percentRealises=100.0/diffFirstYearRootNodeRealises*diffRootNodeRealises;

            if(diffFirstYearRootNodeEngages*diffRootNodeEngages!=0)
                percentEngages=100.0/diffFirstYearRootNodeEngages*diffRootNodeEngages;

            if(diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles!=0)
                percentDisponibles=100.0/diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles;

            if(diffRootNodeNC+diffFirstYearRootNodeNC!=0)
                percentNC=100.0/diffFirstYearRootNodeNC*diffRootNodeNC;


            output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' class='t5pourcent'>%2</td>"
                                  "<td align='right' class='t5pourcent'>%3</td><td align='right' class='t5pourcent'>%4</td>"
                                  "<td align='right' class='t5valeur'>%5</td><td align='right' class='t5valeur'>%6</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,0,true)).arg(formatDouble(percentRealises,0,true)).arg(formatDouble(percentNC,0,true))
                          .arg(formatDouble(percentEngages,0,true)).arg(formatDouble(percentDisponibles,0,true)));
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT6(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t6entete'><td colspan=6 align='center'>"
                           "<b>Analyse en base 100 du compte administratif de [ %1 ]"
                           "<br>(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t6entete'><th valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Pr&eacute;vu</th>"
                           "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr>"
                           "<tr class='t6entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th style='font-weight:normal'>dont<br>disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    bool doneFirstForNode=false;
    qint64 firstYearOuvertsNode=0,firstYearRealisesNode=0,firstYearEngagesNode=0,firstYearDisponiblesNode=0,firstYearNodeNC=0;

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        qint64 engages=q.value("engages").toLongLong();
        qint64 disponibles=q.value("disponibles").toLongLong();
        qint64 nc=engages+disponibles;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            firstYearEngagesNode=engages;
            firstYearDisponiblesNode=disponibles;
            firstYearNodeNC=nc;

            output.append(QString("<tr><td class='t6annee'>%1</td><td align='right' class='t6pourcent'>100</td>"
                                  "<td align='right' class='t6pourcent'>100</td><td align='right' class='t6pourcent'>100</td>"
                                  "<td align='right' class='t6valeur'>100</td><td align='right' class='t6valeur'>100</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0,percentNC=0.0;
            if(ouverts*firstYearOuvertsNode!=0)
                percentOuverts=100.0/firstYearOuvertsNode*ouverts;

            if(realises*firstYearRealisesNode!=0)
                percentRealises=100.0/firstYearRealisesNode*realises;

            if(engages*firstYearEngagesNode!=0)
                percentEngages=100.0/firstYearEngagesNode*engages;

            if(disponibles*firstYearDisponiblesNode!=0)
                percentDisponibles=100.0/firstYearDisponiblesNode*disponibles;

            if(nc*firstYearNodeNC!=0)
                percentNC=100.0/firstYearNodeNC*nc;


            output.append(QString("<tr><td class='t6annee'>%1</td><td align='right' class='t6pourcent'>%2</td>"
                                  "<td align='right' class='t6pourcent'>%3</td><td align='right' class='t6pourcent'>%4</td>"
                                  "<td align='right' class='t6valeur'>%5</td><td align='right' class='t6valeur'>%6</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,0,true)).arg(formatDouble(percentRealises,0,true))
                          .arg(formatDouble(percentNC,0,true)).arg(formatDouble(percentEngages,0,true))
                          .arg(formatDouble(percentDisponibles,0,true)));
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT7(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t7entete'><td colspan=5 align='center'>"
                           "<b>Transcription en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; de<br>"
                           "[ %1 ] (<span style='color:#7c0000'>%2</span>)<br><u>sur la base d'une &eacute;quivalence de"
                           " 365/365<sup>&egrave;me</sup> entre le r&eacute;alis&eacute; budg&eacute;taire et l'ann&eacute;e</u></b>"
                           "<tr class='t7entete'><th>Exercice</th><th>Pr&eacute;vu</th><th>R&eacute;alis&eacute;</th>"
                           "<th>Engag&eacute;</th><th>Disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        qint64 ouverts=q.value("ouverts").toLongLong();
        qint64 realises=q.value("realises").toLongLong();
        qint64 engages=q.value("engages").toLongLong();
        qint64 disponibles=q.value("disponibles").toLongLong();
        double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0;

        if(realises!=0)
        {
            percentOuverts=365.0*ouverts/realises;
            percentRealises=365.0;
            percentEngages=365.0*engages/realises;
            percentDisponibles=365.0*disponibles/realises;
        }
        output.append(QString("<tr><td class='t7annee'>%1</td><td align='right' class='t7pourcent'>%2</td>"
                              "<td align='right' class='t7pourcent'>%3</td><td align='right' class='t7pourcent'>%4</td>"
                              "<td align='right' class='t7pourcent'>%5</td></tr>")
                      .arg(annee).arg(formatDouble(percentOuverts,0,true)).arg(formatDouble(percentRealises,0,true))
                      .arg(formatDouble(percentEngages,0,true)).arg(formatDouble(percentDisponibles,0,true)));
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT8(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='50%' align='center' cellpadding='5'>"
                           "<tr class='t8entete'><td colspan=2 align='center'><b>Moyennes budg&eacute;taires de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b></td></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    qint64 sumOuverts=0,sumRealises=0,sumEngages=0,sumDisponibles=0;

    while(q.next())
    {
        sumOuverts+=q.value("ouverts").toLongLong();
        sumRealises+=q.value("realises").toLongLong();
        sumEngages+=q.value("engages").toLongLong();
        sumDisponibles+=q.value("disponibles").toLongLong();
    }

    double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

    if(sumOuverts!=0)
    {
        percentRealisesOuverts=sumRealises*100.0/sumOuverts;
        percentEngagesOuverts=sumEngages*100.0/sumOuverts;
        percentDisponiblesOuverts=sumDisponibles*100.0/sumOuverts;
    }

    output.append(QString("<tr><td class='t8annee' align='center'>R&eacute;alis&eacute;</td><td align='right' class='t8pourcent'>%1\%</td></tr>"
                          "<tr><td class='t8annee' align='center' style='font-weight:normal'>Non utilis&eacute;</td><td align='right' class='t8valeur'>%2\%</td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont engag&eacute;</i></td><td align='right' class='t8pourcent'><i>%3\%</i></td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont disponible</i></td><td align='right' class='t8pourcent'><i>%4\%</i></td></tr>")
                  .arg(formatDouble(percentRealisesOuverts,-1,true)).arg(formatDouble(percentDisponiblesOuverts+percentEngagesOuverts,-1,true))
                  .arg(formatDouble(percentEngagesOuverts,-1,true)).arg(formatDouble(percentDisponiblesOuverts,-1,true)));

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT9(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='60%' align='center' cellpadding='5'>"
                           "<tr class='t9entete'><td colspan=2 align='center'>"
                           "<b>&Eacute;quivalences moyennes en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; de<br>[ %1 ]"
                           " (<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b>"
                           "</td></tr>").arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(PCx_AuditManage::modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    qint64 sumOuverts=0,sumRealises=0,sumEngages=0,sumDisponibles=0;
    double percentOuverts=0.0,percentNC=0.0,percentEngages=0.0,percentDisponibles=0.0;

    while(q.next())
    {
        sumOuverts+=q.value("ouverts").toLongLong();
        sumRealises+=q.value("realises").toLongLong();
        sumEngages+=q.value("engages").toLongLong();
        sumDisponibles+=q.value("disponibles").toLongLong();
    }
    if(sumRealises!=0)
    {
        percentOuverts=sumOuverts*365.0/sumRealises;
        percentEngages=sumEngages*365.0/sumRealises;
        percentDisponibles=sumDisponibles*365.0/sumRealises;
        percentNC=percentEngages+percentDisponibles;
    }
    output.append(QString("<tr><td colspan='2' align='center' class='t9annee'>Face aux cr&eacute;dits r&eacute;alis&eacute;s, les pr&eacute;visions correspondent"
                          " &agrave; :<br><b>%1 jours de travail par an</b></td></tr>"
                          "<tr><td class='t9annee' style='font-weight:normal'>D&eacute;passement de capacit&eacute; des pr&eacute;visions</td><td align='right' class='t9valeur' valign='middle'>%2 jours</td></tr>"
                          "<tr><td class='t9annee'><i>dont engag&eacute;</i></td><td class='t9pourcent' align='right' valign='middle'><i>%3 jours</i></td></tr>"
                          "<tr><td class='t9annee'><i>dont disponible</i></td><td class='t9pourcent' align='right' valign='middle'><i>%4 jours</i></td></tr>")
                  .arg(formatDouble(percentOuverts,0,true)).arg(formatDouble(percentNC,0,true))
                  .arg(formatDouble(percentEngages,0,true)).arg(formatDouble(percentDisponibles,0,true)));
    output.append("</table>");
    return output;
}



QString PCx_Tables::getT10(unsigned int node) const
{
    Q_ASSERT(node>0);

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=4><b>R&Eacute;SULTATS de FONCTIONNEMENT de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) </span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped());

    QSqlQuery q;
    q.prepare(QString("select a.annee,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as diff_ouverts, "
                      "coalesce(a.realises,0)-coalesce(b.realises,0) as diff_realises, "
                      "coalesce(a.engages,0)-coalesce(b.engages,0) as diff_engages "
                      "from audit_RF_%1 as a, audit_DF_%1 as b where a.id_node=:id and b.id_node=:id "
                      "and a.annee=b.annee order by a.annee").arg(model->getAuditId()));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        qint64 diff_ouverts=q.value("diff_ouverts").toLongLong();
        qint64 diff_realises=q.value("diff_realises").toLongLong();
        qint64 diff_engages=q.value("diff_engages").toLongLong();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(diff_ouverts==0)
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_ouverts)));

        if(diff_realises==0)
            output.append("<span style='color:black'>");
        else if(diff_realises>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_realises)));

        if(diff_engages==0)
            output.append("<span style='color:black'>");
        else if(diff_engages>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatCurrency(diff_engages)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT11(unsigned int node) const
{
    Q_ASSERT(node>0);

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=4><b>R&Eacute;SULTATS d'INVESTISSEMENT de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) </span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped());

    QSqlQuery q;
    q.prepare(QString("select a.annee,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as diff_ouverts, "
                      "coalesce(a.realises,0)-coalesce(b.realises,0) as diff_realises, "
                      "coalesce(a.engages,0)-coalesce(b.engages,0) as diff_engages "
                      "from audit_RI_%1 as a, audit_DI_%1 as b where a.id_node=:id and b.id_node=:id "
                      "and a.annee=b.annee order by a.annee").arg(model->getAuditId()));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        qint64 diff_ouverts=q.value("diff_ouverts").toLongLong();
        qint64 diff_realises=q.value("diff_realises").toLongLong();
        qint64 diff_engages=q.value("diff_engages").toLongLong();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(diff_ouverts==0)
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_ouverts)));

        if(diff_realises==0)
            output.append("<span style='color:black'>");
        else if(diff_realises>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_realises)));

        if(diff_engages==0)
            output.append("<span style='color:black'>");
        else if(diff_engages>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatCurrency(diff_engages)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT12(unsigned int node) const
{
    Q_ASSERT(node>0);

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=4><b>R&Eacute;SULTATS BUDG&Eacute;TAIRES de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) </span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped());

    QSqlQuery q;
    q.prepare(QString("select a.annee, "
                      "(coalesce(a.ouverts,0)-coalesce(b.ouverts,0)) "
                      " + (coalesce(c.ouverts,0)-coalesce(d.ouverts,0)) as diff_ouverts, "
                      "(coalesce(a.realises,0)-coalesce(b.realises,0)) "
                      " + (coalesce(c.realises,0)-coalesce(d.realises,0)) as diff_realises, "
                      "(coalesce(a.engages,0)-coalesce(b.engages,0)) "
                      " + (coalesce(c.engages,0)-coalesce(d.engages,0)) as diff_engages "
                      "from audit_RF_%1 as a, audit_DF_%1 as b, audit_RI_%1 as c, audit_DI_%1 as d"
                      " where a.id_node=:id and b.id_node=:id and c.id_node=:id and d.id_node=:id "
                      "and a.annee=b.annee and b.annee=c.annee and c.annee=d.annee order by a.annee").arg(model->getAuditId()));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        qint64 diff_ouverts=q.value("diff_ouverts").toLongLong();
        qint64 diff_realises=q.value("diff_realises").toLongLong();
        qint64 diff_engages=q.value("diff_engages").toLongLong();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(diff_ouverts==0)
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_ouverts)));

        if(diff_realises==0)
            output.append("<span style='color:black'>");
        else if(diff_realises>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatCurrency(diff_realises)));

        if(diff_engages==0)
            output.append("<span style='color:black'>");
        else if(diff_engages>0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatCurrency(diff_engages)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getTabRecap(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    QString out=getT1(node,mode)+"<br>\n"+getT4(node,mode)+"<br>\n"+getT8(node,mode)+"<br>\n";
    return out;
}

QString PCx_Tables::getTabResults(unsigned int node) const
{
    QString out=getT10(node)+"<br>\n"+getT11(node)+"<br>\n"+getT12(node)+"<br>\n";
    return out;
}

QString PCx_Tables::getCSS()
{
    return "\ntable{color:navy;font-weight:400;font-size:8pt;page-break-inside:avoid;}"
    "\ntd.t1annee,td.t3annee{background-color:#b3b3b3;}"
    "\ntd.t1pourcent{background-color:#b3b3b3;color:#FFF;}"
    "\ntd.t1valeur,td.t2valeur,td.t3valeur,td.t4annee,td.t4valeur,td.t4pourcent,td.t5annee,td.t7annee{background-color:#e6e6e6;}"
    "\ntd.t2annee{background-color:#b3b3b3;color:green;}"
    "\ntd.t2pourcent,td.t3pourcent{background-color:#e6e6e6;color:#000;}"
    "\ntd.t5pourcent,td.t6pourcent{background-color:#b3b3b3;color:#000;}"
    "\ntd.t5valeur,td.t6valeur,td.t7valeur{background-color:#b3b3b3;color:#000;font-weight:400;}"
    "\ntd.t6annee{background-color:#e6e6e6;color:green;}"
    "\ntd.t7pourcent{background-color:#666;color:#FFF;}"
    "\ntd.t8pourcent{background-color:#e6e6e6;text-align:center;color:#000;}"
    "\ntd.t8valeur{background-color:#e6e6e6;font-weight:400;text-align:center;color:#000;}"
    "\ntd.t9pourcent{background-color:#666;text-align:center;color:#FFF;}"
    "\ntd.t9valeur{background-color:#666;color:#FFF;font-weight:400;text-align:center;}"
    "\ntr.t1entete,tr.t3entete,td.t9annee{background-color:#b3b3b3;text-align:center;}"
    "\ntr.t2entete,td.t8annee{background-color:#b3b3b3;color:green;text-align:center;}"
    "\ntr.t4entete,tr.t5entete,tr.t7entete,tr.t9entete{background-color:#e6e6e6;text-align:center;}"
    "\ntr.t6entete{background-color:#e6e6e6;color:green;text-align:center;}"
    "\ntr.t8entete{background-color:#e6e6e6;text-align:center;color:green;}\n";
}

QString PCx_Tables::getTabEvolution(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    QString out=getT2bis(node,mode)+"<br>\n"+getT3bis(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabEvolutionCumul(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    QString out=getT2(node,mode)+"<br>\n"+getT3(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabBase100(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    QString out=getT5(node,mode)+"<br>\n"+getT6(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabJoursAct(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const
{
    QString out=getT7(node,mode)+"<br>\n"+getT9(node,mode)+"<br>\n";
    return out;
}




