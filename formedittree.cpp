/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le
 * Développement Local (2006--)
 *
 * labo@gipilab.org
 *
 * Ce logiciel est un programme informatique servant à cerner l'ensemble des
 * données budgétaires de la collectivité territoriale (audit, reporting
 * infra-annuel, prévision des dépenses à venir)
 *
 *
 * Ce logiciel est régi par la licence CeCILL soumise au droit français et
 * respectant les principes de diffusion des logiciels libres. Vous pouvez
 * utiliser, modifier et/ou redistribuer ce programme sous les conditions
 * de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA
 * sur le site "http://www.cecill.info".
 *
 * En contrepartie de l'accessibilité au code source et des droits de copie,
 * de modification et de redistribution accordés par cette licence, il n'est
 * offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
 * seule une responsabilité restreinte pèse sur l'auteur du programme, le
 * titulaire des droits patrimoniaux et les concédants successifs.
 *
 * A cet égard l'attention de l'utilisateur est attirée sur les risques
 * associés au chargement, à l'utilisation, à la modification et/ou au
 * développement et à la reproduction du logiciel par l'utilisateur étant
 * donné sa spécificité de logiciel libre, qui peut le rendre complexe à
 * manipuler et qui le réserve donc à des développeurs et des professionnels
 * avertis possédant des connaissances informatiques approfondies. Les
 * utilisateurs sont donc invités à charger et tester l'adéquation du
 * logiciel à leurs besoins dans des conditions permettant d'assurer la
 * sécurité de leurs systèmes et ou de leurs données et, plus généralement,
 * à l'utiliser et l'exploiter dans les mêmes conditions de sécurité.
 *
 * Le fait que vous puissiez accéder à cet en-tête signifie que vous avez
 * pris connaissance de la licence CeCILL, et que vous en avez accepté les
 * termes.
 *
 */

#include "formedittree.h"
#include "formdisplaytree.h"
#include "formtreeconsistency.h"
#include "pcx_tree.h"
#include "ui_formedittree.h"
#include "utils.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>
#include <QSqlField>
#include <QSqlRecord>
#include <QStandardPaths>

FormEditTree::FormEditTree(QWidget *parent) : QWidget(parent), ui(new Ui::FormEditTree) {
  ui->setupUi(this);
  model = nullptr;
  updateListOfTree();
  updateRandomButtonVisibility();
}

FormEditTree::~FormEditTree() {
  delete model;
  delete ui;
}

void FormEditTree::updateRandomButtonVisibility() {
  QSettings settings;
  bool randomAllowed = settings.value("misc/randomAllowed", true).toBool();
  ui->randomTreeButton->setEnabled(randomAllowed);
}

void FormEditTree::on_addTypeButton_clicked() {
  if (model != nullptr) {
    bool ok;
    QString text;

  redo:
    do {
      text = QInputDialog::getText(this, tr("Nouveau type"), tr("Nom du type à ajouter : "), QLineEdit::Normal, "", &ok)
                 .simplified();

    } while (ok && text.isEmpty());

    if (ok) {
      if (model->getTypeNames().contains(text)) {
        QMessageBox::warning(this, QObject::tr("Attention"),
                             QObject::tr("Le type <b>%1</b> existe déjà !").arg(text.toHtmlEscaped()));
        goto redo;
      }
      if (text.size() > MAXOBJECTNAMELENGTH) {
        QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
        goto redo;
      }

      model->addType(text);
    }
  }
}

