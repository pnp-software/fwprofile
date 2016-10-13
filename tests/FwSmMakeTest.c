/**
 * @file
 * @ingroup tsSmGroup
 * Implementation of test state machines for FW State Machine Module.
 *
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2013, All Rights Reserved
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

#include "FwSmMakeTest.h"
#include "FwSmCore.h"
#include "FwSmConfig.h"
#include "FwSmDCreate.h"
#include "FwSmSCreate.h"
#include "FwSmPrivate.h"	/* Needed for direct creation and configuration */
#include <stdlib.h>

/**
 * Log array where the actions of the test state machines write their markers.
 */
int fwSm_logMarker[LOG_ARRAY_SIZE];
/**
 * Log array where the actions of the test state machines write the state of their
 * state machine.
 */
int fwSm_logState[LOG_ARRAY_SIZE];
/**
 * Location in the log array where the next entry is written.
 */
int fwSm_logIndex;

/**
 * Get the pointer to the state machine data of a test state machine.
 * This function is a wrapper for the <code>::FwSmGetData</code> function.
 * Function <code>::FwSmGetData</code> is defined at the level of the State
 * Machine Module and returns the pointer to the state machine data as a
 * pointer to <code>void</code>.
 * This function cast this pointer to pointer to
 * <code>struct TestSmData*</code>.
 * @param smDesc the state machine descriptor
 * @return the state machine data
 */
struct TestSmData* GetTestSmData(FwSmDesc_t smDesc);

/**
 * Operation used as entry action in the test state machines.
 * @param smDesc the state machine descriptor
 */
