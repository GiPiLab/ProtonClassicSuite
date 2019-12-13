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

#ifndef PCX_PREVISION_H
#define PCX_PREVISION_H

#include "pcx_auditwithtreemodel.h"
#include "pcx_previsionitemcriteria.h"
#include <QAbstractListModel>
#include <QDateTime>

class PCx_Prevision {
public:
  PCx_Prevision(unsigned int previsionId);
  virtual ~PCx_Prevision();

  unsigned int getPrevisionId() const { return previsionId; }
  QString getPrevisionName() const { return previsionName; }
  unsigned int getAttachedAuditId() const { return attachedAuditId; }
  unsigned int getAttachedTreeId() const { return attachedTreeId; }
  QDateTime getCreationTimeLocal() const { return creationTimeLocal; }
  QDateTime getCreationTimeUTC() const { return creationTimeUTC; }
  PCx_Audit *getAttachedAudit() const { return attachedAudit; }
  PCx_Tree *getAttachedTree() const { return attachedTree; }

  int toPrevisionalExtendedAudit(const QString &newAuditName);

  bool isPrevisionEmpty() const;
  unsigned int duplicatePrevision(const QString &newName) const;

  static unsigned int addNewPrevision(unsigned int auditId, const QString &name);
  static bool previsionNameExists(const QString &previsionName);
  static QList<QPair<unsigned int, QString>> getListOfPrevisions();
  static bool deletePrevision(unsigned int previsionId);

  QString generateHTMLPrevisionTitle() const;

private:
  unsigned int previsionId;
  unsigned int attachedAuditId;
  unsigned int attachedTreeId;
  QString previsionName;
  QDateTime creationTimeLocal;
  QDateTime creationTimeUTC;

  PCx_Audit *attachedAudit;
  PCx_Tree *attachedTree;

  PCx_Prevision(const PCx_Prevision &c);
  PCx_Prevision &operator=(const PCx_Prevision &);
};

#endif // PCX_PREVISION_H