void FormEditTree::updateListOfTree() {
  ui->comboBox->clear();

  QList<QPair<unsigned int, QString>> lot = PCx_Tree::getListOfTrees();
  bool nonEmpty = !lot.isEmpty();

  if (lot.isEmpty()) {
    QMessageBox::information(this, tr("Pour continuer"),
                             tr("Vous pouvez ajouter un arbre, soit en le créant à la main, soit en "
                                "l'important à partir d'un fichier excel, soit en générer un "
                                "aléatoire à des fins de test. Le format de données attendu dans le "
                                "fichier excel est détaillé dans l'aide contextuelle du bouton "
                                "<b>importer arbre</b>"));
  }

  setReadOnly(!nonEmpty);
  ui->deleteTreeButton->setEnabled(nonEmpty);
  ui->duplicateTreeButton->setEnabled(nonEmpty);
  ui->viewTreeButton->setEnabled(nonEmpty);
  ui->pushButtonExpandAll->setEnabled(nonEmpty);
  ui->pushButtonCollapseAll->setEnabled(nonEmpty);
  ui->consistencyButton->setEnabled(nonEmpty);
  QPair<unsigned int, QString> p;
  foreach (p, lot) { ui->comboBox->insertItem(0, p.second, p.first); }

  if (ui->comboBox->count() > 0) {
    ui->comboBox->setCurrentIndex(0);
    on_comboBox_activated(0);
  } else {
    ui->deleteNodeButton->setDisabled(true);
    ui->addNodeButton->setDisabled(true);
    ui->addTypeButton->setDisabled(true);
    ui->deleteTypeButton->setDisabled(true);
    ui->finishTreeButton->setDisabled(true);
    ui->modifyNodeButton->setDisabled(true);
    ui->guessHierarchyButton->setDisabled(true);
    ui->treeView->setDragEnabled(false);
    ui->listTypesView->setDisabled(true);
  }
}

void FormEditTree::setReadOnly(bool state) {
  ui->deleteNodeButton->setDisabled(state);
  ui->addNodeButton->setDisabled(state);
  ui->addTypeButton->setDisabled(false);
  ui->deleteTypeButton->setDisabled(false);
  ui->finishTreeButton->setDisabled(state);
  ui->modifyNodeButton->setDisabled(false);
  ui->guessHierarchyButton->setDisabled(state);
  ui->treeView->setDragEnabled(!state);
  ui->listTypesView->setDisabled(false);
}

void FormEditTree::on_deleteTreeButton_clicked() {
  if (model != nullptr) {
    if (question(tr("Vous allez supprimer cet arbre. Cette action ne peut pas "
                    "être annulée. En êtes-vous sûr ?")) == QMessageBox::No) {
      return;
    }

    unsigned int treeId = model->getTreeId();
    // qDebug()<<"Deleting tree "<<treeId;

    int result = PCx_Tree::deleteTree(treeId);

    if (result == 1) {
      ui->listTypesView->setModel(nullptr);
      model->getTypesTableModel()->clear();
      model->clear();
      delete model;
      model = nullptr;

      updateListOfTree();

      emit listOfTreeChanged();
    } else if (result == 0) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe des audits ou des reportings liés à "
                                                     "cet arbre. Supprimez-les d'abord"));
      return;
    }
  }
}

void FormEditTree::onTypesChanged() {
  if (model->updateTree() == false) {
    die();
  }
  ui->treeView->expandToDepth(1);
}

void FormEditTree::on_newTreeButton_clicked() {
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this->parentWidget(), tr("Nouvel arbre"), tr("Nom du nouvel arbre : "),
                                 QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    if (PCx_Tree::treeNameExists(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un arbre portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }
    if (PCx_Tree::addTree(text) != -1) {
      emit listOfTreeChanged();
      updateListOfTree();
    }
  }
}

void FormEditTree::on_deleteTypeButton_clicked() {
  if (model != nullptr) {
    QModelIndex index = ui->listTypesView->currentIndex();
    if (index.row() > -1) {
      if (question(tr("Voulez-vous supprimer ce type ?")) == QMessageBox::No) {
        return;
      }

      if (ui->listTypesView->model()->rowCount() == 1) {
        QMessageBox::warning(this, tr("Attention"), tr("Les noeuds doivent tous être typés"));
        return;
      }

      model->deleteType(model->getTypesTableModel()->record(index.row()).field("id").value().toUInt());
    }
  }
}

