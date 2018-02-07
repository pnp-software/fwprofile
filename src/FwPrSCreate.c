/**
 * @file
 * @ingroup prGroup
 * Implements the static initialization functions for the FW
 * Procedure Module.
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

#include "FwPrPrivate.h"
#include <stdlib.h>

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrInit(FwPrDesc_t prDesc) {

  FwPrCounterS1_t i;
  PrBaseDesc_t*   prBase = prDesc->prBase;

  for (i = 0; i < prBase->nOfANodes; i++) {
    prBase->aNodes[i].iFlow = -1;
  }

  for (i = 0; i < prBase->nOfDNodes; i++) {
    prBase->dNodes[i].outFlowIndex = -1;
  }

  for (i = 0; i < prBase->nOfFlows; i++) {
    prBase->flows[i].iGuard = -1;
  }

  for (i = 0; i < prDesc->nOfActions; i++) {
    prDesc->prActions[i] = NULL;
  }

  prDesc->prGuards[0] = &PrDummyGuard;
  for (i = 1; i < prDesc->nOfGuards; i++) {
    prDesc->prGuards[i] = NULL;
  }
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwPrInitDer(FwPrDesc_t prDesc, FwPrDesc_t prDescBase) {
  FwPrCounterS1_t i;
  PrBaseDesc_t*   prBase = prDescBase->prBase;

  if (prDesc->nOfActions != prDescBase->nOfActions) {
    prDesc->errCode = prWrongNOfActions;
    return;
  }

  if (prDesc->nOfGuards != prDescBase->nOfGuards) {
    prDesc->errCode = prWrongNOfGuards;
    return;
  }

  prDesc->prBase = prBase;

  /* This cycle will always be executed at least once because
   * the number of actions (nOfActions) is always greater than
   * zero (since all procedures have at least one action node)
   */
  for (i = 0; i < prDesc->nOfActions; i++) {
    prDesc->prActions[i] = prDescBase->prActions[i];
  }

  /* This cycle will always be executed at least once because
   * the number of guards (nOfGuards) is always greater than
   * zero (since all procedures have at least the dummy guard)
   */
  for (i = 0; i < prDesc->nOfGuards; i++) {
    prDesc->prGuards[i] = prDescBase->prGuards[i];
  }

  prDesc->errCode = prDescBase->errCode;

  prDesc->flowCnt     = 0;
  prDesc->curNode     = 0;
  prDesc->nodeExecCnt = 0;
  prDesc->prExecCnt   = 0;

  return;
}
