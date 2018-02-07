/**
 * @file
 * @ingroup prGroup
 * Implements the configuration functions for the FW Procedure Module.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2011, All Rights Reserved
 *
 * This file is part of the FW Profile.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#include "FwPrConfig.h"
#include "FwPrPrivate.h"
#include <stdlib.h>

/**
 * Create a control flow (other than the control flow from the initial node) with the
 * given characteristics and add it to state machine.
 * The newly created control flow is stored in the control flow array at the first non-null location
 * (in other words, the first control flow to be added to the state machine is stored at location 0,
 * the second control flow is stored at location 1, etc).
 * @param prDesc the descriptor of the state machine to which the control flow is added.
 * @param srcId the identifier of the source of the control flow.
 * @param srcType the type of the source.
 * @param destId the identifier of the destination of the control flow. If the destination is an action
 * node, this argument has a positive value equal to the destination identifier. If the destination
 * is a decision node, this argument has a negative value equal to the opposite of the destination
 * identifier. If the destination is the final node, this argument has the value zero.
 * @param cfGuard the control flow guard (or NULL if no guard is associated to this control flow).
 */
static void AddFlow(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, NodeType_t srcType, FwPrCounterS1_t destId,
                    FwPrGuard_t cfGuard);

/**
 * Add an action to the set of actions in the procedure descriptor.
 * This function scans the array of actions in the procedure descriptor and
 * can then have one of three outcomes:
 * - if the argument action is already present in the array of actions, the function
 *   returns the index of the location where the action is stored;
 * - if the argument action is not present and the array of actions is not full, the
 *   function adds it at the first free location and returns the index of this
 *   location;
 * - if the argument action is not present but the array of actions is already full,
 *   the function returns 0.
 * .
 * Note that the argument action is guaranteed to be different from NULL
 * (because function <code>::FwPrAddAction</code> handles the case of action
 * being equal to NULL and, in that case, it does not call <code>AddAction</code>.
 * @param prDesc descriptor of the procedure where the action is added
 * @param action action to be added
 * @return the location where the action is stored in the array or -1 if the
 * action cannot be added because the array is already full
 */
static FwPrCounterS1_t AddAction(FwPrDesc_t prDesc, FwPrAction_t action);

/**
 * Add a guard to the set of guards in the procedure descriptor.
 * This function scans the array of guards in the procedure descriptor and
 * can then have one of three outcomes:
 * - if the argument guard is equal to NULL, the function returns 0;
 * - if the argument guard is already present in the array of guards, the function
 *   returns the index of the location where the guard is stored;
 * - if the argument guard is not present and the array of guards is not full, the
 *   function adds it at the first free location and returns the index of this
 *   location;
 * - if the argument guard is not present but the array of guards is already full, the
 *   function returns 0.
 * @param prDesc descriptor of the procedure where the guard is added
 * @param guard guard to be added
 * @return the location where the guard is stored in the array or -1 if the
 * guard cannot be added because the array is already full
 */
