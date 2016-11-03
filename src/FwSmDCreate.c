/**
 * @file
 * @ingroup smGroup
 * Implements the dynamical creation functions for the FW State Machine Module.
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

#include "FwSmDCreate.h"
#include "FwSmPrivate.h"
#include <stdlib.h>

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmDesc_t FwSmCreate(FwSmCounterS1_t nOfStates, FwSmCounterS1_t nOfChoicePseudoStates, FwSmCounterS1_t nOfTrans,
                      FwSmCounterS1_t nOfActions, FwSmCounterS1_t nOfGuards) {

  FwSmCounterS1_t i;
  SmBaseDesc_t*   smBase;
  FwSmDesc_t      smDesc;

  if (nOfTrans < 1)
    return NULL;

  if (nOfStates < 0)
    return NULL;

  if (nOfChoicePseudoStates < 0)
    return NULL;

  if (nOfActions < 0)
    return NULL;

  if (nOfGuards < 0)
    return NULL;

  smDesc = (FwSmDesc_t)malloc(sizeof(struct FwSmDesc));
  if (smDesc == NULL)
    return NULL;

  smBase = (SmBaseDesc_t*)malloc(sizeof(SmBaseDesc_t));
  if (smBase == NULL)
    return NULL;

  if (nOfStates > 0) {
    smBase->pStates = (SmPState_t*)malloc(((FwSmCounterU4_t)(nOfStates)) * sizeof(SmPState_t));
    if (smBase->pStates == NULL)
      return NULL;
    for (i = 0; i < nOfStates; i++)
      smBase->pStates[i].outTransIndex = 0;
    smDesc->esmDesc = (struct FwSmDesc**)malloc(((FwSmCounterU4_t)(nOfStates)) * sizeof(FwSmDesc_t));
    if (smDesc->esmDesc == NULL)
      return NULL;
    for (i = 0; i < nOfStates; i++)
      smDesc->esmDesc[i] = NULL;
  }
  else {
    smBase->pStates = NULL;
    smDesc->esmDesc = NULL;
  }

  if (nOfChoicePseudoStates > 0) {
    smBase->cStates = (SmCState_t*)malloc(((FwSmCounterU4_t)(nOfChoicePseudoStates)) * sizeof(SmCState_t));
    if (smBase->cStates == NULL)
      return NULL;
    for (i = 0; i < nOfChoicePseudoStates; i++)
      smBase->cStates[i].outTransIndex = 0;
  }
  else
    smBase->cStates = NULL;

  smBase->trans = (SmTrans_t*)malloc(((FwSmCounterU4_t)(nOfTrans)) * sizeof(SmTrans_t));
  if (smBase->trans == NULL)
    return NULL;
  for (i = 0; i < nOfTrans; i++)
    smBase->trans[i].iTrAction = -1;

  smDesc->smActions = (FwSmAction_t*)malloc(((FwSmCounterU4_t)(nOfActions + 1)) * sizeof(FwSmAction_t));
  if (smDesc->smActions == NULL)
    return NULL;
  smDesc->smActions[0] = &SmDummyAction;
  for (i = 1; i <= nOfActions; i++)
    smDesc->smActions[i] = NULL;

  smDesc->smGuards = (FwSmGuard_t*)malloc(((FwSmCounterU4_t)(nOfGuards + 1)) * sizeof(FwSmGuard_t));
  if (smDesc->smGuards == NULL)
    return NULL;
  smDesc->smGuards[0] = &SmDummyGuard;
  for (i = 1; i <= nOfGuards; i++)
    smDesc->smGuards[i] = NULL;

  smBase->nOfCStates = nOfChoicePseudoStates;
  smBase->nOfPStates = nOfStates;
  smBase->nOfTrans = nOfTrans;
  smDesc->smBase = smBase;
  smDesc->curState = 0;
  smDesc->smData = NULL;
  smDesc->transCnt = 1;
  smDesc->nOfActions = (FwSmCounterS1_t)(nOfActions + 1);
  smDesc->nOfGuards = (FwSmCounterS1_t)(nOfGuards + 1);
  smDesc->smExecCnt = 0;
  smDesc->stateExecCnt = 0;
  smDesc->errCode = smSuccess;

  return smDesc;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmDesc_t FwSmCreateDer(FwSmDesc_t smDesc) {
  FwSmCounterS1_t i;
  SmBaseDesc_t*   smBase = smDesc->smBase;
  FwSmDesc_t      extSmDesc;

  /* Create descriptor for derived SM */
  extSmDesc = (FwSmDesc_t)malloc(sizeof(struct FwSmDesc));
  if (extSmDesc == NULL)
    return NULL;

  /* Create array of embedded state machines in the derived SM */
  if (smBase->nOfPStates > 0) {
    extSmDesc->esmDesc = (struct FwSmDesc**)malloc(((FwSmCounterU4_t)(smBase->nOfPStates)) * sizeof(FwSmDesc_t));
    if (extSmDesc->esmDesc == NULL)
      return NULL;
  }
  else
    extSmDesc->esmDesc = NULL;

  /* Create array of actions in the derived SM */
  extSmDesc->smActions = (FwSmAction_t*)malloc(((FwSmCounterU4_t)(smDesc->nOfActions)) * sizeof(FwSmAction_t));
  if (extSmDesc->smActions == NULL)
    return NULL;
  for (i = 0; i < smDesc->nOfActions; i++)
    extSmDesc->smActions[i] = smDesc->smActions[i];

  /* Create array of guards in the derived SM */
  extSmDesc->smGuards = (FwSmGuard_t*)malloc(((FwSmCounterU4_t)(smDesc->nOfGuards)) * sizeof(FwSmGuard_t));
  if (extSmDesc->smGuards == NULL)
    return NULL;
  for (i = 0; i < smDesc->nOfGuards; i++)
    extSmDesc->smGuards[i] = smDesc->smGuards[i];

  /* Create embedded state machines */
  for (i = 0; i < smBase->nOfPStates; i++)
    if (smDesc->esmDesc[i] != NULL)
      extSmDesc->esmDesc[i] = FwSmCreateDer(smDesc->esmDesc[i]);
    else
      extSmDesc->esmDesc[i] = NULL;

  extSmDesc->smBase = smBase;
  extSmDesc->curState = 0;
  extSmDesc->smData = NULL;
  extSmDesc->transCnt = 0;
  extSmDesc->nOfActions = smDesc->nOfActions;
  extSmDesc->nOfGuards = smDesc->nOfGuards;
  extSmDesc->errCode = smDesc->errCode;
  extSmDesc->smExecCnt = 0;
  extSmDesc->stateExecCnt = 0;

  return extSmDesc;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmRelease(FwSmDesc_t smDesc) {
  SmBaseDesc_t* smBase;

  /* Release memory allocated to base descriptor */
  smBase = smDesc->smBase;
  if (smBase->pStates != NULL)
    free(smBase->pStates);

  if (smBase->cStates != NULL)
    free(smBase->cStates);

  /* Release pointer to transition array (note that the transition array is guaranteed to exist and to
   * have at least one element, see operation FwSmCreate) */
  free(smBase->trans);

  /* Release memory allocated to base descriptor */
  free(smDesc->smBase);

  /* Release memory allocated to extension part of the state machine descriptor */
  FwSmReleaseDer(smDesc);

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmReleaseDer(FwSmDesc_t smDesc) {

  /* Release pointer to the action and guard arrays (note that both arrays are guaranteed to
   * have non-zero length) */
  free(smDesc->smActions);
  free(smDesc->smGuards);

  if (smDesc->esmDesc != NULL)
    free(smDesc->esmDesc);

  /* Release pointer to state machine descriptor */
  free(smDesc);
  smDesc = NULL;

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmReleaseRec(FwSmDesc_t smDesc) {
  FwSmCounterS1_t i;

  /* Release memory used by the embedded state machines */
  for (i = 0; i < smDesc->smBase->nOfPStates; i++)
    if (smDesc->esmDesc[i] != NULL)
      FwSmReleaseRec(smDesc->esmDesc[i]);

  /* Release memory used by the embedding state machine */
  FwSmRelease(smDesc);

  return;
}
