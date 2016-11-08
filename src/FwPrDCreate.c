/**
 * @file
 * @ingroup prGroup
 * Implements the dynamical creation functions for the FW Procedure Module.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2011, All Rights Reserved
 *
 * This file is part of FW Profile.
 *
 * FW Profile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FW Profile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FW Profile.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#include "FwPrDCreate.h"
#include "FwPrPrivate.h"
#include <stdlib.h>

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrCreate(FwPrCounterS1_t nOfANodes, FwPrCounterS1_t nOfDNodes, FwPrCounterS1_t nOfFlows,
                      FwPrCounterS1_t nOfActions, FwPrCounterS1_t nOfGuards) {

  FwPrCounterS1_t i;
  PrBaseDesc_t*   prBase;
  FwPrDesc_t      prDesc;

  if (nOfFlows < 2) {
    return NULL;
  }

  if (nOfANodes < 1) {
    return NULL;
  }

  if (nOfDNodes < 0) {
    return NULL;
  }

  if (nOfActions < 1) {
    return NULL;
  }

  if (nOfActions > nOfANodes) {
    return NULL;
  }

  if (nOfGuards < 0) {
    return NULL;
  }

  if (nOfGuards > nOfFlows) {
    return NULL;
  }

  prDesc = (FwPrDesc_t)malloc(sizeof(struct FwPrDesc));
  if (prDesc == NULL) {
    return NULL;
  }

  prBase = (PrBaseDesc_t*)malloc(sizeof(PrBaseDesc_t));
  if (prBase == NULL) {
    return NULL;
  }

  prBase->aNodes = (PrANode_t*)malloc(((FwPrCounterU4_t)(nOfANodes)) * sizeof(PrANode_t));
  if (prBase->aNodes == NULL) {
    return NULL;
  }
  for (i = 0; i < nOfANodes; i++) {
    prBase->aNodes[i].iFlow = -1;
  }

  if (nOfDNodes > 0) {
    prBase->dNodes = (PrDNode_t*)malloc(((FwPrCounterU4_t)(nOfDNodes)) * sizeof(PrDNode_t));
    if (prBase->dNodes == NULL) {
      return NULL;
    }
    for (i = 0; i < nOfDNodes; i++) {
      prBase->dNodes[i].outFlowIndex = -1;
    }
  }
  else {
    prBase->dNodes = NULL;
  }

  prBase->flows = (PrFlow_t*)malloc(((FwPrCounterU4_t)(nOfFlows)) * sizeof(PrFlow_t));
  if (prBase->flows == NULL) {
    return NULL;
  }
  for (i = 0; i < nOfFlows; i++) {
    prBase->flows[i].iGuard = -1;
  }

  prDesc->prActions = (FwPrAction_t*)malloc(((FwPrCounterU4_t)(nOfActions)) * sizeof(FwPrAction_t));
  if (prDesc->prActions == NULL) {
    return NULL;
  }
  for (i = 0; i < nOfActions; i++) {
    prDesc->prActions[i] = NULL;
  }

  prDesc->prGuards = (FwPrGuard_t*)malloc(((FwPrCounterU4_t)(nOfGuards + 1)) * sizeof(FwPrGuard_t));
  if (prDesc->prGuards == NULL) {
    return NULL;
  }

  for (i = 1; i <= nOfGuards; i++) {
    prDesc->prGuards[i] = NULL;
  }
  prDesc->prGuards[0] = &PrDummyGuard;

  prBase->nOfANodes   = nOfANodes;
  prBase->nOfDNodes   = nOfDNodes;
  prBase->nOfFlows    = nOfFlows;
  prDesc->prBase      = prBase;
  prDesc->curNode     = 0;
  prDesc->prData      = NULL;
  prDesc->flowCnt     = 1;
  prDesc->nOfActions  = nOfActions;
  prDesc->nOfGuards   = (FwPrCounterS1_t)(nOfGuards + 1);
  prDesc->errCode     = prSuccess;
  prDesc->nodeExecCnt = 0;
  prDesc->prExecCnt   = 0;

  return prDesc;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrCreateDer(FwPrDesc_t prDesc) {
  FwPrCounterS1_t i;
  PrBaseDesc_t*   prBase = prDesc->prBase;
  FwPrDesc_t      extPrDesc;

  /* Create descriptor for derived SM */
  extPrDesc = (FwPrDesc_t)malloc(sizeof(struct FwPrDesc));
  if (extPrDesc == NULL) {
    return NULL;
  }

  /* Create array of actions in the derived SM */
  extPrDesc->prActions = (FwPrAction_t*)malloc(((FwPrCounterU4_t)(prDesc->nOfActions)) * sizeof(FwPrAction_t));
  if (extPrDesc->prActions == NULL) {
    return NULL;
  }
  for (i = 0; i < prDesc->nOfActions; i++) {
    extPrDesc->prActions[i] = prDesc->prActions[i];
  }

  /* Create array of guards in the derived SM (NB: number of guards is guaranteed to be greater than 0 */
  extPrDesc->prGuards = (FwPrGuard_t*)malloc(((FwPrCounterU4_t)(prDesc->nOfGuards)) * sizeof(FwPrGuard_t));
  if (extPrDesc->prGuards == NULL) {
    return NULL;
  }
  for (i = 0; i < prDesc->nOfGuards; i++) {
    extPrDesc->prGuards[i] = prDesc->prGuards[i];
  }

  extPrDesc->prBase      = prBase;
  extPrDesc->curNode     = 0;
  extPrDesc->prData      = NULL;
  extPrDesc->flowCnt     = 0;
  extPrDesc->nOfActions  = prDesc->nOfActions;
  extPrDesc->nOfGuards   = prDesc->nOfGuards;
  extPrDesc->errCode     = prDesc->errCode;
  extPrDesc->nodeExecCnt = 0;
  extPrDesc->prExecCnt   = 0;

  return extPrDesc;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrRelease(FwPrDesc_t prDesc) {
  PrBaseDesc_t* prBase;

  /* Release memory allocated to base descriptor (base descriptor is always allocated and
   * hence it is not necessary to check whether it is non-NULL) */
  prBase = prDesc->prBase;

  /* Release memory allocated to action node array (this is guaranteed to have at least one entry
   * since procedure are constrained to have at least one action node) */
  free(prBase->aNodes);

  /* Release memory allocated to decision node array (this may be empty) */
  if (prBase->dNodes != NULL) {
    free(prBase->dNodes);
  }

  /* Release pointer to transition array (note that the transition array is guaranteed to exist and to
   * have at least one element, see operation FwPrCreate) */
  free(prBase->flows);

  /* Release memory allocated to base descriptor */
  free(prDesc->prBase);

  /* Release memory allocated to extension part of the state machine descriptor */
  FwPrReleaseDer(prDesc);

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrReleaseDer(FwPrDesc_t prDesc) {

  /* Release pointer to the action and guard arrays (note that both arrays are guaranteed to
   * have non-zero length) */
  free(prDesc->prActions);
  free(prDesc->prGuards);

  /* Release pointer to state machine descriptor */
  free(prDesc);
  prDesc = NULL;

  return;
}
