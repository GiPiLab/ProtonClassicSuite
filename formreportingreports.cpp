#include "formreportingreports.h"
#include "ui_formreportingreports.h"


FormReportingReports::FormReportingReports(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingReports)
{
    ui->setupUi(this);
    model=nullptr;
    plot=new QCustomPlot();
    updateListOfReportings();
}

FormReportingReports::~FormReportingReports()
{
    delete ui;
    if(model!=nullptr)
    {
        delete model;
        delete report;
    }
    delete plot;
}

void FormReportingReports::onListOfReportingsChanged()
{
    updateListOfReportings();
}


void FormReportingReports::updateListOfReportings()
{
    ui->comboListReportings->clear();

    QList<QPair<unsigned int,QString> >listOfReportings=PCx_Reporting::getListOfReportings();
    if(listOfReportings.isEmpty())
    {
        QMessageBox::information(this,tr("Information"),tr("Commencez par créer et ajouter des données à un reporting dans la fenêtre de gestion des reportings"));
    }
    bool nonEmpty=!listOfReportings.isEmpty();
    this->setEnabled(nonEmpty);
    QPair<unsigned int, QString> p;
    foreach(p,listOfReportings)
    {
        ui->comboListReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListReportings->setCurrentIndex(0);
    on_comboListReportings_activated(0);
}

void FormReportingReports::updateTooltips()
{
    QList<unsigned int> leaves=model->getAttachedTree()->getLeavesId();
    unsigned int nodeId=leaves.at(qrand()%leaves.count());
    ui->checkBoxS->setToolTip("<span style='font-size:8pt'>"+report->getTables().getPCRRFDF(nodeId)+"</span>");
    ui->checkBoxA->setToolTip("<span style='font-size:8pt'>"+report->getTables().getPCRProvenance(nodeId,MODES::DFRFDIRI::DF)+"</span>");
    ui->checkBoxB->setToolTip("<span style='font-size:8pt'>"+report->getTables().getPCRVariation(nodeId,MODES::DFRFDIRI::DF)+"</span>");
    ui->checkBoxC->setToolTip("<span style='font-size:8pt'>"+report->getTables().getPCRUtilisation(nodeId,MODES::DFRFDIRI::DF)+"</span>");
    ui->checkBoxD->setToolTip("<span style='font-size:8pt'>"+report->getTables().getPCRCycles(nodeId,MODES::DFRFDIRI::DF)+"</span>");
}

void FormReportingReports::on_comboListReportings_activated(int index)
{
    if(index==-1||ui->comboListReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListReportings->currentData().toUInt();

    if(model!=nullptr)
    {
        delete model;
        delete report;
    }
    model=new PCx_ReportingWithTreeModel(selectedReportingId);
    report=new PCx_Report(model);
    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(model->getAttachedTree());
    delete m;
    ui->treeView->expandToDepth(1);

    ui->comboListTypes->clear();

    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTree()->getAllTypes();
    if(listOfTypes.isEmpty())
    {
        qFatal("Assertion failed");
    }
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboListTypes->addItem(p.second,p.first);
    }
    ui->comboListTypes->setCurrentIndex(0);
    updateTooltips();
}

void FormReportingReports::on_saveButton_clicked()
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
        selectedNodes.append(idx.data(PCx_TreeModel::NodeIdUserRole).toUInt());
    }

    //qDebug()<<"Selected nodes : "<<selectedNodes;
    QList<unsigned int>sortedSelectedNodes;
    if(ui->radioButtonBFS->isChecked())
        sortedSelectedNodes=model->getAttachedTree()->sortNodesBFS(selectedNodes);
    else if(ui->radioButtonDFS->isChecked())
        sortedSelectedNodes=model->getAttachedTree()->sortNodesDFS(selectedNodes);
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Choisissez l'ordre d'affichage des noeuds sélectionnés dans le rapport !"));
        return;
    }

    QList<PCx_Report::PCRPRESETS> selectedPresets;

    if(ui->checkBoxA->isChecked())
        selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_A);

    if(ui->checkBoxB->isChecked())
        selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_B);

    if(ui->checkBoxC->isChecked())
        selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_C);

    if(ui->checkBoxD->isChecked())
        selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_D);

    if(ui->checkBoxS->isChecked())
        selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_S);



    if(selectedPresets.isEmpty())
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez au moins un élément à inclure"));
        return;
    }

    QString output=model->generateHTMLHeader();
    output.append(model->generateHTMLReportingTitle());
    QList<MODES::DFRFDIRI> listModes;
    if(ui->checkBoxDF->isChecked())
        listModes.append(MODES::DFRFDIRI::DF);
    if(ui->checkBoxRF->isChecked())
        listModes.append(MODES::DFRFDIRI::RF);
    if(ui->checkBoxDI->isChecked())
        listModes.append(MODES::DFRFDIRI::DI);
    if(ui->checkBoxRI->isChecked())
        listModes.append(MODES::DFRFDIRI::RI);

    if(listModes.isEmpty() && !selectedPresets.isEmpty())
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
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
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

   // QElapsedTimer timer;
   // timer.start();

    output.append(report->generateHTMLTOC(sortedSelectedNodes));

    QList<PCx_Report::PCRPRESETS> presetsSansS=selectedPresets;
    presetsSansS.removeAll(PCx_Report::PCRPRESETS::PCRPRESET_S);

    foreach (unsigned int selectedNode,sortedSelectedNodes)
    {
        output.append(QString("<h1 id='node%2'>%1</h1>").arg(model->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped()).arg(selectedNode));

        if(selectedPresets.contains(PCx_Report::PCRPRESETS::PCRPRESET_S))
        {
            QList<PCx_Report::PCRPRESETS> tmpPreset={PCx_Report::PCRPRESETS::PCRPRESET_S};
            output.append(report->generateHTMLReportingReportForNode(tmpPreset,selectedNode,MODES::DFRFDIRI::GLOBAL,ui->checkBoxIncludeGraphics->isChecked(),nullptr,absoluteImagePath,relativeImagePath,nullptr));
        }
        foreach(MODES::DFRFDIRI mode,listModes)
        {
            output.append(report->generateHTMLReportingReportForNode(presetsSansS,selectedNode,mode,ui->checkBoxIncludeGraphics->isChecked(),nullptr,absoluteImagePath,relativeImagePath,nullptr));
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

    //qDebug()<<"Report generated in "<<timer.elapsed()<<"ms";

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
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
        QMessageBox::information(this,tr("Information"),tr("Le rapport <b>%1</b> a bien été enregistré. Les images sont stockées dans le dossier <b>%2</b>").arg(fi.fileName().toHtmlEscaped()).arg(relativeImagePath.toHtmlEscaped()));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le rapport n'a pas pu être enregistré !"));

}


void FormReportingReports::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormReportingReports::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
    ui->treeView->expandToDepth(0);
}