void FormEditTree::on_addNodeButton_clicked() {
  if (model == nullptr) {
    return;
  }
  QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

  if (!selection.isEmpty()) {
    if (selection.count() > 1) {
      QMessageBox::information(this, tr("Information"), tr("Pas de sélection multiple pour ajouter un noeud !"));
      return;
    }

    if (model->getNumberOfNodes() >= PCx_Tree::MAXNODES) {
      QMessageBox::warning(this, QObject::tr("Attention"),
                           QObject::tr("Nombre maximum de noeuds dans l'arbre atteint !"));
      return;
    }

    QModelIndex indexType = ui->listTypesView->currentIndex();
    if (indexType.row() < 0) {
      QMessageBox::information(this, tr("Information"), tr("Sélectionnez le type de l'élément à ajouter "
                                                           "dans la zone de droite"));
      return;
    }

    unsigned int selectedTypeId = model->getTypesTableModel()->record(indexType.row()).field("id").value().toUInt();

    // qDebug()<<"Selected type ID : "<<selectedTypeId;

    bool ok;
    QString text;

  redo:
    do {
      text = QInputDialog::getText(
                 this, tr("Nouveau noeud"),
                 tr("Donnez un nom au nouveau noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),
                 QLineEdit::Normal, "", &ok)
                 .simplified();

    } while (ok && text.isEmpty());

    if (ok) {      
      if (text.size() > PCx_Tree::MAXNODENAMELENGTH) {
        QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
        goto redo;
      }
      model->addNode(selectedTypeId, text, selection[0]);
    }

  } else {
    QMessageBox::information(this, tr("Information"), tr("Sélectionnez d'abord le noeud père dans l'arbre"));
    return;
  }
}

void FormEditTree::on_modifyNodeButton_clicked() {
  if (model == nullptr) {
    return;
  }

  QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

  if (!selection.isEmpty()) {
    if (selection.count() > 1) {
      QMessageBox::information(this, tr("Information"), tr("Pas de sélection multiple pour modifier un noeud !"));
      return;
    }
    unsigned int selectedId = selection[0].data(PCx_TreeModel::NodeIdUserRole).toUInt();
    unsigned int oldTypeId = model->getTypeId(selectedId);
    QPair<QString, QString> typeAndNodeName = model->getTypeNameAndNodeName(selectedId);
    // qDebug()<<"Selected Node ID : "<<selectedId;

    // No modification for the root
    if (selectedId == 1) {
      bool ok;
      QString text;

    redoRoot:
      do {
        text = QInputDialog::getText(this, tr("Modifier noeud"), tr("Nouveau nom de la racine : "), QLineEdit::Normal,
                                     typeAndNodeName.second, &ok)
                   .simplified();

      } while (ok && text.isEmpty());

      if (ok) {
        if (text != typeAndNodeName.second) {          
          if (text.size() > MAXOBJECTNAMELENGTH) {
            QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
            goto redoRoot;
          }
          model->updateNode(selection[0], text, 0);
        }
      }
      // QMessageBox::warning(this,tr("Attention"),tr("Vous ne pouvez pas
      // modifier la racine de l'arbre"));
      return;
    }

    QModelIndex indexType = ui->listTypesView->currentIndex();
    if (indexType.row() < 0) {
      QMessageBox::information(this, tr("Information"), tr("Sélectionnez le nouveau type du noeud à "
                                                           "modifier dans la zone de droite"));
      return;
    }

    unsigned int selectedTypeId = model->getTypesTableModel()->record(indexType.row()).field("id").value().toUInt();

    bool ok;
    QString text;

  redo:
    do {
      text =
          QInputDialog::getText(this, tr("Modifier noeud"),
                                tr("Nouveau nom du noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),
                                QLineEdit::Normal, typeAndNodeName.second, &ok)
              .simplified();

    } while (ok && text.isEmpty());

    if (ok) {
      if (selectedTypeId != oldTypeId || text != typeAndNodeName.second) {

        if (text.size() > MAXOBJECTNAMELENGTH) {
          QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
          goto redo;
        }
        model->updateNode(selection[0], text, selectedTypeId);
      }
    }
  } else {
    QMessageBox::information(this, tr("Information"), tr("Sélectionnez le noeud à modifier"));
    return;
  }
}

void FormEditTree::on_deleteNodeButton_clicked() {
  if (model == nullptr) {
    return;
  }

  QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

  if (!selection.isEmpty()) {
    if (selection.count() > 1) {
      QMessageBox::information(this, tr("Information"), tr("Pas de sélection multiple pour supprimer un noeud !"));
      return;
    }
    // Assume only single selection
    unsigned int selectedId = selection[0].data(PCx_TreeModel::NodeIdUserRole).toUInt();
    // qDebug()<<"Selected Node ID : "<<selectedId;

    // No modification for the root
    if (selectedId == 1) {
      QMessageBox::warning(this, tr("Attention"), tr("Vous ne pouvez pas supprimer la racine de l'arbre"));
      return;
    }

    if (question(tr("Voulez-vous vraiment supprimer le noeud <b>%1</b> ainsi "
                    "que tous ses descendants ?")
                     .arg(selection[0].data().toString().toHtmlEscaped())) == QMessageBox::No) {
      return;
    }
    model->deleteNode(selection[0]);
  } else {
    QMessageBox::information(this, tr("Information"), tr("Sélectionner le noeud à supprimer. Tous ses "
                                                         "descendants seront également supprimés"));
  }
}

void FormEditTree::on_finishTreeButton_clicked() {
  if (model != nullptr) {
    if (question(tr("Voulez-vous vraiment terminer cet arbre ? Une fois "
                    "terminé, vous ne pourrez plus modifier sa structure, mais "
                    "vous pourrez toujours changer le nom des noeuds.")) == QMessageBox::No) {
      return;
    }
    if (model->finishTree() == false) {
      die();
    }
    emit listOfTreeChanged();
    updateListOfTree();
    QMessageBox::information(this, tr("Information"),
                             tr("Arbre terminé. Vous pouvez maintenant l'utiliser pour créer des "
                                "audits ou des reportings"));
  }
}

void FormEditTree::on_viewTreeButton_clicked() {
  auto *ddt = new FormDisplayTree(model->getTreeId(), this);
  ddt->setAttribute(Qt::WA_DeleteOnClose);
  auto *mdiSubWin = qobject_cast<QMdiSubWindow *>(this->parentWidget());
  QMdiArea *mdiArea = mdiSubWin->mdiArea();
  QMdiSubWindow *subWin = mdiArea->addSubWindow(ddt);
  subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
  ddt->show();
}

void FormEditTree::on_comboBox_activated(int index) {
  if (index == -1) {
    return;
  }
  if (model != nullptr) {
    QItemSelectionModel *m = ui->listTypesView->selectionModel();
    ui->listTypesView->setModel(nullptr);
    delete m;
    delete model;
  }

  // Read-write types
  model = new PCx_TreeModel(ui->comboBox->currentData().toUInt());

  QItemSelectionModel *m = ui->treeView->selectionModel();
  ui->treeView->setModel(model);
  delete m;
  ui->treeView->expandToDepth(1);

  m = ui->listTypesView->selectionModel();
  if (model->getTypesTableModel() == nullptr) {
    qFatal("Assertion failed");
  }
  ui->listTypesView->setModel(model->getTypesTableModel());
  ui->listTypesView->setModelColumn(1);
  delete m;

  connect(model, &PCx_TreeModel::typesUpdated, this, &FormEditTree::onTypesChanged);
  setReadOnly(model->isFinished());
}

void FormEditTree::on_duplicateTreeButton_clicked() {
  if (model != nullptr) {
    bool ok;
    QString text;

  redo:
    do {
      text = QInputDialog::getText(this, tr("Dupliquer arbre"), tr("Nom de l'arbre dupliqué : "), QLineEdit::Normal, "",
                                   &ok)
                 .simplified();

    } while (ok && text.isEmpty());

    if (ok) {
      if (PCx_Tree::treeNameExists(text)) {
        QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un arbre portant ce nom !"));
        goto redo;
      }
      if (text.size() > MAXOBJECTNAMELENGTH) {
        QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
        goto redo;
      }
      model->duplicateTree(text);
      updateListOfTree();
    }
  }
}

void FormEditTree::on_randomTreeButton_clicked() {
  bool ok;
  QString text;
  unsigned int nbNodes;

  nbNodes = static_cast<unsigned int>(QInputDialog::getInt(this, tr("Nouvel arbre aléatoire"), tr("Nombre de noeuds"),
                                                           20, 2, PCx_Tree::MAXNODES, 1, &ok));

  if (ok) {
  redo:
    do {
      text = QInputDialog::getText(this, tr("Nouvel arbre aléatoire"), tr("Nom de l'arbre aléatoire : "),
                                   QLineEdit::Normal, "", &ok)
                 .simplified();

    } while (ok && text.isEmpty());

    if (ok) {
      if (model->treeNameExists(text)) {
        QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un arbre portant ce nom !"));
        goto redo;
      }
      if (text.size() > MAXOBJECTNAMELENGTH) {
        QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
        goto redo;
      }

      PCx_Tree::createRandomTree(text, nbNodes);
      updateListOfTree();
    }
  }
}

void FormEditTree::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormEditTree::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormEditTree::on_importTreeButton_clicked() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName = fileDialog.getOpenFileName(this, tr("Ouvrir un arbre"), "", tr("Arbres au format XLSX (*.xlsx)"));

  if (fileName.isEmpty()) {
    return;
  }

  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Importer arbre"), tr("Donnez un nom à cet arbre : "), QLineEdit::Normal, "",
                                 &ok)
               .simplified();
  } while (ok && text.isEmpty());

  if (!ok) {
    return;
  }

  if (PCx_Tree::treeNameExists(text)) {
    QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un arbre portant ce nom !"));
    goto redo;
  }
  if (text.size() > MAXOBJECTNAMELENGTH) {
    QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
    goto redo;
  }

  if (PCx_Tree::importTreeFromXLSX(fileName, text) > 0) {
    updateListOfTree();
    emit listOfTreeChanged();
  }
}