static FwPrCounterS1_t AddGuard(FwPrDesc_t prDesc, FwPrGuard_t guard);

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrSetData(FwPrDesc_t prDesc, void* prData) {
  prDesc->prData = prData;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void* FwPrGetData(FwPrDesc_t prDesc) {
  return prDesc->prData;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddActionNode(FwPrDesc_t prDesc, FwPrCounterS1_t nodeId, FwPrAction_t action) {

  PrANode_t*    aNode;
  PrBaseDesc_t* prBase = prDesc->prBase;

  if (action == NULL) {
    prDesc->errCode = prNullAction;
    return;
  }

  if (nodeId > prBase->nOfANodes) {
    prDesc->errCode = prIllActNodeId;
    return;
  }

  if (nodeId < 1) {
    prDesc->errCode = prIllActNodeId;
    return;
  }

  if (prBase->aNodes[nodeId - 1].iFlow != -1) {
    prDesc->errCode = prActNodeIdInUse;
    return;
  }

  if (prDesc->flowCnt + 1 > prBase->nOfFlows) {
    prDesc->errCode = prTooManyOutFlows;
    return;
  }

  /* Initialize newly added state */
  aNode = &(prBase->aNodes[nodeId - 1]);

  aNode->iFlow    = prDesc->flowCnt;
  prDesc->flowCnt = (FwPrCounterS1_t)(prDesc->flowCnt + 1);

  aNode->iAction = AddAction(prDesc, action);

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddDecisionNode(FwPrDesc_t prDesc, FwPrCounterS1_t nodeId, FwPrCounterS1_t nOfOutFlows) {
  PrDNode_t*    dNode;
  PrBaseDesc_t* prBase = prDesc->prBase;

  if (nodeId > prBase->nOfDNodes) {
    prDesc->errCode = prIllDecNodeId;
    return;
  }

  if (nodeId < 1) {
    prDesc->errCode = prIllDecNodeId;
    return;
  }

  if (prBase->dNodes[nodeId - 1].outFlowIndex != -1) {
    prDesc->errCode = prDecNodeIdInUse;
    return;
  }

  if (nOfOutFlows < 2) {
    prDesc->errCode = prIllNOfOutFlows;
    return;
  }

  if (prDesc->flowCnt + nOfOutFlows > prBase->nOfFlows) {
    prDesc->errCode = prTooManyOutFlows;
    return;
  }

  /* Initialize newly added decision node */
  dNode = &(prBase->dNodes[nodeId - 1]);

  dNode->outFlowIndex = prDesc->flowCnt;
  prDesc->flowCnt     = (FwPrCounterS1_t)(prDesc->flowCnt + nOfOutFlows);

  dNode->nOfOutTrans = nOfOutFlows;

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowIniToAct(FwPrDesc_t prDesc, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, 0, stoppedNode, destId, cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowIniToDec(FwPrDesc_t prDesc, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, 0, stoppedNode, (FwPrCounterS1_t)(-destId), cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowActToAct(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, actionNode, destId, cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowActToDec(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, actionNode, (FwPrCounterS1_t)(-destId), cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowDecToAct(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, decisionNode, destId, cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowDecToDec(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, decisionNode, (FwPrCounterS1_t)(-destId), cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowActToFin(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, actionNode, 0, cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrAddFlowDecToFin(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrGuard_t cfGuard) {
  AddFlow(prDesc, srcId, decisionNode, 0, cfGuard);
}

/* ----------------------------------------------------------------------------------------------------------------- */
static void AddFlow(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, NodeType_t srcType, FwPrCounterS1_t destId,
                    FwPrGuard_t cfGuard) {

  FwPrCounterS1_t i, baseLoc, nOfOutFlows;
  FwPrCounterS1_t loc = -1;
  PrFlow_t*       flow;
  PrBaseDesc_t*   prBase = prDesc->prBase;

  /* check that the source state is legal and has been defined */
  if (srcType == actionNode) {
    if (srcId > prBase->nOfANodes) {
      prDesc->errCode = prIllFlowSrc;
      return;
    }
    if (srcId < 1) {
      prDesc->errCode = prIllFlowSrc;
      return;
    }
    if (prBase->aNodes[srcId - 1].iFlow == -1) {
      prDesc->errCode = prUndefinedFlowSrc;
      return;
    }
    baseLoc     = prBase->aNodes[srcId - 1].iFlow;
    nOfOutFlows = 1;
  }
  else if (srcType == decisionNode) {
    if (srcId > prBase->nOfDNodes) {
      prDesc->errCode = prIllFlowSrc;
      return;
    }
    if (srcId < 1) {
      prDesc->errCode = prIllFlowSrc;
      return;
    }
    if (prBase->dNodes[srcId - 1].outFlowIndex == -1) {
      prDesc->errCode = prUndefinedFlowSrc;
      return;
    }
    baseLoc     = prBase->dNodes[srcId - 1].outFlowIndex;
    nOfOutFlows = prBase->dNodes[srcId - 1].nOfOutTrans;
  }
  else { /* Source state is the stopped state */
    baseLoc     = 0;
    nOfOutFlows = 1;
  }

  /* New control flow will be stored in the control flow array of the Procedure Descriptor at a location
   * in the range [baseLoc, baseLoc+nOfOutFlows-1]. We check that this range of locations
   * is not already full (note that the checks performed when a node is added to the procedure
   * ensure that the location [baseLoc+nOfOutFlows-1] is within the range of flow array..
   */
  if (prBase->flows[baseLoc + nOfOutFlows - 1].iGuard != -1) {
    prDesc->errCode = prTooManyFlows;
    return;
  }

  /* Identify location where newly added control flow will be stored in the control flow array.
   * Note that the nOfOutFlows is guaranteed to be greater than zero by the way it is
   * initialized in the previous statements in this function. Hence, the loop will be
   * taken at least once. */
  for (i = 0; i < nOfOutFlows; i++) {
    if (prBase->flows[baseLoc + i].iGuard == -1) {
      loc = (FwPrCounterS1_t)(baseLoc + i);
      break;
    }
  }
  flow = &(prBase->flows[loc]);

  /* Assign control flow destination */
  flow->dest = destId;

  /* add guard to control flow descriptor */
  flow->iGuard = AddGuard(prDesc, cfGuard);

  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
static FwPrCounterS1_t AddAction(FwPrDesc_t prDesc, FwPrAction_t action) {
  FwPrCounterS1_t i;

  for (i = 0; i < prDesc->nOfActions; i++) {
    if (prDesc->prActions[i] == NULL) {
      break;
    }
    if (action == prDesc->prActions[i]) {
      return i;
    }
  }

  if (i < prDesc->nOfActions) {
    prDesc->prActions[i] = action;
    return i;
  }

  prDesc->errCode = prTooManyActions;
  return 0;
}

/* ----------------------------------------------------------------------------------------------------------------- */
static FwPrCounterS1_t AddGuard(FwPrDesc_t prDesc, FwPrGuard_t guard) {
  FwPrCounterS1_t i;

  if (guard == NULL) {
    return 0;
  }

  for (i = 1; i < prDesc->nOfGuards; i++) {
    if (prDesc->prGuards[i] == NULL) {
      break;
    }
    if (guard == prDesc->prGuards[i]) {
      return i;
    }
  }

  if (i < prDesc->nOfGuards) {
    prDesc->prGuards[i] = guard;
    return i;
  }

  prDesc->errCode = prTooManyGuards;
  return 0;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrErrCode_t FwPrCheck(FwPrDesc_t prDesc) {

  FwPrCounterS1_t i, j;
  PrBaseDesc_t*   prBase = prDesc->prBase;
  FwPrBool_t      found;

  /* Check that no error occurred during the configuration process */
  if (prDesc->errCode != prSuccess) {
    return prConfigErr;
  }

  /* Check that all action nodes have been defined */
  for (i = 0; i < prBase->nOfANodes; i++) {
    if (prBase->aNodes[i].iFlow == -1) {
      return prNullActNode;
    }
  }

  /* Check that all decision nodes have been defined */
  for (i = 0; i < prBase->nOfDNodes; i++) {
    if (prBase->dNodes[i].outFlowIndex == -1) {
      return prNullDecNode;
    }
  }

  /* Check that all control flows have been defined */
  for (i = 0; i < prBase->nOfFlows; i++) {
    if (prBase->flows[i].iGuard == -1) {
      return prNullFlow;
    }
  }

  /* Check that all control flow destinations are legal action nodes or decision nodes */
  for (i = 0; i < prBase->nOfFlows; i++) {
    if (prBase->flows[i].dest > prBase->nOfANodes) {
      return prIllegalADest;
    }
    if (prBase->flows[i].dest < -prBase->nOfDNodes) {
      return prIllegalDDest;
    }
  }

  /* Check that all actions have been defined */
  for (i = 0; i < prDesc->nOfActions; i++) {
    if (prDesc->prActions[i] == NULL) {
      return prTooFewActions;
    }
  }

  /* Check that all guards have been defined */
  for (i = 0; i < prDesc->nOfGuards; i++) {
    if (prDesc->prGuards[i] == NULL) {
      return prTooFewGuards;
    }
  }

  /* Check that all action nodes are reachable */
  for (i = 1; i <= prBase->nOfANodes; i++) {
    found = 0;
    for (j = 0; j < prBase->nOfFlows; j++) {
      if (prBase->flows[j].dest == i) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      return prUnreachableANode;
    }
  }

  /* Check that all decision nodes are reachable */
  for (i = 1; i <= prBase->nOfDNodes; i++) {
    found = 0;
    for (j = 0; j < prBase->nOfFlows; j++) {
      if (prBase->flows[j].dest == -i) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      return prUnreachableDNode;
    }
  }

  return prSuccess;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrOverrideAction(FwPrDesc_t prDesc, FwPrAction_t oldAction, FwPrAction_t newAction) {
  FwPrCounterS1_t i;

  if (prDesc->flowCnt != 0) {
    prDesc->errCode = prNotDerivedPr;
    return;
  }

  for (i = 0; i < prDesc->nOfActions; i++) {
    if (prDesc->prActions[i] == oldAction) {
      prDesc->prActions[i] = newAction;
      return;
    }
  }

  prDesc->errCode = prUndefAction;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrOverrideGuard(FwPrDesc_t prDesc, FwPrGuard_t oldGuard, FwPrGuard_t newGuard) {
  FwPrCounterS1_t i;

  if (prDesc->flowCnt != 0) {
    prDesc->errCode = prNotDerivedPr;
    return;
  }

  for (i = 1; i < prDesc->nOfGuards; i++) {
    if (prDesc->prGuards[i] == oldGuard) {
      prDesc->prGuards[i] = newGuard;
      return;
    }
  }

  prDesc->errCode = prUndefGuard;
}