static void incrCnt1By1(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_1++;
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 1;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Operation used as transition action in the test state machines.
 * @param smDesc the state machine descriptor
 */
static void incrCnt2By1(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_2++;
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 4;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Operation used as do-action in the test state machines.
 * @param smDesc the state machine descriptor
 */
static void incrCnt1By2(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_1 = smData->counter_1+2;
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 2;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Operation used as exit action in the test state machines.
 * @param smDesc the state machine descriptor
 */
static void incrCnt1By4(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_1 = smData->counter_1+4;
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 3;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Operation used in the derived state machines to override actions in the base
 * state machine.
 * This operation writes a marker equal to 10 to the log file.
 * @param smDesc the state machine descriptor
 */
static void incrCnt1By8(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_1 = smData->counter_1+8;
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 3;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Operation used as state action in the test state machines.
 * This operation assigns the value of the State Machine
 * Execution Counter to counter_1 and the value of the State
 * Execution Counter to counter_2.
 * @param smDesc the state machine descriptor
 */
static void assignExecCnt(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	smData->counter_1 = FwSmGetExecCnt(smDesc);
	smData->counter_2 = FwSmGetStateExecCnt(smDesc);
	fwSm_logMarker[fwSm_logIndex] = smData->logBase + 20;
	fwSm_logState[fwSm_logIndex] = smData->logBase+FwSmGetCurState(smDesc);
	fwSm_logIndex++;
}

/**
 * Guard which returns the value of <code>flag_1</code>.
 * @param smDesc the state machine descriptor
 * @return the value of <code>flag_1</code>
 */
static FwSmBool_t retFlag1(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	return (smData->flag_1 == 1);
}

/**
 * Guard which returns the value of <code>flag_2</code>.
 * @param smDesc the state machine descriptor
 * @return the value of <code>flag_2</code>
 */
static FwSmBool_t retFlag2(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	return (smData->flag_2 == 1);
}

/**
 * Guard which returns the value of <code>flag_3</code>.
 * @param smDesc the state machine descriptor
 * @return the value of <code>flag_3</code>
 */
static FwSmBool_t retFlag3(FwSmDesc_t smDesc) {
	struct TestSmData* smData = GetTestSmData(smDesc);
	return (smData->flag_3 == 1);
}

/**
 * Operation used as transition guard in the test state machines.
 * This guard returns TRUE after one cycle has
 * elapsed in the current state.
 * @param smDesc the state machine descriptor
 */
static FwSmBool_t wait1Cycle(FwSmDesc_t smDesc) {
	return (FwSmGetStateExecCnt(smDesc) == 1);
}

/**
 * Operation used as transition guard in the test state machines.
 * This guard returns TRUE after two cycles have
 * elapsed in the current state.
 * @param smDesc the state machine descriptor
 */
static FwSmBool_t wait2Cycle(FwSmDesc_t smDesc) {
	return (FwSmGetStateExecCnt(smDesc) == 2);
}

/**
 * Operation used as transition guard in the test state machines.
 * This guard returns TRUE if the state machine execution counter
 * is equal to 5.
 * @param smDesc the state machine descriptor
 */
static FwSmBool_t smExecCntEqual4(FwSmDesc_t smDesc) {
	return (FwSmGetExecCnt(smDesc) == 4);
}

/**
 * Operation used as transition guard in the test state machines.
 * This guard returns TRUE if the state machine execution counter
 * is smaller than 5.
 * @param smDesc the state machine descriptor
 */
static FwSmBool_t smExecCntSmallerThan4(FwSmDesc_t smDesc) {
	return (FwSmGetExecCnt(smDesc) < 4);
}

struct TestSmData* GetTestSmData(FwSmDesc_t smDesc) {
	return (struct TestSmData*)FwSmGetData(smDesc);
}


/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM1(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 3;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 1;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, NULL, &incrCnt1By2, NULL);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToFps(p_sm, TR_S1_FPS, STATE_S1, &incrCnt2By1, &retFlag1);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM1Static(struct TestSmData* smData) {

	/* Create the state machine */
	FW_SM_INST_NOCPS(smDesc, 1, 2, 3, 1)
	FwSmInit(&smDesc);

	/* Configure the state machine */
	FwSmSetData(&smDesc, smData);
	FwSmAddState(&smDesc, STATE_S1, 1, &incrCnt1By1, NULL, &incrCnt1By2, NULL);
	FwSmAddTransIpsToSta(&smDesc, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToFps(&smDesc, TR_S1_FPS, STATE_S1, &incrCnt2By1, &retFlag1);
	return &smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM2(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToCps(p_sm, CPS1, &incrCnt2By1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag2);
	FwSmAddTransStaToFps(p_sm, TR_S1_FPS, STATE_S1, &incrCnt2By1, &retFlag2);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM3(struct TestSmData* smData, struct TestSmData* esmData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 3;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 1;	/* number of guards */
	FwSmDesc_t p_sm;
	FwSmDesc_t p_esm;

	/* Create and configure the embedded state machine (the state machine is initially
	 * configured without any data; its data are added to the state machine separately)
	 */
	p_esm = FwSmMakeTestSM2(NULL);

	/* Create the embedding state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, NULL, &incrCnt1By2, p_esm);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToFps(p_sm, TR_S1_FPS, STATE_S1, &incrCnt2By1, &retFlag1);

	/* Set the data for the embedded state machine */
	FwSmSetData(p_esm, esmData);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM4(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 6;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 1;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddState(p_sm, STATE_S2, 4, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToSta(p_sm, FW_TR_EXECUTE, STATE_S1, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, FW_TR_EXECUTE, STATE_S2, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, TR6, STATE_S2, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToFps(p_sm, TR3, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, TR4, STATE_S2, STATE_S2, &incrCnt2By1, &retFlag1);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM5(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 7;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create the state machine */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(smDesc, smData);
	FwSmAddState(smDesc, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddState(smDesc, STATE_S2, 3, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToSta(smDesc, TR2, STATE_S1, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToCps(smDesc, TR6, STATE_S2, CPS1, &incrCnt2By1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S2, &incrCnt2By1, &retFlag2);
	FwSmAddTransStaToFps(smDesc, TR5, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(smDesc, TR4, STATE_S2, STATE_S2, &incrCnt2By1, &retFlag1);
	return smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM5Static(struct TestSmData* smData) {

	/* Create the state machine */
	FW_SM_INST(smDesc, 2, 1, 7, 4, 2)
	FwSmInit(&smDesc);

	/* Configure the state machine */
	FwSmSetData(&smDesc, smData);
	FwSmAddState(&smDesc, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddState(&smDesc, STATE_S2, 3, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(&smDesc, CPS1, 2);
	FwSmAddTransIpsToSta(&smDesc, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToSta(&smDesc, TR2, STATE_S1, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToCps(&smDesc, TR6, STATE_S2, CPS1, &incrCnt2By1, NULL);
	FwSmAddTransCpsToSta(&smDesc, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToSta(&smDesc, CPS1, STATE_S2, &incrCnt2By1, &retFlag2);
	FwSmAddTransStaToFps(&smDesc, TR5, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(&smDesc, TR4, STATE_S2, STATE_S2, &incrCnt2By1, &retFlag1);
	return &smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM5Dir(struct TestSmData* smData) {

	static SmTrans_t trans[7];
	static FwSmAction_t actions[5];
	static FwSmGuard_t guards[3];
	static SmPState_t pStates[2];
	static SmCState_t cStates[1];
	static FwSmDesc_t esmDesc[2];
	static struct FwSmDesc smDesc;
	static SmBaseDesc_t smBase;

	/* Configure the array of state machine actions */
	actions[0] = &SmDummyAction;
	actions[1] = &incrCnt1By1;
	actions[2] = &incrCnt1By2;
	actions[3] = &incrCnt1By4;
	actions[4] = &incrCnt2By1;

	/* Configure the array of state machine guards */
	guards[0] = &SmDummyGuard;
	guards[1] = &retFlag1;
	guards[2] = &retFlag2;

	/* Configure the array of embedded state machines */
	esmDesc[0] = NULL;
	esmDesc[1] = NULL;

	/* Configure the array of proper states */
	pStates[0].outTransIndex = 1;
	pStates[0].nOfOutTrans = 1;
	pStates[0].iEntryAction = 1;
	pStates[0].iDoAction = 2;
	pStates[0].iExitAction = 3;

	pStates[1].outTransIndex = 2;
	pStates[1].nOfOutTrans = 3;
	pStates[1].iEntryAction = 1;
	pStates[1].iDoAction = 2;
	pStates[1].iExitAction = 3;

	/* Configure the array of choice pseudo-states */
	cStates[0].outTransIndex = 5;
	cStates[0].nOfOutTrans = 2;

	/* Configure the array of transitions */
	trans[0].dest = STATE_S1;
	trans[0].id = 0;
	trans[0].iTrAction = 4;
	trans[0].iTrGuard = 0;

	trans[1].dest = STATE_S2;
	trans[1].id = TR2;
	trans[1].iTrAction = 4;
	trans[1].iTrGuard = 1;

	trans[2].dest = -CPS1;
	trans[2].id = TR6;
	trans[2].iTrAction = 4;
	trans[2].iTrGuard = 0;

	trans[3].dest = 0;
	trans[3].id = TR5;
	trans[3].iTrAction = 4;
	trans[3].iTrGuard = 1;

	trans[4].dest = STATE_S2;
	trans[4].id = TR4;
	trans[4].iTrAction = 4;
	trans[4].iTrGuard = 1;

	trans[5].dest = STATE_S1;
	trans[5].id = 0;
	trans[5].iTrAction = 4;
	trans[5].iTrGuard = 1;

	trans[6].dest = STATE_S2;
	trans[6].id = 0;
	trans[6].iTrAction = 4;
	trans[6].iTrGuard = 2;

	/* Configure the SM Descriptor */
	smBase.pStates = pStates;
	smBase.cStates = cStates;
	smBase.trans = trans;
	smBase.nOfPStates = 2;
	smBase.nOfCStates = 1;
	smBase.nOfTrans = 7;
	smDesc.smBase = &smBase;
	smDesc.transCnt = 0;
	smDesc.curState = 0;
	smDesc.smData = NULL;
	smDesc.nOfActions = 5;
	smDesc.nOfGuards = 3;
	smDesc.smActions = actions;
	smDesc.smGuards = guards;
	smDesc.esmDesc = esmDesc;
	smDesc.smExecCnt = 0;
	smDesc.stateExecCnt = 0;
	smDesc.errCode = smSuccess;
	smDesc.smData = smData;

	return &smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM6(struct TestSmData* smData, struct TestSmData* esmData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 6;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 1;	/* number of guards */
	FwSmDesc_t p_sm;
	FwSmDesc_t p_esm;

	/* Create and configure the embedded state machine */
	p_esm = FwSmMakeTestSM5(esmData);

	/* Create the embedding state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddState(p_sm, STATE_S2, 4, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, p_esm);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToSta(p_sm, FW_TR_EXECUTE, STATE_S1, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, FW_TR_EXECUTE, STATE_S2, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, TR6, STATE_S2, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToFps(p_sm, TR3, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransStaToSta(p_sm, TR4, STATE_S2, STATE_S2, &incrCnt2By1, &retFlag1);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM7(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 3;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, NULL, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToCps(p_sm, TR1, STATE_S1, CPS1, &incrCnt2By1, NULL);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag2);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM8(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 0;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 1;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToCps(p_sm, CPS1, &incrCnt2By1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag2);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM9(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 1;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 0, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM9Static(struct TestSmData* smData) {

	/* Create the state machine */
	FW_SM_INST_NOCPS(smDesc, 1, 1, 4, 0)
	FwSmInit(&smDesc);

	/* Configure the state machine */
	FwSmSetData(&smDesc, smData);
	FwSmAddState(&smDesc, STATE_S1, 0, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddTransIpsToSta(&smDesc, STATE_S1, &incrCnt2By1);
	return &smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM10(struct TestSmData* smData, struct TestSmData* esmData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 3;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;
	FwSmDesc_t p_esm;

	/* Create SM to be embedded in state S1 */
	p_esm = FwSmMakeTestSM1(esmData);
	if (p_esm==NULL)
		return NULL;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 0, &incrCnt1By1, &incrCnt1By4, NULL, p_esm);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, &incrCnt2By1);
	FwSmAddTransStaToCps(p_sm, TR1, STATE_S1, CPS1, &incrCnt2By1, NULL);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag2);
	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM11(struct TestSmData* smData) {

	static SmTrans_t trans[1];
	static FwSmAction_t actions[2];
	static FwSmGuard_t guards[1];

	actions[0] = &SmDummyAction;
	actions[1] = &incrCnt2By1;

	guards[0] = &SmDummyGuard;

	trans[0].dest = 0;
	trans[0].id = 0;
	trans[0].iTrAction = 1;
	trans[0].iTrGuard = 0;

	static struct FwSmDesc smDesc;
	static SmBaseDesc_t smBase;

	smBase.pStates = NULL;
	smBase.cStates = NULL;
	smBase.trans = trans;
	smBase.nOfPStates = 0;
	smBase.nOfCStates = 0;
	smBase.nOfTrans = 1;
	smDesc.transCnt = 0;
	smDesc.smBase = &smBase;
	smDesc.curState = 0;
	smDesc.smData = NULL;
	smDesc.nOfActions = 2;
	smDesc.nOfGuards = 1;
	smDesc.smActions = actions;
	smDesc.smGuards = guards;
	smDesc.esmDesc = NULL;
	smDesc.errCode = smSuccess;
	smDesc.smData = smData;

	return &smDesc;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM12(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 4;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToCps(p_sm, CPS1, &incrCnt2By1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, &incrCnt2By1, &retFlag2);
	FwSmAddTransStaToFps(p_sm, TR_S1_FPS, STATE_S1, &incrCnt2By1, &retFlag2);

	/* Modify first transition out of CPS1 to terminate in CPS1 */
	p_sm->smBase->trans[2].dest = -CPS1;

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM13(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 5;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 3;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 2, &incrCnt1By1, &incrCnt1By4, &incrCnt1By2, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, NULL);
	FwSmAddTransStaToCps(p_sm, FW_TR_EXECUTE, STATE_S1, CPS1, NULL, &retFlag1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S1, NULL, &retFlag1);
	FwSmAddTransCpsToFps(p_sm, CPS1, NULL, &retFlag2);
	FwSmAddTransStaToFps(p_sm, FW_TR_EXECUTE, STATE_S1, NULL, &retFlag2);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM14(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 2;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 6;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 1;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 2;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddChoicePseudoState(p_sm, CPS2, 1);
	FwSmAddTransIpsToCps(p_sm, CPS1, &incrCnt2By1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S1, &incrCnt2By1, &retFlag1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, &incrCnt2By1, &retFlag2);
	FwSmAddTransStaToCps(p_sm, TR1, STATE_S1, CPS2, &incrCnt2By1, NULL);
	FwSmAddTransStaToCps(p_sm, TR2, STATE_S2, CPS2, &incrCnt2By1, NULL);
	FwSmAddTransCpsToFps(p_sm, CPS2, &incrCnt2By1, NULL);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM15(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 5;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 1;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 4;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 1, &assignExecCnt, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 1, &assignExecCnt, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 2);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, NULL);
	FwSmAddTransStaToCps(p_sm, FW_TR_EXECUTE, STATE_S1, CPS1, NULL, &wait1Cycle);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, NULL, &smExecCntSmallerThan4);
	FwSmAddTransCpsToFps(p_sm, CPS1, NULL, &smExecCntEqual4);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S2, STATE_S1, NULL, &wait2Cycle);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSMDer1(FwSmDesc_t smDescBase, struct TestSmData* smData) {
	FwSmDesc_t p_sm;

	/* Create and configure the derived state machine */
	p_sm = FwSmCreateDer(smDescBase);
	FwSmOverrideAction(p_sm, &incrCnt1By1, &incrCnt1By8);
	FwSmOverrideGuard(p_sm, &retFlag1, &retFlag2);
	FwSmSetData(p_sm, smData);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSMDer1Static(FwSmDesc_t smDescBase, struct TestSmData* smData) {

	/* Instantiate the derived state machine. The number of states, of actions and of guards
	 * of the derived state machine must be the same as number of states, of actions, and of
	 * guards of the base state machine.
	 */
	FW_SM_INST_DER(smDescDer, 1, 3, 1)
	FwSmDesc_t p_sm = &smDescDer;

	/* Initialize the derived state machine */
	FwSmInitDer(p_sm, smDescBase);

	/* Configure the derived state machine */
	FwSmOverrideAction(p_sm, &incrCnt1By1, &incrCnt1By8);
	FwSmOverrideGuard(p_sm, &retFlag1, &retFlag2);
	FwSmSetData(p_sm, smData);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM16_1(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 4;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 8;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 3;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 4, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S3, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S4, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 3);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, NULL);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S2, NULL, &retFlag1);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S4, NULL, &retFlag3);
	FwSmAddTransStaToCps(p_sm, TR2, STATE_S1, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, NULL, &retFlag1);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S4, NULL, &retFlag3);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM16_2(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 4;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 8;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 3;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 4, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S3, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S4, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 3);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, NULL);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S4, NULL, &retFlag3);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S2, NULL, &retFlag1);
	FwSmAddTransStaToCps(p_sm, TR2, STATE_S1, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S4, NULL, &retFlag3);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, NULL, &retFlag1);

	return p_sm;
}

/*-------------------------------------------------------------------------------------------------*/
FwSmDesc_t FwSmMakeTestSM16_3(struct TestSmData* smData) {
	const FwSmCounterS1_t nOfPStates = 4;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 8;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 3;	/* number of guards */
	FwSmDesc_t p_sm;

	/* Create the state machine */
	p_sm = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);

	/* Configure the state machine */
	FwSmSetData(p_sm, smData);
	FwSmAddState(p_sm, STATE_S1, 4, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S2, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S3, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(p_sm, STATE_S4, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(p_sm, CPS1, 3);
	FwSmAddTransIpsToSta(p_sm, STATE_S1, NULL);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S4, NULL, &retFlag3);
	FwSmAddTransStaToSta(p_sm, TR1, STATE_S1, STATE_S2, NULL, &retFlag1);
	FwSmAddTransStaToCps(p_sm, TR2, STATE_S1, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S3, NULL, &retFlag2);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S4, NULL, &retFlag3);
	FwSmAddTransCpsToSta(p_sm, CPS1, STATE_S2, NULL, &retFlag1);

	return p_sm;
}