void FormEditTree::on_consistencyButton_clicked() {
  auto *form = new FormTreeConsistency(model->getTreeId(), this);
  form->setAttribute(Qt::WA_DeleteOnClose);
  auto *mdiSubWin = qobject_cast<QMdiSubWindow *>(this->parentWidget());
  QMdiArea *mdiArea = mdiSubWin->mdiArea();
  QMdiSubWindow *subWin = mdiArea->addSubWindow(form);
  subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
  form->show();
}

QSize FormEditTree::sizeHint() const { return {700, 550}; }

void FormEditTree::on_guessHierarchyButton_clicked() {
  if (question(tr("Tenter de deviner la hiérarchie de l'arbre à l'aide des "
                  "chiffres contenus au début du nom des noeuds ? L'arbre sera "
                  "modifié et cette opération ne peut pas être annulée.")) == QMessageBox::Yes) {
    int count = model->guessHierarchy();
    if (count > 0) {
      ui->treeView->expandToDepth(1);
      if (count > 1) {
        QMessageBox::information(this, tr("Information"), tr("%1 noeuds réorganisés").arg(count));
      } else {
        QMessageBox::information(this, tr("Information"), tr("%1 noeud réorganisé").arg(count));
      }
    } else {
      QMessageBox::information(this, tr("Information"), tr("Aucune nouvelle relation trouvée"));
    }
  }
}
