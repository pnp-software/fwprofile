/**
 * @file
 * @ingroup smGroup
 * Implements the configuration functions for the FW State Machine Module.
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

#include "FwSmConfig.h"
#include "FwSmPrivate.h"
#include <stdlib.h>

/**
 * Create a transition (other than the transition from the initial pseudo-state) with the
 * given characteristics and add it to state machine.
 * The newly created transition is stored in the transition array at the first non-null location
 * (in other words, the first transition to be added to the state machine is stored at location 0,
 * the second transition is stored at location 1, etc).
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param transId the identifier of the trigger which is associated to the transition. This is an integer
 * which is unique for the state machine and for all its embedded state machines. The zero identifier is
 * reserved for the "execute" transition.
 * @param srcId the identifier of the source of the transition.
 * @param srcType the type of the source.
 * @param destId the identifier of the destination of the transition. If the destination is a proper
 * state, this argument has a positive value equal to the destination identifier. If the destination
 * is a choice pseudo-state, this argument has a negative value equal to the opposite of the destination
 * identifier. If the destination is the final pseudo-state, this argument has the value zero.
 * @param trAction the transition action (or NULL if no action is associated to this transition).
 * @param trGuard the transition guard (or NULL if no guard is associated to this transition).
 */
static void AddTrans(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, StateType_t srcType,
                     FwSmCounterS1_t destId, FwSmAction_t trAction, FwSmGuard_t trGuard);

/**
 * Add an action to the set of actions in the state machine descriptor.
 * This function scans the array of actions in the state machine descriptor and
 * can then have one of three outcomes:
 * - if the argument action is equal to NULL, the function returns 0;
 * - if the argument action is already present in the array of actions, the function
 *   returns the index of the location where the action is stored;
 * - if the argument action is not present and the array of actions is not full, the
 *   function adds it at the first free location and returns the index of this
 *   location;
 * - if the argument action is not present but the array of actions is already full,
 *   the function returns 0.
 * @param smDesc descriptor of the state machine where the action is added
 * @param action action to be added
 * @return the location where the action is stored in the array or -1 if the
 * action cannot be added because the array is already full
 */
static FwSmCounterS1_t AddAction(FwSmDesc_t smDesc, FwSmAction_t action);

/**
 * Add a guard to the set of guards in the state machine descriptor.
 * This function scans the array of guards in the state machine descriptor and
 * can then have one of three outcomes:
 * - if the argument guard is equal to NULL, the function returns 0;
 * - if the argument guard is already present in the array of guards, the function
 *   returns the index of the location where the guard is stored;
 * - if the argument guard is not present and the array of guards is not full, the
 *   function adds it at the first free location and returns the index of this
 *   location;
 * - if the argument guard is not present but the array of guards is already full, the
 *   function returns 0.
 * @param smDesc descriptor of the state machine where the guard is added
 * @param guard guard to be added
 * @return the location where the guard is stored in the array or -1 if the
 * guard cannot be added because the array is already full
 */
static FwSmCounterS1_t AddGuard(FwSmDesc_t smDesc, FwSmGuard_t guard);

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmSetData(FwSmDesc_t smDesc, void* smData) {
  smDesc->smData = smData;
}

