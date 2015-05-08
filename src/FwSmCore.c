/**
 * @file
 * @ingroup smGroup
 * Implements the core functions of the FW State Machine Module.
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

#include "FwSmCore.h"
#include "FwSmPrivate.h"
#include <stdlib.h>

/**
 *  Private helper function implementing the transition logic from the point where the
 *  transition action is executed to the end of the transition (see figure 4.3-3 of the
 *  FW Profile Definition Document).
 *  This operation may set the error code to #smTransErr if either of the following non-nominal
 *  situations occurs:
 *  - the transition encounters a choice pseudo-state which has no out-going transitions with a true
 *    guard; or
 *  - the transition encounters a transition which has a choice pseudo-state as both source and
 *    destination of the same transition.
 *  .
 *  In both the above cases, the state machine may remain in an inconsistent state.
 *  @param smDesc the descriptor of the state machine where the transition is executed
 *  @param trans the transition which is executed
 */
static void ExecTrans(FwSmDesc_t smDesc, SmTrans_t* trans);

/* ----------------------------------------------------------------------------------------------------------------- */
void SmDummyAction(FwSmDesc_t smDesc) {
	(void)(smDesc);
	return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmBool_t SmDummyGuard(FwSmDesc_t smDesc) {
	(void)(smDesc);
	return 1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmStart(FwSmDesc_t smDesc) {
	SmTrans_t* trans;

	if (smDesc->curState != 0) 	/* Check if SM is already STARTED */
		return;

	/* Reset execution counters */
	smDesc->smExecCnt = 0;
	smDesc->stateExecCnt = 0;

	/* Execution transition into initial state */
	trans = &(smDesc->smBase->trans[0]);
	ExecTrans(smDesc,trans);
}	

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmStop(FwSmDesc_t smDesc) {
	SmPState_t* pState;
	SmBaseDesc_t* smBase = smDesc->smBase;
	FwSmCounterS1_t iCurState = smDesc->curState;

	/* check if state machine (SM) is already stopped */
	if (iCurState == 0)
		return;

	pState = &(smBase->pStates[iCurState-1]);	/* get current state */
	/* If the current state (CS) has an embedded SM (ESM), stop it */
	if (smDesc->esmDesc[iCurState-1] != NULL)
		FwSmStop(smDesc->esmDesc[iCurState-1]);
	/* execute exit action of current state */
	smDesc->smActions[pState->iExitAction](smDesc);
	/* set state of SM to "undefined" */
	smDesc->curState = 0;
	return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmMakeTrans(FwSmDesc_t smDesc, FwSmCounterU2_t transId) {
	SmPState_t* curState;
	SmTrans_t* trans;
	FwSmCounterS1_t i;
	FwSmDesc_t esmDesc;
	SmBaseDesc_t* smBase = smDesc->smBase;

	/* check if state machine (SM) is started */
	if (smDesc->curState == 0)
		return;

	/* get current state */
	curState = &(smBase->pStates[(smDesc->curState)-1]);

	/* If this is the "execute" transition, increment execution counters and execute do-action */
	if (transId == FW_TR_EXECUTE) {
		smDesc->smExecCnt++;
		smDesc->stateExecCnt++;
		smDesc->smActions[curState->iDoAction](smDesc);
	}

	/* If there is an embedded SM (ESM), propagate transition trigger to it */
	esmDesc = smDesc->esmDesc[(smDesc->curState)-1];
	if (esmDesc != NULL)
		FwSmMakeTrans(esmDesc, transId);

	/* look for transition from CS matching transition trigger */
	for (i=0; i<curState->nOfOutTrans; i++) {
		trans = &(smBase->trans[curState->outTransIndex + i]);
		/* check if outgoing transition responds to trigger tr_id */
		if (trans->id == transId)
			/* check if outgoing transition has a true guard */
			if (smDesc->smGuards[trans->iTrGuard](smDesc) != 0) {
				/* If CS has an ESM, stop it before exiting the CS */
				if (esmDesc != NULL)
					FwSmStop(esmDesc);
				/* Execute exit action of CS */
				smDesc->smActions[curState->iExitAction](smDesc);
				ExecTrans(smDesc,trans);
				return;
			}
	}
	return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
void FwSmExecute(FwSmDesc_t smDesc) {
	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
}

/* ----------------------------------------------------------------------------------------------------------------- */
static void ExecTrans(FwSmDesc_t smDesc, SmTrans_t* trans) {
	SmPState_t* pDest;
	SmCState_t* cDest;
	SmTrans_t* cTrans;
	FwSmCounterS1_t i;
	FwSmDesc_t esmDesc;
	SmBaseDesc_t* smBase = smDesc->smBase;

	/* execute transition action */
	smDesc->smActions[trans->iTrAction](smDesc);

	if (trans->dest > 0) {					/* destination is a proper state */
		smDesc->stateExecCnt = 0;
		smDesc->curState = trans->dest;
		pDest = &(smBase->pStates[(trans->dest)-1]);
		/* execute entry action of destination state */
		smDesc->smActions[pDest->iEntryAction](smDesc);
		esmDesc = smDesc->esmDesc[(trans->dest)-1];
		if (esmDesc != NULL)
			FwSmStart(esmDesc);
		return;
	} else if (trans->dest < 0) {			/* destination is a choice pseudo-state */
		cDest = &(smBase->cStates[-(trans->dest)-1]);
		for (i=0; i<cDest->nOfOutTrans; i++) {
			cTrans = &(smBase->trans[cDest->outTransIndex + i]);
			if (smDesc->smGuards[cTrans->iTrGuard](smDesc) != 0) {
				/* Execute transition from choice pseudo-state */
				smDesc->smActions[cTrans->iTrAction](smDesc);
				if (cTrans->dest > 0) {	/* destination is a proper state */
					smDesc->stateExecCnt = 0;
					smDesc->curState = cTrans->dest;
					pDest = &(smBase->pStates[(cTrans->dest)-1]);
					/* execute entry action of destination state */
					smDesc->smActions[pDest->iEntryAction](smDesc);
					esmDesc = smDesc->esmDesc[(cTrans->dest)-1];
					if (esmDesc != NULL)
						FwSmStart(esmDesc);
					return;
				} else if (cTrans->dest == 0) {	/* destination is a final state */
					smDesc->curState = 0;
					return;
				} else
					break;	/* this point is reached only if there is a transition from a CPS to a CPS */
			}
		}
		smDesc->errCode = smTransErr;
	} else {					/* destination is a final pseudo-state */
		smDesc->curState = 0;
		return;
	}
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmDesc_t FwSmGetEmbSmCur(FwSmDesc_t smDesc) {
	if (smDesc->curState > 0)
		return smDesc->esmDesc[(smDesc->curState)-1];
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmDesc_t FwSmGetEmbSm(FwSmDesc_t smDesc, FwSmCounterS1_t i) {
	return smDesc->esmDesc[i-1];
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmCounterS1_t FwSmGetCurState(FwSmDesc_t smDesc) {
	return smDesc->curState;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmCounterS1_t FwSmGetCurStateEmb(FwSmDesc_t smDesc) {
	if (smDesc->curState == 0)
		return -1;

	if (smDesc->esmDesc[(smDesc->curState)-1] != NULL)
		return smDesc->esmDesc[(smDesc->curState)-1]->curState;

	return -1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmBool_t FwSmIsStarted(FwSmDesc_t smDesc) {
	if (smDesc->curState != 0)
		return 1;
	else
		return 0;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmErrCode_t FwSmGetErrCode(FwSmDesc_t smDesc) {
	return smDesc->errCode;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmCounterU3_t FwSmGetExecCnt(FwSmDesc_t smDesc) {
	return smDesc->smExecCnt;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwSmCounterU3_t FwSmGetStateExecCnt(FwSmDesc_t smDesc) {
	return smDesc->stateExecCnt;
}