void FormReportingReports::on_pushButtonSelectType_clicked()
{
    PCx_TreeModel *treeModel=model->getAttachedTree();
    unsigned int selectedType=ui->comboListTypes->currentData().toUInt();
    QModelIndexList indexOfTypes=treeModel->getIndexesOfNodesWithThisType(selectedType);

    foreach(const QModelIndex &index,indexOfTypes)
    {
        ui->treeView->selectionModel()->select(index,QItemSelectionModel::Select);
    }
}

void FormReportingReports::on_pushButtonSelectAllNodes_clicked()
{
    ui->treeView->expandAll();
    ui->treeView->selectAll();
}

void FormReportingReports::on_pushButtonUnSelectAllNodes_clicked()
{
    ui->treeView->clearSelection();
}

/*QSize FormReportingReports::sizeHint() const
{
    return QSize(900,600);
}*/

void FormReportingReports::on_pushButtonSelectAll_clicked()
{
    ui->checkBoxA->setChecked(true);
    ui->checkBoxB->setChecked(true);
    ui->checkBoxC->setChecked(true);
    ui->checkBoxD->setChecked(true);
    ui->checkBoxS->setChecked(true);
}

void FormReportingReports::on_pushButtonSelectNone_clicked()
{
    ui->checkBoxA->setChecked(false);
    ui->checkBoxB->setChecked(false);
    ui->checkBoxC->setChecked(false);
    ui->checkBoxD->setChecked(false);
    ui->checkBoxS->setChecked(false);
}
