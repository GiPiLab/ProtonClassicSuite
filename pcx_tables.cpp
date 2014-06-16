#include "pcx_tables.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PCx_Tables::PCx_Tables(PCx_AuditModel *model)
{
    this->model=model;
}

//NOTE : Perhaps separate HTML generation from model (as in a true MVC approach), but it may decrease performances for the same result

QString PCx_Tables::getT1(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table align='center' width='98%' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=8><b>%1 (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th>"
                           "<th>R&eacute;alis&eacute;</th><th>%/pr&eacute;vu</th>"
                           "<th>Engag&eacute;</th><th>%/pr&eacute;vu</th><th>Disponible</th><th>%/pr&eacute;vu</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();

        double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

        if(ouverts!=0.0)
        {
            percentRealisesOuverts=realises*100/ouverts;
            percentEngagesOuverts=engages*100/ouverts;
            percentDisponiblesOuverts=disponibles*100/ouverts;
        }
        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>%2</td><td align='right' class='t1valeur'>%3</td>"
                              "<td align='right' class='t1pourcent'>%4\%</td><td align='right' class='t1valeur'>%5</td><td align='right' class='t1pourcent'>%6\%</td>"
                              "<td align='right' class='t1valeur'>%7</td><td align='right' class='t1pourcent'>%8\%</td></tr>").arg(q.value("annee").toUInt())
                      .arg(formatDouble(ouverts)).arg(formatDouble(realises)).arg(formatDouble(percentRealisesOuverts))
                      .arg(formatDouble(engages)).arg(formatDouble(percentEngagesOuverts))
                      .arg(formatDouble(disponibles)).arg(formatDouble(percentDisponiblesOuverts)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT2(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution cumul&eacute;e du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de [ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));


    QMap<unsigned int,double> ouvertsRoot,realisesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
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
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0;
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
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
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(!qFuzzyIsNull(diffFirstYearRootNodeOuverts))
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/diffFirstYearRootNodeOuverts;
            }
            if(!qFuzzyIsNull(diffFirstYearRootNodeRealises))
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT2bis(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    //The classes "t3xxx" are not a mistake
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de<br>[ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));


    QMap<unsigned int,double> ouvertsRoot,realisesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
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
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0;
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
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
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(!qFuzzyIsNull(diffFirstYearRootNodeOuverts))
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/diffFirstYearRootNodeOuverts;
            }
            if(!qFuzzyIsNull(diffFirstYearRootNodeRealises))
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

            //Here is the trick between T2 and T2bis, change the reference each year (the css changes also)
            diffFirstYearRootNodeOuverts=diffRootNodeOuverts;
            diffFirstYearRootNodeRealises=diffRootNodeRealises;
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT3(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    //The classes "t2xxx" are not a mistake
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution cumul&eacute;e du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;

    while(q.next())
    {
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t2annee'>%1</td><td class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(!qFuzzyIsNull(firstYearOuvertsNode))
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/firstYearOuvertsNode;
            }
            if(!qFuzzyIsNull(firstYearRealisesNode))
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT3bis(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>&Eacute;volution du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;

    while(q.next())
    {
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t3annee'>%1</td><td class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(!qFuzzyIsNull(firstYearOuvertsNode))
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/firstYearOuvertsNode;
            }
            if(!qFuzzyIsNull(firstYearRealisesNode))
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

            //Here is the only difference between T3 and T3bis (appart the styling of tables)
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT4(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table align='center' width='65%' cellpadding='5'>"
                           "<tr class='t4entete'><td colspan=3 align='center'><b>Poids relatif de [ %1 ]<br>au sein de la Collectivit&eacute; "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t4entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QHash<unsigned int,double> ouvertsRoot,realisesRoot;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        ouvertsRoot.insert(q.value("annee").toUInt(),q.value("ouverts").toDouble());
        realisesRoot.insert(q.value("annee").toUInt(),q.value("realises").toDouble());
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        unsigned int annee=q.value("annee").toUInt();
        Q_ASSERT(annee>0);

        double percentOuvertsRoot=0.0,percentRealisesRoot=0.0;

        if(!qFuzzyIsNull(ouvertsRoot[annee]))
        {
            percentOuvertsRoot=ouverts*100/ouvertsRoot[annee];
        }
        if(!qFuzzyIsNull(realisesRoot[annee]))
        {
            percentRealisesRoot=realises*100/realisesRoot[annee];
        }

        output.append(QString("<tr><td class='t4annee'>%1</td><td align='right' class='t4pourcent'>%2\%</td><td align='right' class='t4pourcent'>"
                              "%3\%</td></tr>").arg(annee).arg(formatDouble(percentOuvertsRoot)).arg(formatDouble(percentRealisesRoot)));

    }

    output.append("</table>");
    return output;
}


QString PCx_Tables::getT5(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t5entete'><td colspan=6 align='center'>"
                           "<b>Analyse en base 100 du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de<br>[ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t5entete'><th valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Pr&eacute;vu</th>"
                           "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr>"
                           "<tr class='t5entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th style='font-weight:normal'>dont<br>disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QMap<unsigned int,double> ouvertsRoot,realisesRoot,engagesRoot,disponiblesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0,firstYearEngagesRoot=0.0,firstYearDisponiblesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
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
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0,firstYearEngagesNode=0.0,firstYearDisponiblesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0,diffFirstYearRootNodeEngages=0.0,
            diffFirstYearRootNodeDisponibles=0.0,diffFirstYearRootNodeNC=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0,diffRootNodeEngages=0.0,diffRootNodeDisponibles=0.0,diffRootNodeNC=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
        double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0,percentNC=0.0;
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
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffRootNodeEngages=engagesRoot[annee]-engages;
            diffRootNodeDisponibles=disponiblesRoot[annee]-disponibles;
            diffRootNodeNC=diffRootNodeEngages+diffRootNodeDisponibles;

            if(!qFuzzyIsNull(diffFirstYearRootNodeOuverts*diffRootNodeOuverts))
                percentOuverts=100/diffFirstYearRootNodeOuverts*diffRootNodeOuverts;

            if(!qFuzzyIsNull(diffFirstYearRootNodeRealises*diffRootNodeRealises))
                percentRealises=100/diffFirstYearRootNodeRealises*diffRootNodeRealises;

            if(!qFuzzyIsNull(diffFirstYearRootNodeEngages*diffRootNodeEngages))
                percentEngages=100/diffFirstYearRootNodeEngages*diffRootNodeEngages;

            if(!qFuzzyIsNull(diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles))
                percentDisponibles=100/diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles;

            if(!qFuzzyIsNull(diffRootNodeNC+diffFirstYearRootNodeNC))
                percentNC=100/diffFirstYearRootNodeNC*diffRootNodeNC;


            output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' class='t5pourcent'>%2</td>"
                                  "<td align='right' class='t5pourcent'>%3</td><td align='right' class='t5pourcent'>%4</td>"
                                  "<td align='right' class='t5valeur'>%5</td><td align='right' class='t5valeur'>%6</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,0)).arg(formatDouble(percentRealises,0)).arg(formatDouble(percentNC,0))
                          .arg(formatDouble(percentEngages,0)).arg(formatDouble(percentDisponibles,0)));
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT6(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t6entete'><td colspan=6 align='center'>"
                           "<b>Analyse en base 100 du compte administratif de [ %1 ]"
                           "<br>(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t6entete'><th valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Pr&eacute;vu</th>"
                           "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr>"
                           "<tr class='t6entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th style='font-weight:normal'>dont<br>disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0,firstYearEngagesNode=0.0,firstYearDisponiblesNode=0.0,firstYearNodeNC=0.0;

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
        double nc=engages+disponibles;
        double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0,percentNC=0.0;
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
            if(!qFuzzyIsNull(ouverts*firstYearOuvertsNode))
                percentOuverts=100/firstYearOuvertsNode*ouverts;

            if(!qFuzzyIsNull(realises*firstYearRealisesNode))
                percentRealises=100/firstYearRealisesNode*realises;

            if(!qFuzzyIsNull(engages*firstYearEngagesNode))
                percentEngages=100/firstYearEngagesNode*engages;

            if(!qFuzzyIsNull(disponibles*firstYearDisponiblesNode))
                percentDisponibles=100/firstYearDisponiblesNode*disponibles;

            if(!qFuzzyIsNull(nc*firstYearNodeNC))
                percentNC=100/firstYearNodeNC*nc;


            output.append(QString("<tr><td class='t6annee'>%1</td><td align='right' class='t6pourcent'>%2</td>"
                                  "<td align='right' class='t6pourcent'>%3</td><td align='right' class='t6pourcent'>%4</td>"
                                  "<td align='right' class='t6valeur'>%5</td><td align='right' class='t6valeur'>%6</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,0)).arg(formatDouble(percentRealises,0)).arg(formatDouble(percentNC,0))
                          .arg(formatDouble(percentEngages,0)).arg(formatDouble(percentDisponibles,0)));
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT7(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t7entete'><td colspan=5 align='center'>"
                           "<b>Transcription en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; de<br>"
                           "[ %1 ] (<span style='color:#7c0000'>%2</span>)<br><u>sur la base d'une &eacute;quivalence de"
                           " 365/365<sup>&egrave;me</sup> entre le r&eacute;alis&eacute; budg&eacute;taire et l'ann&eacute;e</u></b>"
                           "<tr class='t7entete'><th>Exercice</th><th>Pr&eacute;vu</th><th>R&eacute;alis&eacute;</th>"
                           "<th>Engag&eacute;</th><th>Disponible</th></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
        double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0;

        if(!qFuzzyIsNull(realises))
        {
            percentOuverts=365*ouverts/realises;
            percentRealises=365.0;
            percentEngages=365*engages/realises;
            percentDisponibles=365*disponibles/realises;
        }
        output.append(QString("<tr><td class='t7annee'>%1</td><td align='right' class='t7pourcent'>%2</td>"
                              "<td align='right' class='t7pourcent'>%3</td><td align='right' class='t7pourcent'>%4</td>"
                              "<td align='right' class='t7pourcent'>%5</td></tr>")
                      .arg(annee).arg(formatDouble(percentOuverts,0)).arg(formatDouble(percentRealises,0))
                      .arg(formatDouble(percentEngages,0)).arg(formatDouble(percentDisponibles,0)));
    }
    output.append("</table>");
    return output;
}

QString PCx_Tables::getT8(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());
    QString output=QString("\n<table width='50%' align='center' cellpadding='5'>"
                           "<tr class='t8entete'><td colspan=2 align='center'><b>Moyennes budg&eacute;taires de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b></td></tr>")
            .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    double sumOuverts=0.0,sumRealises=0.0,sumEngages=0.0,sumDisponibles=0.0;

    while(q.next())
    {
        sumOuverts+=q.value("ouverts").toDouble();
        sumRealises+=q.value("realises").toDouble();
        sumEngages+=q.value("engages").toDouble();
        sumDisponibles+=q.value("disponibles").toDouble();
    }

    double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

    if(!qFuzzyIsNull(sumOuverts))
    {
        percentRealisesOuverts=sumRealises*100/sumOuverts;
        percentEngagesOuverts=sumEngages*100/sumOuverts;
        percentDisponiblesOuverts=sumDisponibles*100/sumOuverts;
    }

    output.append(QString("<tr><td class='t8annee' align='center'>R&eacute;alis&eacute;</td><td align='right' class='t8pourcent'>%1\%</td></tr>"
                          "<tr><td class='t8annee' align='center' style='font-weight:normal'>Non utilis&eacute;</td><td align='right' class='t8valeur'>%2\%</td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont engag&eacute;</i></td><td align='right' class='t8pourcent'><i>%3\%</i></td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont disponible</i></td><td align='right' class='t8pourcent'><i>%4\%</i></td></tr>")
                  .arg(formatDouble(percentRealisesOuverts)).arg(formatDouble(percentDisponiblesOuverts+percentEngagesOuverts))
                  .arg(formatDouble(percentEngagesOuverts)).arg(formatDouble(percentDisponiblesOuverts)));

    output.append("</table>");
    return output;
}

QString PCx_Tables::getT9(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(model->modeToTableString(mode)).arg(model->getAuditId());

    QString output=QString("\n<table width='60%' align='center' cellpadding='5'>"
                           "<tr class='t9entete'><td colspan=2 align='center'>"
                           "<b>&Eacute;quivalences moyennes en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; de<br>[ %1 ]"
                           " (<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b>"
                           "</td></tr>").arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped()).arg(model->modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    double sumOuverts=0.0,sumRealises=0.0,sumEngages=0.0,sumDisponibles=0.0;
    double percentOuverts=0.0,percentNC=0.0,percentEngages=0.0,percentDisponibles=0.0;

    while(q.next())
    {
        sumOuverts+=q.value("ouverts").toDouble();
        sumRealises+=q.value("realises").toDouble();
        sumEngages+=q.value("engages").toDouble();
        sumDisponibles+=q.value("disponibles").toDouble();
    }
    if(!qFuzzyIsNull(sumRealises))
    {
        percentOuverts=sumOuverts*365/sumRealises;
        percentEngages=sumEngages*365/sumRealises;
        percentDisponibles=sumDisponibles*365/sumRealises;
        percentNC=percentEngages+percentDisponibles;
    }
    output.append(QString("<tr><td colspan='2' align='center' class='t9annee'>Face aux cr&eacute;dits r&eacute;alis&eacute;s, les pr&eacute;visions correspondent"
                          " &agrave; :<br><b>%1 jours de travail par an</b></td></tr>"
                          "<tr><td class='t9annee' style='font-weight:normal'>D&eacute;passement de capacit&eacute; des pr&eacute;visions</td><td align='right' class='t9valeur' valign='middle'>%2 jours</td></tr>"
                          "<tr><td class='t9annee'><i>dont engag&eacute;</i></td><td class='t9pourcent' align='right' valign='middle'><i>%3 jours</i></td></tr>"
                          "<tr><td class='t9annee'><i>dont disponible</i></td><td class='t9pourcent' align='right' valign='middle'><i>%4 jours</i></td></tr>")
                  .arg(formatDouble(percentOuverts,0)).arg(formatDouble(percentNC,0))
                  .arg(formatDouble(percentEngages,0)).arg(formatDouble(percentDisponibles,0)));
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
    q.prepare(QString("select a.annee,coalesce(a.ouverts,0.0)-coalesce(b.ouverts,0.0) as diff_ouverts, "
                      "coalesce(a.realises,0.0)-coalesce(b.realises,0.0) as diff_realises, "
                      "coalesce(a.engages,0.0)-coalesce(b.engages,0.0) as diff_engages "
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
        double diff_ouverts=q.value("diff_ouverts").toDouble();
        double diff_realises=q.value("diff_realises").toDouble();
        double diff_engages=q.value("diff_engages").toDouble();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(qFuzzyIsNull(diff_ouverts))
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_ouverts)));

        if(qFuzzyIsNull(diff_realises))
            output.append("<span style='color:black'>");
        else if(diff_realises>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_realises)));

        if(qFuzzyIsNull(diff_engages))
            output.append("<span style='color:black'>");
        else if(diff_engages>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatDouble(diff_engages)));
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
    q.prepare(QString("select a.annee,coalesce(a.ouverts,0.0)-coalesce(b.ouverts,0.0) as diff_ouverts, "
                      "coalesce(a.realises,0.0)-coalesce(b.realises,0.0) as diff_realises, "
                      "coalesce(a.engages,0.0)-coalesce(b.engages,0.0) as diff_engages "
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
        double diff_ouverts=q.value("diff_ouverts").toDouble();
        double diff_realises=q.value("diff_realises").toDouble();
        double diff_engages=q.value("diff_engages").toDouble();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(qFuzzyIsNull(diff_ouverts))
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_ouverts)));

        if(qFuzzyIsNull(diff_realises))
            output.append("<span style='color:black'>");
        else if(diff_realises>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_realises)));

        if(qFuzzyIsNull(diff_engages))
            output.append("<span style='color:black'>");
        else if(diff_engages>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatDouble(diff_engages)));
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
                      "(coalesce(a.ouverts,0.0)-coalesce(b.ouverts,0.0)) "
                      " + (coalesce(c.ouverts,0.0)-coalesce(d.ouverts,0.0)) as diff_ouverts, "
                      "(coalesce(a.realises,0.0)-coalesce(b.realises,0.0)) "
                      " + (coalesce(c.realises,0.0)-coalesce(d.realises,0.0)) as diff_realises, "
                      "(coalesce(a.engages,0.0)-coalesce(b.engages,0.0)) "
                      " + (coalesce(c.engages,0.0)-coalesce(d.engages,0.0)) as diff_engages "
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
        double diff_ouverts=q.value("diff_ouverts").toDouble();
        double diff_realises=q.value("diff_realises").toDouble();
        double diff_engages=q.value("diff_engages").toDouble();


        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>").arg(annee));
        if(qFuzzyIsNull(diff_ouverts))
            output.append("<span style='color:black'>");
        else if(diff_ouverts>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_ouverts)));

        if(qFuzzyIsNull(diff_realises))
            output.append("<span style='color:black'>");
        else if(diff_realises>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatDouble(diff_realises)));

        if(qFuzzyIsNull(diff_engages))
            output.append("<span style='color:black'>");
        else if(diff_engages>0.0)
            output.append("<span style='color:#008000'>");
        else
            output.append("<span style='color:#7c0000'>");

        output.append(QString("%1</span></td></tr>").arg(formatDouble(diff_engages)));
    }

    output.append("</table>");
    return output;
}

QString PCx_Tables::getTabRecap(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    QString out=getT1(node,mode)+"<br>\n"+getT4(node,mode)+"<br>\n"+getT8(node,mode)+"<br>\n";
    return out;
}

QString PCx_Tables::getTabResults(unsigned int node) const
{
    QString out=getT10(node)+"<br>\n"+getT11(node)+"<br>\n"+getT12(node)+"<br>\n";
    return out;
}

QString PCx_Tables::getTabEvolution(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    QString out=getT2bis(node,mode)+"<br>\n"+getT3bis(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabEvolutionCumul(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    QString out=getT2(node,mode)+"<br>\n"+getT3(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabBase100(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    QString out=getT5(node,mode)+"<br>\n"+getT6(node,mode)+"<br>\n";
    return out;
}


QString PCx_Tables::getTabJoursAct(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const
{
    QString out=getT7(node,mode)+"<br>\n"+getT9(node,mode)+"<br>\n";
    return out;
}




