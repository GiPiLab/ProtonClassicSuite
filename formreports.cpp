#include "formreports.h"
#include "ui_formreports.h"
#include "pcx_auditmodel.h"

FormReports::FormReports(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReports)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    model=NULL;
    plot=new QCustomPlot();
    populateLists();
    updateListOfAudits();
}

FormReports::~FormReports()
{
    delete ui;
    if(model!=NULL)
    {
        delete model;
        delete report;
    }
    delete plot;
}

void FormReports::populateLists()
{
    QListWidgetItem *item;
    item=new QListWidgetItem(tr("Récapitulatif"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T1);
    item=new QListWidgetItem(tr("Évolution cumulée du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T2);
    item=new QListWidgetItem(tr("Évolution du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T2BIS);
    item=new QListWidgetItem(tr("Évolution cumulée du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T3);
    item=new QListWidgetItem(tr("Évolution du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T3BIS);
    item=new QListWidgetItem(tr("Poids relatif de [...] au sein de la collectivité"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T4);
    item=new QListWidgetItem(tr("Analyse en base 100 du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T5);
    item=new QListWidgetItem(tr("Analyse en base 100 du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T6);
    item=new QListWidgetItem(tr("Transcription en \"jours/activité\" de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T7);
    item=new QListWidgetItem(tr("Moyennes budgétaires de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T8);
    item=new QListWidgetItem(tr("Equivalences moyennes en \"jours activité\" de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T9);
    item=new QListWidgetItem(tr("Résultats de fonctionnement de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T10);
    item=new QListWidgetItem(tr("Résultats d'investissement de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T11);
    item=new QListWidgetItem(tr("Résultats budgétaire de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,PCx_Tables::T12);

    item=new QListWidgetItem(tr("Évolution comparée du prévu de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G1);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du prévu de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G2);
    item=new QListWidgetItem(tr("Évolution comparée du réalisé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G3);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du réalisé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G4);
    item=new QListWidgetItem(tr("Évolution comparée de l'engagé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G5);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé de l'engagé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G6);
    item=new QListWidgetItem(tr("Évolution comparée du disponible de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G7);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du disponible de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G8);
    item=new QListWidgetItem(tr("Décomposition par année"),ui->listGraphics);
    item->setData(Qt::UserRole+1,PCx_Graphics::G9);
}

void FormReports::onListOfAuditsChanged()
{
    updateListOfAudits();
}


void FormReports::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(PCx_AuditModel::FinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void FormReports::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
        delete report;
    }
    model=new PCx_AuditModel(selectedAuditId,this);
    report=new PCx_Report(model);
    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(model->getAttachedTreeModel());
    delete m;
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);

    ui->comboListTypes->clear();

    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTreeModel()->getTypes()->getTypes();
    Q_ASSERT(!listOfTypes.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboListTypes->addItem(p.second,p.first);
    }
    ui->comboListTypes->setCurrentIndex(0);
}

void FormReports::on_saveButton_clicked()
{
    //TODO for V2 : Order tables and graphics
    QItemSelectionModel *sel=ui->treeView->selectionModel();
    QModelIndexList selIndexes=sel->selectedIndexes();

    if(selIndexes.isEmpty())
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez au moins un noeud dans l'arborescence"));
        return;
    }

    QList<unsigned int> selectedNodes;
    foreach(const QModelIndex &idx,selIndexes)
    {
        selectedNodes.append(idx.data(Qt::UserRole+1).toUInt());
    }

    qDebug()<<"Selected nodes : "<<selectedNodes;
    QList<unsigned int>sortedSelectedNodes;
    if(ui->radioButtonBFS->isChecked())
        sortedSelectedNodes=model->getAttachedTreeModel()->sortNodesBFS(selectedNodes);
    else if(ui->radioButtonDFS->isChecked())
        sortedSelectedNodes=model->getAttachedTreeModel()->sortNodesDFS(selectedNodes);
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Choisissez l'ordre d'affichage des noeuds sélectionnés dans le rapport !"));
        return;
    }
    qDebug()<<"Selected nodes (sorted) : "<<sortedSelectedNodes;


    QList<QListWidgetItem *>selectedItemsTables=ui->listTables->selectedItems();
    QList<QListWidgetItem *>selectedItemsGraphics=ui->listGraphics->selectedItems();

    QList<PCx_Tables::TABLES> selectedTables;
    QList<PCx_Graphics::GRAPHICS> selectedGraphics;

    foreach(QListWidgetItem *item, selectedItemsTables)
    {
        qDebug()<<"Selecting table "<<item->data(Qt::UserRole+1).toUInt();
        selectedTables.append((PCx_Tables::TABLES)item->data(Qt::UserRole+1).toUInt());
    }

    foreach(QListWidgetItem *item, selectedItemsGraphics)
    {
        qDebug()<<"Selecting graphic "<<item->data(Qt::UserRole+1).toUInt();
        selectedGraphics.append((PCx_Graphics::GRAPHICS)item->data(Qt::UserRole+1).toUInt());
    }

    if(selectedTables.isEmpty() && selectedGraphics.isEmpty())
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez au moins un tableau ou un graphique"));
        return;
    }

    QList<PCx_Tables::TABLES> modeIndependantTables;
    QList<PCx_Graphics::GRAPHICS> modeIndependantGraphics;

    //Isolate mode-independant tables and graphics
    if(selectedTables.contains(PCx_Tables::T10))
    {
        modeIndependantTables.append(PCx_Tables::T10);
        selectedTables.removeAll(PCx_Tables::T10);
    }
    if(selectedTables.contains(PCx_Tables::T11))
    {
        modeIndependantTables.append(PCx_Tables::T11);
        selectedTables.removeAll(PCx_Tables::T11);
    }
    if(selectedTables.contains(PCx_Tables::T12))
    {
        modeIndependantTables.append(PCx_Tables::T12);
        selectedTables.removeAll(PCx_Tables::T12);
    }
    if(selectedGraphics.contains(PCx_Graphics::G9))
    {
        modeIndependantGraphics.append(PCx_Graphics::G9);
        selectedGraphics.removeAll(PCx_Graphics::G9);
    }


    /*qDebug()<<"Mode-independant selected tables = "<<modeIndependantTables;
    qDebug()<<"Mode-independant selected graphics = "<<modeIndependantGraphics;

    qDebug()<<"Mode-dependant selected tables = "<<selectedTables;
    qDebug()<<"Mode-dependant selected graphics = "<<selectedGraphics;*/

    QString output=report->generateHTMLHeader();
    QList<PCx_AuditModel::DFRFDIRI> listModes;
    if(ui->checkBoxDF->isChecked())
        listModes.append(PCx_AuditModel::DF);
    if(ui->checkBoxRF->isChecked())
        listModes.append(PCx_AuditModel::RF);
    if(ui->checkBoxDI->isChecked())
        listModes.append(PCx_AuditModel::DI);
    if(ui->checkBoxRI->isChecked())
        listModes.append(PCx_AuditModel::RI);

    if(listModes.isEmpty() && (!selectedGraphics.isEmpty() || !selectedTables.isEmpty()))
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez au moins un mode (dépenses/recettes de fonctionnement/d'investissement)"));
        return;
    }

    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le rapport en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible"));
        return;
    }
    //Will reopen later
    file.close();
    file.remove();

    QString relativeImagePath=fi.fileName()+"_files";
    QString absoluteImagePath=fi.absoluteFilePath()+"_files";

    QFileInfo imageDirInfo(absoluteImagePath);

    if(!imageDirInfo.exists())
    {
        if(!fi.absoluteDir().mkdir(relativeImagePath))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Création du dossier des images impossible"));
            return;
        }
    }
    else
    {
        if(!imageDirInfo.isWritable())
        {
            QMessageBox::critical(this,tr("Attention"),tr("Écriture impossible dans le dossier des images"));
            return;
        }
    }

    int maximumProgressValue=selIndexes.count()+1;

    QProgressDialog progress(tr("Enregistrement du rapport en cours..."),tr("Annuler"),0,maximumProgressValue,this);
    progress.setMinimumDuration(1000);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setValue(0);
    unsigned int i=0;
    QSettings settings;

    report->getGraphics().setGraphicsWidth(settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt());
    report->getGraphics().setGraphicsHeight(settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt());
    report->getGraphics().setScale(settings.value("graphics/scale",PCx_Graphics::DEFAULTSCALE).toDouble());

    QElapsedTimer timer;
    timer.start();

    //FIXME : better cleanup and progress, count maximum value with selected graphics

    foreach (unsigned int selectedNode,sortedSelectedNodes)
    {
        output.append(QString("<h2>%1</h2>").arg(model->getAttachedTreeModel()->getNodeName(selectedNode).toHtmlEscaped()));

        if(!modeIndependantGraphics.isEmpty() || !modeIndependantTables.isEmpty())
        {
            //Mode-independant
            output.append(report->generateHTMLReportForNode(QList<PCx_Tables::TABS>(),modeIndependantTables,modeIndependantGraphics,selectedNode,PCx_AuditModel::DF,NULL,absoluteImagePath,relativeImagePath,NULL));
        }
        foreach(PCx_AuditModel::DFRFDIRI mode,listModes)
        {
            output.append(report->generateHTMLReportForNode(QList<PCx_Tables::TABS>(),selectedTables,selectedGraphics,selectedNode,mode,NULL,absoluteImagePath,relativeImagePath,NULL));
            if(progress.wasCanceled())
                goto cleanup;
            output.append("<br><br><br>");
        }
        if(!progress.wasCanceled())
            progress.setValue(++i);
        else
        {
            cleanup:
            QDir dir(absoluteImagePath);
            dir.removeRecursively();
            return;
        }
        output.append("<br><br><br><br>");
    }
    output.append("</body></html>");

    QString settingStyle=settings.value("output/style","CSS").toString();
    if(settingStyle=="INLINE")
    {
        //Pass HTML through a temp QTextDocument to reinject css into tags (more compatible with text editors)
        QTextDocument formattedOut;
        formattedOut.setHtml(output);
        output=formattedOut.toHtml("utf-8");

        //Cleanup the output a bit
        output.replace(" -qt-block-indent:0;","");
    }

    if(!progress.wasCanceled())
        progress.setValue(maximumProgressValue-1);
    else
    {
        QDir dir(absoluteImagePath);
        dir.removeRecursively();
        return;
    }

    qDebug()<<"Report generated in "<<timer.elapsed()<<"ms";

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible"));
        QDir dir(absoluteImagePath);
        dir.removeRecursively();
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<output;
    stream.flush();
    file.close();

    progress.setValue(maximumProgressValue);
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le rapport <b>%1</b> a bien été enregistré. Les images sont stockées dans le dossier <b>%2</b>").arg(fi.fileName()).arg(relativeImagePath));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le rapport n'a pas pu être enregistré !"));

}

//WARNING : Preselections are done with fixed indexing, refers to populateLists
void FormReports::on_pushButtonPoidsRelatifs_clicked()
{
    //T1,T4,T8
    ui->listTables->item(0)->setSelected(true);
    ui->listTables->item(5)->setSelected(true);
    ui->listTables->item(9)->setSelected(true);
}

void FormReports::on_pushButtonBase100_clicked()
{
    //T5,T6
    ui->listTables->item(6)->setSelected(true);
    ui->listTables->item(7)->setSelected(true);
}

void FormReports::on_pushButtonEvolution_clicked()
{
    //T2bis,T3bis
    ui->listTables->item(2)->setSelected(true);
    ui->listTables->item(4)->setSelected(true);

    //G1,G3,G5,G7
    ui->listGraphics->item(0)->setSelected(true);
    ui->listGraphics->item(2)->setSelected(true);
    ui->listGraphics->item(4)->setSelected(true);
    ui->listGraphics->item(6)->setSelected(true);
}

void FormReports::on_pushButtonEvolutionCumul_clicked()
{
    //T2,T3
    ui->listTables->item(1)->setSelected(true);
    ui->listTables->item(3)->setSelected(true);

    //G2,G4,G6,G8
    ui->listGraphics->item(1)->setSelected(true);
    ui->listGraphics->item(3)->setSelected(true);
    ui->listGraphics->item(5)->setSelected(true);
    ui->listGraphics->item(7)->setSelected(true);
}

void FormReports::on_pushButtonJoursAct_clicked()
{
    //T7,T9
    ui->listTables->item(8)->setSelected(true);
    ui->listTables->item(10)->setSelected(true);
}

void FormReports::on_pushButtonResultats_clicked()
{
    //T10,T11,T12
    ui->listTables->item(11)->setSelected(true);
    ui->listTables->item(12)->setSelected(true);
    ui->listTables->item(13)->setSelected(true);

    //G9
    ui->listGraphics->item(8)->setSelected(true);

}

void FormReports::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormReports::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
}

void FormReports::on_pushButtonSelectAll_clicked()
{
    ui->listTables->selectAll();
    ui->listGraphics->selectAll();
}

void FormReports::on_pushButtonSelectNone_clicked()
{
    ui->listTables->clearSelection();
    ui->listGraphics->clearSelection();
}

void FormReports::on_pushButtonSelectType_clicked()
{
    PCx_TreeModel *treeModel=model->getAttachedTreeModel();
    unsigned int selectedType=ui->comboListTypes->currentData().toUInt();
    QModelIndexList indexOfTypes=treeModel->getIndexesOfNodesWithThisType(selectedType);

    foreach(const QModelIndex &index,indexOfTypes)
    {
        ui->treeView->selectionModel()->select(index,QItemSelectionModel::Select);
    }
}

void FormReports::on_pushButtonSelectAllNodes_clicked()
{
    ui->treeView->expandAll();
    ui->treeView->selectAll();
}

void FormReports::on_pushButtonUnSelectAllNodes_clicked()
{
    ui->treeView->clearSelection();
}
