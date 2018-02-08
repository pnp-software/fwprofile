/**
 * @file
 * @ingroup prGroup
 * Implements Core Functions of the FW Procedure.
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

#include "FwPrCore.h"
#include "FwPrPrivate.h"
#include <stdlib.h>

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrBool_t PrDummyGuard(FwPrDesc_t prDesc) {
  (void)(prDesc);
  return 1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrStart(FwPrDesc_t prDesc) {
  if (prDesc->curNode == 0) {
    prDesc->curNode     = -1;
    prDesc->prExecCnt   = 0;
    prDesc->nodeExecCnt = 0;
  }
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrStop(FwPrDesc_t prDesc) {
  prDesc->curNode = 0;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrExecute(FwPrDesc_t prDesc) {
  PrANode_t*      curNode;
  PrDNode_t*      decNode;
  PrFlow_t*       flow;
  FwPrCounterS1_t i;
  PrBaseDesc_t*   prBase = prDesc->prBase;
  FwPrCounterS1_t trueGuardFound;

  /* check if procedure is started */
  if (prDesc->curNode == 0) { /* procedure is stopped */
    return;
  }

  prDesc->prExecCnt++;   /* Increment procedure execution counter */
  prDesc->nodeExecCnt++; /* Increment node execution counter */

  /* Get the Control Flow issuing from the current node */
  if (prDesc->curNode == -1) { /* procedure is at initial node */
    flow = &(prBase->flows[0]);
  }
  else {
    curNode = &(prBase->aNodes[prDesc->curNode - 1]); /* procedure is at an action node */
    flow    = &(prBase->flows[curNode->iFlow]);
  }

  /* Evaluate guard of control flow issuing from current node */
  trueGuardFound = (FwPrCounterS1_t)prDesc->prGuards[flow->iGuard](prDesc);

  /* Execute loop as long as guard of control flow issuing from current node is true */
  while (trueGuardFound) {
    /* Target of flow is a final node */
    if (flow->dest == 0) {
      prDesc->curNode = 0; /* Stop procedure */
      return;
    }

    if (flow->dest > 0) { /* Target of control flow is an action node */
      prDesc->curNode     = flow->dest;
      prDesc->nodeExecCnt = 0;
      curNode             = &(prBase->aNodes[(prDesc->curNode) - 1]);
      prDesc->prActions[curNode->iAction](prDesc);
      flow           = &(prBase->flows[curNode->iFlow]);
      trueGuardFound = (FwPrCounterS1_t)prDesc->prGuards[flow->iGuard](prDesc);
    }
    else { /* Target of flow is a decision node */
      trueGuardFound = 0;
      decNode        = &(prBase->dNodes[(-flow->dest) - 1]);
      /* Evaluate guards of control flows issuing from decision node */
      for (i = 0; i < decNode->nOfOutTrans; i++) {
        flow = &(prBase->flows[decNode->outFlowIndex + i]);
        if (prDesc->prGuards[flow->iGuard](prDesc) != 0) {
          trueGuardFound = 1;
          break; /* First control flow out of dec. node with true guard */
        }
      }
      /* All control flows out of decision node have false guards */
      if (trueGuardFound == 0) {
        prDesc->errCode = prFlowErr;
        return;
      }
    }
  }
  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrRun(FwPrDesc_t prDesc) {
  FwPrStart(prDesc);
  FwPrExecute(prDesc);
  FwPrStop(prDesc);
  return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrCounterS1_t FwPrGetCurNode(FwPrDesc_t prDesc) {
  return prDesc->curNode;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrBool_t FwPrIsStarted(FwPrDesc_t prDesc) {
  if (prDesc->curNode == 0) {
    return 0;
  }

  return 1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrErrCode_t FwPrGetErrCode(FwPrDesc_t prDesc) {
  return prDesc->errCode;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrCounterU3_t FwPrGetExecCnt(FwPrDesc_t prDesc) {
  return prDesc->prExecCnt;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrCounterU3_t FwPrGetNodeExecCnt(FwPrDesc_t prDesc) {
  return prDesc->nodeExecCnt;
}