void* FwSmGetData(FwSmDesc_t smDesc) {
  return smDesc->smData;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddState(FwSmDesc_t smDesc, FwSmCounterS1_t stateId, FwSmCounterS1_t nOfOutTrans, FwSmAction_t entryAction,
                  FwSmAction_t exitAction, FwSmAction_t doAction, FwSmDesc_t esmDesc) {

  SmPState_t*   pState;
  SmBaseDesc_t* smBase = smDesc->smBase;

  if (stateId > smBase->nOfPStates) {
    smDesc->errCode = smIllStateId;
    return;
  }

  if (stateId < 1) {
    smDesc->errCode = smIllStateId;
    return;
  }

  if (smBase->pStates[stateId - 1].outTransIndex != 0) {
    smDesc->errCode = smStateIdInUse;
    return;
  }

  if (nOfOutTrans < 0) {
    smDesc->errCode = smNegOutTrans;
    return;
  }

  if (smDesc->transCnt + nOfOutTrans > smBase->nOfTrans) {
    smDesc->errCode = smTooManyOutTrans;
    return;
  }

  /* Initialize newly added state */
  pState = &(smBase->pStates[stateId - 1]);

  pState->outTransIndex = smDesc->transCnt;
  smDesc->transCnt      = (FwSmCounterS1_t)(smDesc->transCnt + nOfOutTrans);

  pState->iDoAction    = AddAction(smDesc, doAction);
  pState->iEntryAction = AddAction(smDesc, entryAction);
  pState->iExitAction  = AddAction(smDesc, exitAction);

  smDesc->esmDesc[stateId - 1] = esmDesc;
  pState->nOfOutTrans          = nOfOutTrans;

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddChoicePseudoState(FwSmDesc_t smDesc, FwSmCounterS1_t choiceId, FwSmCounterS1_t nOfOutTrans) {
  SmCState_t*   cState;
  SmBaseDesc_t* smBase = smDesc->smBase;

  if (choiceId > smBase->nOfCStates) {
    smDesc->errCode = smIllChoiceId;
    return;
  }

  if (choiceId < 1) {
    smDesc->errCode = smIllChoiceId;
    return;
  }

  if (smBase->cStates[choiceId - 1].outTransIndex != 0) {
    smDesc->errCode = smChoiceIdInUse;
    return;
  }

  if (nOfOutTrans < 1) {
    smDesc->errCode = smIllNOfOutTrans;
    return;
  }

  if (smDesc->transCnt + nOfOutTrans > smBase->nOfTrans) {
    smDesc->errCode = smTooManyOutTrans;
    return;
  }

  /* Initialize newly added choice pseudo-state */
  cState = &(smBase->cStates[choiceId - 1]);

  cState->outTransIndex = smDesc->transCnt;
  smDesc->transCnt      = (FwSmCounterS1_t)(smDesc->transCnt + nOfOutTrans);

  cState->nOfOutTrans = nOfOutTrans;

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransIpsToSta(FwSmDesc_t smDesc, FwSmCounterS1_t destId, FwSmAction_t trAction) {
  AddTrans(smDesc, 0, 0, stoppedState, destId, trAction, NULL);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransIpsToCps(FwSmDesc_t smDesc, FwSmCounterS1_t destId, FwSmAction_t trAction) {
  AddTrans(smDesc, 0, 0, stoppedState, (FwSmCounterS1_t)(-destId), trAction, NULL);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransStaToSta(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmCounterS1_t destId,
                          FwSmAction_t trAction, FwSmGuard_t trGuard) {
  AddTrans(smDesc, transId, srcId, properState, destId, trAction, trGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransStaToCps(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmCounterS1_t destId,
                          FwSmAction_t trAction, FwSmGuard_t trGuard) {
  AddTrans(smDesc, transId, srcId, properState, (FwSmCounterS1_t)(-destId), trAction, trGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransCpsToSta(FwSmDesc_t smDesc, FwSmCounterS1_t srcId, FwSmCounterS1_t destId, FwSmAction_t trAction,
                          FwSmGuard_t trGuard) {
  AddTrans(smDesc, 0, srcId, choicePseudoState, destId, trAction, trGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransStaToFps(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmAction_t trAction,
                          FwSmGuard_t trGuard) {
  AddTrans(smDesc, transId, srcId, properState, 0, trAction, trGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmAddTransCpsToFps(FwSmDesc_t smDesc, FwSmCounterS1_t srcId, FwSmAction_t trAction, FwSmGuard_t trGuard) {
  AddTrans(smDesc, 0, srcId, choicePseudoState, 0, trAction, trGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
static void AddTrans(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, StateType_t srcType,
                     FwSmCounterS1_t destId, FwSmAction_t trAction, FwSmGuard_t trGuard) {

  FwSmCounterS1_t i, baseLoc, nOfOutTrans;
  FwSmCounterS1_t loc = -1;
  SmTrans_t*      trans;
  SmBaseDesc_t*   smBase = smDesc->smBase;

  /* check that the source state is legal and has been defined */
  if (srcType == properState) {
    if (srcId > smBase->nOfPStates) {
      smDesc->errCode = smIllTransSrc;
      return;
    }
    if (smBase->pStates[srcId - 1].outTransIndex == 0) {
      smDesc->errCode = smUndefinedTransSrc;
      return;
    }
    if (smBase->pStates[srcId - 1].nOfOutTrans < 1) {
      smDesc->errCode = smIllTransSrc;
      return;
    }
    baseLoc     = smBase->pStates[srcId - 1].outTransIndex;
    nOfOutTrans = smBase->pStates[srcId - 1].nOfOutTrans;
  }
  else if (srcType == choicePseudoState) {
    if (srcId > smBase->nOfCStates) {
      smDesc->errCode = smIllTransSrc;
      return;
    }
    if (smBase->cStates[srcId - 1].outTransIndex == 0) {
      smDesc->errCode = smUndefinedTransSrc;
      return;
    }
    baseLoc     = smBase->cStates[srcId - 1].outTransIndex;
    nOfOutTrans = smBase->cStates[srcId - 1].nOfOutTrans;
  }
  else { /* Source state is the stopped state */
    baseLoc     = 0;
    nOfOutTrans = 1;
  }

  /* New transition will be stored in the transition array of the SM Descriptor at a location
   * in the range [baseLoc, baseLoc+nOfOutTrans]. We check that this range of locations
   * is not already full.
   */
  if (smBase->trans[baseLoc + nOfOutTrans - 1].iTrAction != -1) {
    smDesc->errCode = smTooManyTrans;
    return;
  }

  /* Identify location where newly added transition will be stored in the transition array.
   * Note that the nOfOutTrans is guaranteed to be greater than zero by the way it is
   * initialized in the previous statements in this function. Hence, the loop will be
   * taken at least once. */
  for (i = 0; i < nOfOutTrans; i++) {
    if (smBase->trans[baseLoc + i].iTrAction == -1) {
      loc = (FwSmCounterS1_t)(baseLoc + i);
      break;
    }
  }
  trans = &(smBase->trans[loc]);

  /* Assign the transition identifier */
  trans->id = transId;

  /* Assign transition destination */
  trans->dest = destId;

  /* add transition action to transition descriptor */
  trans->iTrAction = AddAction(smDesc, trAction);

  /* add guard to transition descriptor */
  trans->iTrGuard = AddGuard(smDesc, trGuard);

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
static FwSmCounterS1_t AddAction(FwSmDesc_t smDesc, FwSmAction_t action) {
  FwSmCounterS1_t i;

  if (action == NULL) {
    return 0;
  }

  for (i = 1; i < smDesc->nOfActions; i++) {
    if (smDesc->smActions[i] == NULL) {
      break;
    }
    if (action == smDesc->smActions[i]) {
      return i;
    }
  }

  if (i < smDesc->nOfActions) {
    smDesc->smActions[i] = action;
    return i;
  }

  smDesc->errCode = smTooManyActions;
  return 0;
}

/* ----------------------------------------------------------------------------------------------------------------- */
static FwSmCounterS1_t AddGuard(FwSmDesc_t smDesc, FwSmGuard_t guard) {
  FwSmCounterS1_t i;

  if (guard == NULL) {
    return 0;
  }

  for (i = 1; i < smDesc->nOfGuards; i++) {
    if (smDesc->smGuards[i] == NULL) {
      break;
    }
    if (guard == smDesc->smGuards[i]) {
      return i;
    }
  }

  if (i < smDesc->nOfGuards) {
    smDesc->smGuards[i] = guard;
    return i;
  }

  smDesc->errCode = smTooManyGuards;
  return 0;
}
/* ----------------------------------------------------------------------------------------------------------------- */
FwSmErrCode_t FwSmCheck(FwSmDesc_t smDesc) {

  FwSmCounterS1_t i, j;
  FwSmBool_t      found;
  SmBaseDesc_t*   smBase = smDesc->smBase;

  /* Check that no error occurred during the configuration process */
  if (smDesc->errCode != smSuccess) {
    return smConfigErr;
  }

  /* Check that all proper states have been defined */
  for (i = 0; i < smBase->nOfPStates; i++) {
    if (smBase->pStates[i].outTransIndex == 0) {
      return smNullPState;
    }
  }

  /* Check that all choice pseudo-states have been defined */
  for (i = 0; i < smBase->nOfCStates; i++) {
    if (smBase->cStates[i].outTransIndex == 0) {
      return smNullCState;
    }
  }

  /* Check that all transitions have been defined */
  for (i = 0; i < smBase->nOfTrans; i++) {
    if (smBase->trans[i].iTrAction == -1) {
      return smNullTrans;
    }
  }

  /* Check that all transition destinations are legal states or choice pseudo-states */
  for (i = 0; i < smBase->nOfTrans; i++) {
    if (smBase->trans[i].dest > smBase->nOfPStates) {
      return smIllegalPDest;
    }
    if (smBase->trans[i].dest < -smBase->nOfCStates) {
      return smIllegalCDest;
    }
  }

  /* Check that all actions have been defined */
  for (i = 0; i < smDesc->nOfActions; i++) {
    if (smDesc->smActions[i] == NULL) {
      return smTooFewActions;
    }
  }

  /* Check that all guards have been defined */
  for (i = 0; i < smDesc->nOfGuards; i++) {
    if (smDesc->smGuards[i] == NULL) {
      return smTooFewGuards;
    }
  }

  /* Check that all states are reachable */
  for (i = 1; i <= smBase->nOfPStates; i++) {
    found = 0;
    for (j = 0; j < smBase->nOfTrans; j++) {
      if (smBase->trans[j].dest == i) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      return smUnreachablePState;
    }
  }

  /* Check that all choice pseudo-states are reachable */
  for (i = 1; i <= smBase->nOfCStates; i++) {
    found = 0;
    for (j = 0; j < smBase->nOfTrans; j++) {
      if (smBase->trans[j].dest == -i) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      return smUnreachableCState;
    }
  }

  return smSuccess;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmErrCode_t FwSmCheckRec(FwSmDesc_t smDesc) {
  FwSmErrCode_t   outcome;
  FwSmCounterS1_t i;

  /* Check all embedded state machines */
  for (i = 0; i < smDesc->smBase->nOfPStates; i++) {
    if (smDesc->esmDesc[i] != NULL) {
      outcome = FwSmCheckRec(smDesc->esmDesc[i]);
      if (outcome != smSuccess) {
        return outcome;
      }
    }
  }

  return FwSmCheck(smDesc);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmOverrideAction(FwSmDesc_t smDesc, FwSmAction_t oldAction, FwSmAction_t newAction) {
  FwSmCounterS1_t i;

  if (smDesc->transCnt != 0) {
    smDesc->errCode = smNotDerivedSM;
    return;
  }

  for (i = 1; i < smDesc->nOfActions; i++) {
    if (smDesc->smActions[i] == oldAction) {
      smDesc->smActions[i] = newAction;
      return;
    }
  }

  smDesc->errCode = smUndefAction;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmOverrideGuard(FwSmDesc_t smDesc, FwSmGuard_t oldGuard, FwSmGuard_t newGuard) {
  FwSmCounterS1_t i;

  if (smDesc->transCnt != 0) {
    smDesc->errCode = smNotDerivedSM;
    return;
  }

  for (i = 1; i < smDesc->nOfGuards; i++) {
    if (smDesc->smGuards[i] == oldGuard) {
      smDesc->smGuards[i] = newGuard;
      return;
    }
  }

  smDesc->errCode = smUndefGuard;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmEmbed(FwSmDesc_t smDesc, FwSmCounterS1_t stateId, FwSmDesc_t esmDesc) {

  if (smDesc->transCnt != 0) {
    smDesc->errCode = smNotDerivedSM;
    return;
  }

  if (stateId < 1) {
    smDesc->errCode = smIllStateId;
    return;
  }

  if (stateId > smDesc->smBase->nOfPStates) {
    smDesc->errCode = smIllStateId;
    return;
  }

  if (smDesc->esmDesc[stateId - 1] != NULL) {
    smDesc->errCode = smEsmDefined;
    return;
  }

  smDesc->esmDesc[stateId - 1] = esmDesc;
  return;
}
