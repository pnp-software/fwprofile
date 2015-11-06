/**
 * @file
 * @ingroup tsSmGroup
 * Implementation of test cases for FW State Machine Module.
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

#include <stdio.h>
#include <stdlib.h>
#include "FwProfile/FwSmConfig.h"
#include "FwProfile/FwSmSCreate.h"
#include "FwProfile/FwSmDCreate.h"
#include "FwProfile/FwSmAux.h"
#include "FwProfile/FwSmPrivate.h"
#include "FwSmTestCases.h"
#include "FwSmMakeTest.h"

/**
 * Log array where state machine operations write their marker.
 */
extern int fwSm_logMarker[LOG_ARRAY_SIZE];
/**
 * Log array where SM operations write the state of their SM.
 */
extern int fwSm_logState[LOG_ARRAY_SIZE];
/**
 * Index for write operations in log arrays.
 */
extern int fwSm_logIndex;

/**
 * Check whether two state machines are structural clones of each other.
 * State machine B is a structural clone of state machine A if the following
 * conditions are satisfied:
 * - A has the same states with the same actions as B;
 * - A has the same choice pseudo-states as B;
 * - A has the same transitions between the same states or choice pseudo-states
 *   and with the same actions and guards as B;
 * - if state S1 of B has an embedded state machine B1, then state S1 of A has
 *   an embedded state machine which is a structural clone of B1.
 * .
 * This function checks that the two state machines have the same structure
 * by checking that they share the same base descriptor (this guarantees that
 * they have the same states and choice pseudo-states connected by the same
 * transitions).
 * @param smDesc1 first state machine
 * @param smDesc2 second state machine
 * @return 1 if the two state machines are structural clones, zero otherwise
 */
static int IsClone(FwSmDesc_t smDesc1, FwSmDesc_t smDesc2);

static int IsClone(FwSmDesc_t smDesc1, FwSmDesc_t smDesc2) {
	FwSmCounterS1_t i;

	/* Check that the two SMs share the same base descriptor */
	if (smDesc1->smBase != smDesc2->smBase)
		return 0;

	/* check that the two SMs have the same actions */
	if (smDesc1->nOfActions != smDesc2->nOfActions) {
		return 0;
	}
	for (i=0; i<smDesc1->nOfActions; i++)
		if (smDesc1->smActions[i] != smDesc2->smActions[i]) {
			return 0;
		}

	/* check that the two SMs have the same guards */
	if (smDesc1->nOfGuards != smDesc2->nOfGuards) {
		return 0;
	}
	for (i=0; i<smDesc1->nOfGuards; i++)
		if (smDesc1->smGuards[i] != smDesc2->smGuards[i]) {
			return 0;
		}

	/* Check that the ESMs of the second SM are structural
	 * clones of the ESMs of the first SM */
	for (i=0; i<smDesc1->smBase->nOfPStates; i++)
		if (smDesc1->esmDesc[i] != NULL)
			if (!IsClone(smDesc1->esmDesc[i], smDesc2->esmDesc[i]))
					return 0;
	for (i=0; i<smDesc2->smBase->nOfPStates; i++)
		if (smDesc2->esmDesc[i] != NULL)
			if (!IsClone(smDesc2->esmDesc[i], smDesc1->esmDesc[i]))
					return 0;
	return 1;
}


/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStart1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create first test SM */
	smDesc = FwSmMakeTestSM1(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check that state machine is not started */
	if (FwSmIsStarted(smDesc) != 0) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check that no ESM is returned because the SM is stopped */
	if (FwSmGetEmbSmCur(smDesc) != NULL) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)){
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try re-starting the SM and check that nothing happens */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)){
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check that state machine is started */
	if (FwSmIsStarted(smDesc) != 1) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStart2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc2;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;	/* this forces the selection of the transition from IPS to FPS */
	smData->logBase = 0;

	/* Create test SM */
	smDesc2 = FwSmMakeTestSM2(smData);
	if (smDesc2==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc2) != smSuccess) {
		FwSmRelease(smDesc2);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc2);
	if ((smData->counter_1!=0) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc2)!=0)) {
		FwSmRelease(smDesc2);
		return smTestCaseFailure;
	}

	/* Modify flags to force transition into S1 upon start */
	smData->flag_1 = 1;
	smData->flag_2 = 0;

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc2);
	if ((smData->counter_1!=1) || (smData->counter_2!=4) || (FwSmGetCurState(smDesc2)!=STATE_S1)) {
		FwSmRelease(smDesc2);
		return smTestCaseFailure;
	}

	/* Verify the function to get the state of an embedded state machine in the case
	 * where a state has no embedded state machine */
	if (FwSmGetCurStateEmb(smDesc2)!=-1) {
			FwSmRelease(smDesc2);
			return smTestCaseFailure;
		}

	FwSmRelease(smDesc2);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStart3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc3;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 0;
	esmData->flag_2 = 1;	/* this forces the selection of the transition from IPS to FPS */
	esmData->logBase = 0;

	/* Create test SM */
	smDesc3 = FwSmMakeTestSM3(smData, esmData);
	if (smDesc3==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc3) != smSuccess) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Check the function to retrieve the current sub-state */
	if (FwSmGetCurStateEmb(smDesc3) != -1) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=0) || (esmData->counter_2!=2) || (FwSmGetCurStateEmb(smDesc3)!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc3);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStop1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM1(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check that stopping a SM which is already stopped has no effect */
	FwSmStop(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Stop SM and check success */
	FwSmStop(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStop2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc3;
	FwSmDesc_t smDesc2;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 1;	/* this forces the selection of the transition from IPS to S1 */
	esmData->flag_2 = 0;
	esmData->logBase = 0;

	/* Create test SM */
	smDesc3 = FwSmMakeTestSM3(smData, esmData);
	if (smDesc3==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc3) != smSuccess) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=1) || (esmData->counter_2!=2) || (FwSmGetCurStateEmb(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Stop SM and check outcome of stop operation */
	smDesc2 = FwSmGetEmbSmCur(smDesc3);	/* get the SM embedded in the current state of smDesc3 */
	FwSmStop(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=5) || (esmData->counter_2!=2) || (FwSmGetCurState(smDesc2)!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc3);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseStop3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc3;
	FwSmDesc_t smDesc2;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 0;
	esmData->flag_2 = 1;	/* this forces the selection of the transition from IPS to FPS */
	esmData->logBase = 0;

	/* Create test SM */
	smDesc3 = FwSmMakeTestSM3(smData, esmData);
	if (smDesc3==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc3) != smSuccess) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=0) || (esmData->counter_2!=2) || (FwSmGetCurState(FwSmGetEmbSmCur(smDesc3))!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Stop SM and check outcome of stop operation */
	smDesc2 = FwSmGetEmbSmCur(smDesc3);	/* get the SM embedded in the current state of smDesc3 */
	FwSmStop(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=0) || (esmData->counter_2!=2) || (FwSmGetCurState(smDesc2)!=0)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc3);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecute1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM1(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt executing a stopped state machine and check that nothing happens */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=0) ||
			(smData->logBase!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Execute state machine twice and check success */
	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=3) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=5) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecute2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc3;
	FwSmDesc_t smDesc2;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 1;	/* this forces the selection of the transition from IPS to S1 */
	esmData->flag_2 = 0;
	esmData->logBase = 0;

	/* Create test SM */
	smDesc3 = FwSmMakeTestSM3(smData, esmData);
	if (smDesc3==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc3) != smSuccess) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc3);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=1) || (esmData->counter_2!=2) || (FwSmGetCurState(FwSmGetEmbSmCur(smDesc3))!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	/* Execute SM twice and check outcome of Execute operation */
	smDesc2 = FwSmGetEmbSmCur(smDesc3);	/* get the SM embedded in the current state of smDesc3 */
	FwSmMakeTrans(smDesc3, FW_TR_EXECUTE);
	if ((smData->counter_1!=3) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=3) || (esmData->counter_2!=2) || (FwSmGetCurState(smDesc2)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDesc3, FW_TR_EXECUTE);
	if ((smData->counter_1!=5) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc3)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	if ((esmData->counter_1!=5) || (esmData->counter_2!=2) || (FwSmGetCurState(smDesc2)!=STATE_S1)) {
		FwSmReleaseRec(smDesc3);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc3);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecute3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc4;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* Allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc4 = FwSmMakeTestSM4(smData);
	if (smDesc4==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc4) != smSuccess) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc4)!=STATE_S1)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	/* Execute state machine and check success */
	FwSmMakeTrans(smDesc4, FW_TR_EXECUTE);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc4)!=STATE_S2)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDesc4, FW_TR_EXECUTE);
	if ((smData->counter_1!=15) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc4)!=STATE_S1)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc4);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseSelfTrans1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc4;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* Allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc4 = FwSmMakeTestSM4(smData);
	if (smDesc4==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc4) != smSuccess) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc4)!=STATE_S1)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	/* Execute state machine to bring it to state S2 */
	FwSmMakeTrans(smDesc4, FW_TR_EXECUTE);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc4)!=STATE_S2)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	/* Execute self-transition */
	fwSm_logIndex = 0;	/* reset log */
	FwSmMakeTrans(smDesc4, TR4);
	if ((smData->counter_1!=13) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc4)!=STATE_S2)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		FwSmRelease(smDesc4);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc4);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc5;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc5 = FwSmMakeTestSM5(smData);
	if (smDesc5==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc5) != smSuccess) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc5);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Send transition command TR4 to SM and check that this has no effect */
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Set guard to prevent transition TR2 from S1 to S2 and then check that transition does not take place */
	smData->flag_1 = 0;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Set guard to allow transition TR2 from S1 to S2 and then check that transition does take place */
	smData->flag_1 = 1;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=3)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc5);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc5;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc5 = FwSmMakeTestSM5(smData);
	if (smDesc5==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc5) != smSuccess) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc5);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Send transition command TR2 to SM (this brings it to state S2) */
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc5)!=STATE_S2)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Set guards to open transition from S2 to S1 and then perform transition */
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR6);
	if ((smData->counter_1!=11) || (smData->counter_2!=4) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=4)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=4) || (fwSm_logMarker[3]!=1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=2) || (fwSm_logState[1]!=2) || (fwSm_logState[2]!=2) || (fwSm_logState[3]!=1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Send transition command TR2 to SM (this brings it to state S2) */
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=16) || (smData->counter_2!=5) || (FwSmGetCurState(smDesc5)!=STATE_S2)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	/* Set guards to open transition from S2 back to S2 and then perform transition */
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR6);
	if ((smData->counter_1!=21) || (smData->counter_2!=7) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=4)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=4) || (fwSm_logMarker[3]!=1)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=2) || (fwSm_logState[1]!=2) || (fwSm_logState[2]!=2) || (fwSm_logState[3]!=2)) {
		FwSmRelease(smDesc5);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc5);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc6;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* this allows all transitions in embedding SM */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 1;	/* this forces the selection of the transition from S2 to S1 in ESM */
	esmData->flag_2 = 0;
	esmData->logBase = 10;

	/* Create test SM */
	smDesc6 = FwSmMakeTestSM6(smData, esmData);
	if (smDesc6==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc6) != smSuccess) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc6);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc6)!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=0) || (esmData->counter_2!=0) || (FwSmGetCurState(FwSmGetEmbSm(smDesc6, STATE_S2))!=0)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=4) || (fwSm_logMarker[1]!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=0) || (fwSm_logState[1]!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Execute SM (this causes a transition from S1 to S2 and the starting of the ESM) */
	FwSmMakeTrans(smDesc6, FW_TR_EXECUTE);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[2]!=2) || (fwSm_logMarker[3]!=3) || (fwSm_logMarker[4]!=4) || (fwSm_logMarker[5]!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[2]!=1) || (fwSm_logState[3]!=1) || (fwSm_logState[4]!=1) || (fwSm_logState[5]!=2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[6]!=14) || (fwSm_logMarker[7]!=11)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[6]!=10) || (fwSm_logState[7]!=11)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Send TR5 command to SM (this has no effect on either the SM or its ESM) */
	FwSmMakeTrans(smDesc6, TR5);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=8) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Send TR2 command to SM (this has no effect on the SM but causes the ESM to go from S1 to S2) */
	FwSmMakeTrans(smDesc6, TR2);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=6) || (esmData->counter_2!=2) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=11) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[8]!=13) || (fwSm_logMarker[9]!=14) || (fwSm_logMarker[10]!=11)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[8]!=11) || (fwSm_logState[9]!=11) || (fwSm_logState[10]!=12)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc6);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecute4() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc6;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* this allows all transitions in embedding SM */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 1;	/* this forces the selection of the transition from S2 to S1 in ESM */
	esmData->flag_2 = 0;
	esmData->logBase = 10;

	/* Create test SM */
	smDesc6 = FwSmMakeTestSM6(smData, esmData);
	if (smDesc6==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc6) != smSuccess) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Start and execute SM (this brings the embedding SM in S2 and starts the ESM) */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc6);
	FwSmExecute(smDesc6);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Send TR2 command to SM (this has no effect on the SM but causes the ESM to go from S1 to S2) */
	FwSmMakeTrans(smDesc6, TR2);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=6) || (esmData->counter_2!=2) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	fwSm_logIndex = 0;

	/* Execute the SM (this causes the do-action of the S2 of both the SM and ESM to be executed and then triggers */
	/* a transition from S2 to S1 in the SM) */
	FwSmExecute(smDesc6);
	if ((smData->counter_1!=15) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=12) || (esmData->counter_2!=2) || (FwSmGetCurState(FwSmGetEmbSm(smDesc6, STATE_S2))!=0)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=6) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=2) || (fwSm_logMarker[1]!=12)|| (fwSm_logMarker[2]!=13) || (fwSm_logMarker[3]!=3) ||
			(fwSm_logMarker[4]!=4) || (fwSm_logMarker[5]!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=2) || (fwSm_logState[1]!=12)|| (fwSm_logState[2]!=12) ||
			(fwSm_logState[3]!=2) || (fwSm_logState[4]!=2) || (fwSm_logState[5]!=1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc6);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans4() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc6;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* this allows all transitions in embedding SM */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 1;	/* this forces the selection of the transition from S2 to S1 in ESM */
	esmData->flag_2 = 0;
	esmData->logBase = 10;

	/* Create test SM */
	smDesc6 = FwSmMakeTestSM6(smData, esmData);
	if (smDesc6==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc6) != smSuccess) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Start and execute SM (this brings the embedding SM in S2 and starts the ESM) */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc6);
	FwSmExecute(smDesc6);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Send TR2 command to SM (this has no effect on the SM but causes the ESM to go from S1 to S2) */
	FwSmMakeTrans(smDesc6, TR2);
	if ((smData->counter_1!=8) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=6) || (esmData->counter_2!=2) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	/* Send TR4 command to SM (this causes a self-transition on both the SM and the ESM) */
	fwSm_logIndex = 0;	/* reset log counter */
	FwSmMakeTrans(smDesc6, TR4);
	if ((smData->counter_1!=13) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc6)!=STATE_S2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=16) || (esmData->counter_2!=4) || (FwSmGetCurStateEmb(smDesc6)!=STATE_S1)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=9) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=13) || (fwSm_logMarker[1]!=14) || (fwSm_logMarker[2]!=11) || (fwSm_logMarker[3]!=13) ||
			(fwSm_logMarker[4]!=3)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[5]!=4) || (fwSm_logMarker[6]!=1) || (fwSm_logMarker[7]!=14) || (fwSm_logMarker[8]!=11)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=12) || (fwSm_logState[1]!=12) || (fwSm_logState[2]!=12)|| (fwSm_logState[3]!=12) ||
			(fwSm_logState[4]!=2)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[5]!=2) || (fwSm_logState[6]!=2) || (fwSm_logState[7]!=10) || (fwSm_logState[8]!=11)) {
		FwSmReleaseRec(smDesc6);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc6);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDescDir1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc5;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc5 = FwSmMakeTestSM5Dir(smData);

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc5) != smSuccess) {
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc5);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1)) {
		return smTestCaseFailure;
	}

	/* Send transition command TR4 to SM and check that this has no effect */
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		return smTestCaseFailure;
	}

	/* Set guard to prevent transition TR2 from S1 to S2 and then check that transition does not take place */
	smData->flag_1 = 0;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition TR2 from S1 to S2 and then check that transition does take place */
	smData->flag_1 = 1;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=3)) {
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition from CPS1 to S2 and then trigger transition from S2 back to S2 via CPS1 */
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR6);
	if ((smData->counter_1!=11) || (smData->counter_2!=4) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=4)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition from S2 to FPS and then trigger transition */
	smData->flag_1 = 1;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR5);
	if ((smData->counter_1!=15) || (smData->counter_2!=5) || (FwSmGetCurState(smDesc5)!=0) || (fwSm_logIndex!=2)) {
		return smTestCaseFailure;
	}

	return smTestCaseSuccess;
}


/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDescStatic2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc5;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;	/* allow transition to take place */
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc5 = FwSmMakeTestSM5Static(smData);

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc5) != smSuccess) {
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc5);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1)) {
		return smTestCaseFailure;
	}

	/* Send transition command TR4 to SM and check that this has no effect */
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		return smTestCaseFailure;
	}

	/* Set guard to prevent transition TR2 from S1 to S2 and then check that transition does not take place */
	smData->flag_1 = 0;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc5)!=STATE_S1) || (fwSm_logIndex!=0)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition TR2 from S1 to S2 and then check that transition does take place */
	smData->flag_1 = 1;
	FwSmMakeTrans(smDesc5, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=3)) {
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition from CPS1 to S2 and then trigger transition from S2 back to S2 via CPS1 */
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR6);
	if ((smData->counter_1!=11) || (smData->counter_2!=4) || (FwSmGetCurState(smDesc5)!=STATE_S2) || (fwSm_logIndex!=4)) {
		return smTestCaseFailure;
	}

	/* Set guard to allow transition from S2 to FPS and then trigger transition */
	smData->flag_1 = 1;
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDesc5, TR5);
	if ((smData->counter_1!=15) || (smData->counter_2!=5) || (FwSmGetCurState(smDesc5)!=0) || (fwSm_logIndex!=2)) {
		return smTestCaseFailure;
	}

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseConfigErr1() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Attempt to add a state with a negative number of out-going transitions */
	FwSmAddState(smDesc, STATE_S1, -1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smNegOutTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add the same state twice and check that an error is returned */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smStateIdInUse) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add a state with an illegal identifier */
	FwSmAddState(smDesc, 0, 1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllStateId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddState(smDesc, 2, 1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllStateId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add the same choice pseudo-state twice and check that an error is returned */
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	if (FwSmGetErrCode(smDesc) != smChoiceIdInUse) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add a choice pseudo-state with an illegal identifier */
	FwSmAddChoicePseudoState(smDesc, 0, 2);
	if (FwSmGetErrCode(smDesc) != smIllChoiceId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddChoicePseudoState(smDesc, 2, 2);
	if (FwSmGetErrCode(smDesc) != smIllChoiceId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add too many transitions and check that an error is returned */
	FwSmAddTransStaToSta(smDesc, TR1, STATE_S1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllChoiceId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddTransStaToSta(smDesc, TR1, STATE_S1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add the initial transition twice and check that an error is returned */
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck1() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine without defining the state */
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullPState) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck2() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and two transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine without defining the CPS */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullCState) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck3() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine without defining one of the transitions */
	FwSmAddState(smDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck4() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine without defining the initial transition */
	FwSmAddState(smDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck5() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, no CPS, and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine with an extra transition out of S1 */
	FwSmAddState(smDesc, STATE_S1, 2, NULL, NULL, NULL, NULL);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck6() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 5;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and five transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine with an extra transition out of CPS */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 3);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smNullTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck7() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine - state S1 has no out-going transitions */
	FwSmAddState(smDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllTransSrc) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck8() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 5;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and five transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine - CPS1 has only two out-going transitions */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);  /* Two identical transitions from CPS1 to FPS */
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smConfigErr) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck9() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;
	FwSmDesc_t esmDesc;

	/* Create a state machine with a missing transition */
	esmDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(esmDesc, NULL);

	FwSmAddState(esmDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddTransIpsToSta(esmDesc, STATE_S1, NULL);

	/* Create a state machine embedding the previous state machine */
	smDesc = FwSmCreate(1, 0, 2, 0, 0);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, esmDesc);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check outer state machine and verify that no error is reported */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check outer state machine recursively and verify that error is reported */
	if (FwSmCheckRec(smDesc) != smNullTrans) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck10() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine - one transition has an illegal destination */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, 3, NULL, NULL);  /* illegal destination */
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smIllegalPDest) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans5() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;  /* this allows transition from CPS to FPS */
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM7(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to S1 */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=1) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Send TR1 command to SM (this brings it to the FPS via CPS1) */
	FwSmMakeTrans(smDesc, TR1);
	if ((smData->counter_1!=4) || (smData->counter_2!=3) || (FwSmIsStarted(smDesc)!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=4) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDummySM1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM8(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to the FPS */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_2!=2) || (FwSmIsStarted(smDesc)!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=2) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDummySM9() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM9(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to S1 */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=2) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Execute SM */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=3) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=3) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Stop SM */
	FwSmStop(smDesc);
	if ((smData->counter_1!=7) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=4) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDummySM9Static() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM9Static(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to S1 */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=2) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Execute SM */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=3) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=3) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Stop SM */
	FwSmStop(smDesc);
	if ((smData->counter_1!=7) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=4) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck11() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Attempt to add a CPS with zero out-going transitions */
	FwSmAddChoicePseudoState(smDesc, CPS1, 0);
	if (FwSmGetErrCode(smDesc) != smIllNOfOutTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add a transition without first adding its source state */
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smUndefinedTransSrc) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Attempt to add a transition without first adding its source state */
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smUndefinedTransSrc) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck12() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine - one transition has an illegal destination */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToCps(smDesc, TR1, STATE_S1, 3, NULL, NULL);  /* illegal destination */
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) != smIllegalCDest) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans6() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Initialize data structure to hold the data of the embedded state machine */
	esmData->counter_1 = 0;
	esmData->counter_2 = 0;
	esmData->flag_1 = 0;
	esmData->flag_2 = 0;
	esmData->logBase = 10;

	/* Create test SM */
	smDesc = FwSmMakeTestSM10(smData, esmData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheckRec(smDesc) != smSuccess) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Start and execute SM (this brings the embedding SM in S1 - the execute command has no effect) */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	FwSmExecute(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=0) || (esmData->counter_2!=0) || (FwSmGetCurState(FwSmGetEmbSm(smDesc, STATE_S2))!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Send TR1 command to SM (this brings the embedded SM to S2 and starts the embedded SM) */
	FwSmMakeTrans(smDesc, TR1);
	if ((smData->counter_1!=6) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurStateEmb(smDesc)!=STATE_S1)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	/* Stop the embedded SM (this also stops the embedded SM) */
	FwSmStop(smDesc);
	if ((smData->counter_1!=10) || (smData->counter_2!=3) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}
	if ((esmData->counter_1!=1) || (esmData->counter_2!=1) || (FwSmGetCurState(FwSmGetEmbSm(smDesc, STATE_S2))!=0)) {
		FwSmReleaseRec(smDesc);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck14() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 4;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one CPS, and four transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state machine  */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToCps(smDesc, TR1, 2, CPS1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllTransSrc) {	/* illegal source */
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddTransStaToSta(smDesc, TR1, 2, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllTransSrc) {	/* illegal source */
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	FwSmAddTransCpsToFps(smDesc, 2, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllTransSrc) {	/* illegal source */
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check state machine and verify that an error is reported */
	if (FwSmCheck(smDesc) == smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck15() {
	FwSmDesc_t smDesc;

	/* Create a state machine with an illegal number of states */
	smDesc = FwSmCreate(-1, 1, 1, 0, 0);
	if (smDesc != NULL)
		return smTestCaseFailure;

	/* Create a state machine with an illegal number of transitions */
	smDesc = FwSmCreate(1, 1, 0, 0, 0);
	if (smDesc != NULL)
		return smTestCaseFailure;

	/* Create a state machine with an illegal number of choice pseudo-states */
	smDesc = FwSmCreate(1, -1, 1, 0, 0);
	if (smDesc != NULL)
		return smTestCaseFailure;

	/* Create a state machine with an illegal number of actions */
	smDesc = FwSmCreate(1, 1, 1, -1, 0);
	if (smDesc != NULL)
		return smTestCaseFailure;

	/* Create a state machine with an illegal number of guards */
	smDesc = FwSmCreate(1, 1, 1, 0, -1);
	if (smDesc != NULL)
		return smTestCaseFailure;

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTransErr1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM2(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this leaves it in an undefined state because both flag_1 and flag_2 are false */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((fwSm_logIndex!=1) || (FwSmGetErrCode(smDesc)!=smTransErr)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDummySM3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM11(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this causes it to terminate immediately) */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		return smTestCaseFailure;
	}

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTransErr2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure to hold the data of the embedding state machine */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM12(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine (state S1 is unreachable) */
	if (FwSmCheck(smDesc) != smUnreachablePState) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM and verifies that transition encounters an error */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDesc);
	if (FwSmGetErrCode(smDesc) != smTransErr) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck16() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, one choice pseudo-state and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state with too many out-going transitions  */
	FwSmAddState(smDesc, STATE_S1, 3, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyOutTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Configure choice pseudo-state with too many out-going transitions  */
	FwSmAddChoicePseudoState(smDesc, CPS1, 3);
	if (FwSmGetErrCode(smDesc) != smTooManyOutTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck17() {
	const FwSmCounterS1_t nOfPStates = 3;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 1;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with three state and one transition */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state with an entry action (not allowed because no actions have been declared */
	FwSmAddState(smDesc, STATE_S1, 0, &SmDummyAction, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyActions) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Configure state with an exit action (not allowed because no actions have been declared */
	FwSmAddState(smDesc, STATE_S2, 0, NULL, &SmDummyAction, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyActions) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Configure state with a do-action (not allowed because no actions have been declared */
	FwSmAddState(smDesc, STATE_S3, 0, NULL, NULL, &SmDummyAction, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyActions) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck18() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state and three transitions */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state */
	FwSmAddState(smDesc, STATE_S1, 2, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Configure transition with an action (not allowed because no actions have been declared */
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, &SmDummyAction, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyActions) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Configure transition with a guard (not allowed because no guards have been declared */
	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, &SmDummyGuard);
	if (FwSmGetErrCode(smDesc) != smTooManyGuards) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck19() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 1;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, two transitions, and one action */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	if (FwSmCheck(smDesc) != smTooFewActions) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck20() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 2;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 1;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create a state machine with one state, two transitions, and one guard */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	/* Configure state */
	FwSmAddState(smDesc, STATE_S1, 1, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmAddTransStaToFps(smDesc, TR1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	if (FwSmCheck(smDesc) != smTooFewGuards) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck21() {
	const FwSmCounterS1_t nOfPStates = 2;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 0;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 1;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create and configure the test state machine */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	FwSmAddState(smDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddState(smDesc, STATE_S2, 0, NULL, NULL, NULL, NULL);

	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);

	/* Check that configuration error is detected */
	if (FwSmCheck(smDesc) != smUnreachablePState) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseCheck22() {
	const FwSmCounterS1_t nOfPStates = 1;	/* number of proper states */
	const FwSmCounterS1_t nOfCStates = 1;	/* number of choice pseudo-states */
	const FwSmCounterS1_t nOfTrans = 3;		/* number of transitions */
	const FwSmCounterS1_t nOfActions = 0;	/* number of actions */
	const FwSmCounterS1_t nOfGuards = 0;	/* number of guards */
	FwSmDesc_t smDesc;

	/* Create and configure the test state machine */
	smDesc = FwSmCreate(nOfPStates, nOfCStates, nOfTrans, nOfActions, nOfGuards);
	FwSmSetData(smDesc, NULL);

	FwSmAddState(smDesc, STATE_S1, 0, NULL, NULL, NULL, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);

	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, NULL);

	/* Check that configuration error is detected */
	if (FwSmCheck(smDesc) != smUnreachableCState) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDescStatic3() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM1Static(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		return smTestCaseFailure;
	}

	/* Attempt executing a stopped state machine and check that nothing happens */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=0) ||
			(smData->logBase!=0)) {
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		return smTestCaseFailure;
	}

	/* Execute state machine twice and check success */
	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=3) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=5) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		return smTestCaseFailure;
	}

	/* Stop state machine twice and check success */
	FwSmStop(smDesc);
	if ((smData->counter_1!=5) || (smData->counter_2!=1) || (FwSmGetCurState(smDesc)!=0)) {
		return smTestCaseFailure;
	}

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCasePrint1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	struct TestSmData sEsmData;
	struct TestSmData* esmData = &sEsmData;
	FILE* outfile;
	FwSmDesc_t smDesc;

	outfile = fopen("FwSmPrint.out","w");

	fprintf(outfile,"\n===== TEST STATE MACHINE SM1 =====\n");
	smDesc = FwSmMakeTestSM1(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM1 (Static Initialization) =====\n");
	smDesc = FwSmMakeTestSM1Static(smData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM2 =====\n");
	smDesc = FwSmMakeTestSM2(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM3 =====\n");
	smDesc = FwSmMakeTestSM3(smData, esmData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmReleaseRec(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM4 =====\n");
	smDesc = FwSmMakeTestSM4(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM5 =====\n");
	smDesc = FwSmMakeTestSM5(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM5 (Static Initialization) =====\n");
	smDesc = FwSmMakeTestSM5Static(smData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM5 (Direct Initialization) =====\n");
	smDesc = FwSmMakeTestSM5Dir(smData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM6 =====\n");
	smDesc = FwSmMakeTestSM6(smData, esmData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmReleaseRec(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM7 =====\n");
	smDesc = FwSmMakeTestSM7(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM8 =====\n");
	smDesc = FwSmMakeTestSM8(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM9 =====\n");
	smDesc = FwSmMakeTestSM9(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM9 (Static Initialization) =====\n");
	smDesc = FwSmMakeTestSM9Static(smData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM10 =====\n");
	smDesc = FwSmMakeTestSM10(smData, esmData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM10 (After it has been started) =====\n");
	FwSmStart(smDesc);
	FwSmPrintConfig(smDesc, outfile);
	FwSmReleaseRec(smDesc);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM11 (Static Initialization) =====\n");
	smDesc = FwSmMakeTestSM11(smData);
	FwSmPrintConfig(smDesc, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM13 =====\n");
	smDesc = FwSmMakeTestSM13(smData);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fclose(outfile);

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCasePrint2() {
	FILE* outfile;
	FwSmDesc_t smDesc;

	outfile = fopen("FwSmPrint.out","a");

	fprintf(outfile,"\n===== FwSmPrint is called with a NULL State Machine Descriptor =====\n");
	FwSmPrintConfig(NULL, outfile);

	fprintf(outfile,"\n===== TEST STATE MACHINE SM12 (this SM has an illegal configuration) =====\n");
	smDesc = FwSmMakeTestSM12(NULL);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== FwSmPrint is called with an unconfigured State Machine Descriptor =====\n");
	smDesc = FwSmCreate(2, 1, 3, 3, 3);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== FwSmPrint is called on a State Machine Descriptor with Undefined State =====\n");
	smDesc = FwSmCreate(1, 0, 2, 0, 0);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== FwSmPrint is called on a State Machine Descriptor with Undefined Choice Pseudo-State =====\n");
	smDesc = FwSmCreate(0, 1, 3, 0, 0);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fprintf(outfile,"\n===== FwSmPrint is called on a State Machine Descriptor with no states =====\n");
	smDesc = FwSmCreate(0, 1, 3, 0, 0);
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	FwSmAddTransIpsToCps(smDesc, CPS1, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmAddTransCpsToFps(smDesc, CPS1, NULL, NULL);
	FwSmPrintConfig(smDesc, outfile);
	FwSmRelease(smDesc);

	fclose(outfile);

	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecute5() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM13(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Execute state machine and check success */
	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=8) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	smData->flag_1 = 0;
	smData->flag_2 = 1;

	FwSmMakeTrans(smDesc, FW_TR_EXECUTE);
	if ((smData->counter_1!=14) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM5 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM5(NULL);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Set the data structure for the derived state machine */
	FwSmSetData(smDescDer, smData);

	/* Check configuration of derived state machine */
	if (FwSmCheckRec(smDescDer) != smSuccess) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDescDer);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Send transition command TR4 to SM and check that this has no effect */
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDescDer, TR4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Set guard to prevent transition TR2 from S1 to S2 and then check that transition does not take place */
	smData->flag_1 = 0;
	FwSmMakeTrans(smDescDer, TR2);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Set guard to allow transition TR2 from S1 to S2 and then check that transition does take place */
	smData->flag_1 = 1;
	FwSmMakeTrans(smDescDer, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDescDer)!=STATE_S2) || (fwSm_logIndex!=3)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	FwSmRelease(smDescBase);
	FwSmReleaseDer(smDescDer);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM1 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM1(smData);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmMakeTestSMDer1(smDescBase, smData);

	/* Check configuration of derived state machine */
	if (FwSmCheck(smDescDer) != smSuccess) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDescDer);
	if ((smData->counter_1!=8) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Execute state machine twice and check success */
	FwSmMakeTrans(smDescDer, FW_TR_EXECUTE);
	if ((smData->counter_1!=10) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDescDer, FW_TR_EXECUTE);
	if ((smData->counter_1!=12) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Command transition from S1 to FPS and check that nothing happens (guard is not true) */
	FwSmMakeTrans(smDescDer, TR_S1_FPS);
	if ((smData->counter_1!=12) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Command transition from S1 to FPS and check that transition is executed (guard is true) */
	smData->flag_2 = 1;
	FwSmMakeTrans(smDescDer, TR_S1_FPS);
	if ((smData->counter_1!=12) || (smData->counter_2!=2) || (FwSmGetCurState(smDescDer)!=0)) {
		FwSmReleaseDer(smDescDer);
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmReleaseDer(smDescDer);
	FwSmRelease(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer3() {
	struct TestSmData sSmDataBase;
	struct TestSmData* smDataBase = &sSmDataBase;
	struct TestSmData sEsmDataBase;
	struct TestSmData* esmDataBase = &sEsmDataBase;
	struct TestSmData sSmDataDer;
	struct TestSmData* smDataDer = &sSmDataDer;
	struct TestSmData sEsmDataDer;
	struct TestSmData* esmDataDer = &sEsmDataDer;
	FwSmDesc_t smDescBase, smDescDer, esmDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM6(smDataBase, esmDataBase);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Start the base state machine */
	FwSmStart(smDescBase);

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Initialize and set the data structure for the derived state machine (Outer SM) */
	smDataDer->counter_1 = 0;
	smDataDer->counter_2 = 0;
	smDataDer->flag_1 = 1;
	smDataDer->flag_2 = 0;
	smDataDer->logBase = 0;
	FwSmSetData(smDescDer, smDataDer);

	/* Get the SM embedded in state S2 of the derived SM */
	esmDescDer = FwSmGetEmbSm(smDescDer,2);

	/* Initialize and set the data structure for the derived state machine (Embedded SM) */
	esmDataDer->counter_1 = 0;
	esmDataDer->counter_2 = 0;
	esmDataDer->flag_1 = 1;
	esmDataDer->flag_2 = 0;
	esmDataDer->logBase = 10;
	FwSmSetData(esmDescDer, esmDataDer);

	/* Check configuration of derived state machine */
	if (FwSmCheckRec(smDescDer) != smSuccess) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Start and execute SM (this brings the embedding SM in S2 and starts the ESM) */
	fwSm_logIndex = 0;	/* reset log */
	FwSmStart(smDescDer);
	FwSmExecute(smDescDer);
	if ((smDataDer->counter_1!=8) || (smDataDer->counter_2!=2) || (FwSmGetCurState(smDescDer)!=STATE_S2)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if ((esmDataDer->counter_1!=1) || (esmDataDer->counter_2!=1) || (FwSmGetCurStateEmb(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Send TR2 command to SM (this has no effect on the SM but causes the ESM to go from S1 to S2) */
	FwSmMakeTrans(smDescDer, TR2);
	if ((smDataDer->counter_1!=8) || (smDataDer->counter_2!=2) || (FwSmGetCurState(smDescDer)!=STATE_S2)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if ((esmDataDer->counter_1!=6) || (esmDataDer->counter_2!=2) || (FwSmGetCurStateEmb(smDescDer)!=STATE_S2)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	fwSm_logIndex = 0;

	/* Execute the SM (this causes the do-action of the S2 of both the SM and ESM to be executed and then triggers */
	/* a transition from S2 to S1 in the SM) */
	FwSmExecute(smDescDer);
	if ((smDataDer->counter_1!=15) || (smDataDer->counter_2!=3) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if ((esmDataDer->counter_1!=12) || (esmDataDer->counter_2!=2) || (FwSmGetCurState(FwSmGetEmbSm(smDescDer, STATE_S2))!=0)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if (fwSm_logIndex!=6) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=2) || (fwSm_logMarker[1]!=12)|| (fwSm_logMarker[2]!=13) || (fwSm_logMarker[3]!=3) ||
			(fwSm_logMarker[4]!=4) || (fwSm_logMarker[5]!=1)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}
	if ((fwSm_logState[0]!=2) || (fwSm_logState[1]!=12)|| (fwSm_logState[2]!=12) ||
			(fwSm_logState[3]!=2) || (fwSm_logState[4]!=2) || (fwSm_logState[5]!=1)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	FwSmReleaseDer(smDescDer);
	FwSmReleaseDer(esmDescDer);
	FwSmReleaseRec(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDerConfigErr1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;
	FwSmAction_t dummyAct1;
	FwSmAction_t dummyAct2;
	FwSmGuard_t dummyGuard1;
	FwSmGuard_t dummyGuard2;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM1 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM1(smData);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheck(smDescBase) != smSuccess) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Attempt to override a non-existent action and check that an error is reported */
	dummyAct1 = (FwSmAction_t)1111;
	dummyAct2 = (FwSmAction_t)2222;
	FwSmOverrideAction(smDescDer, dummyAct1, dummyAct2);
	if (FwSmGetErrCode(smDescDer) != smUndefAction) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Attempt to override a non-existent guard and check that an error is reported */
	dummyGuard1 = (FwSmGuard_t)1111;
	dummyGuard2 = (FwSmGuard_t)2222;
	FwSmOverrideGuard(smDescDer, dummyGuard1, dummyGuard2);
	if (FwSmGetErrCode(smDescDer) != smUndefGuard) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Attempt to override an action of a non-derived SM and check that an error is reported */
	FwSmOverrideAction(smDescBase, dummyAct1, dummyAct2);
	if (FwSmGetErrCode(smDescBase) != smNotDerivedSM) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Attempt to override a guard of a non-derived SM and check that an error is reported */
	FwSmOverrideGuard(smDescBase, dummyGuard1, dummyGuard2);
	if (FwSmGetErrCode(smDescBase) != smNotDerivedSM) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	FwSmRelease(smDescBase);
	FwSmReleaseDer(smDescDer);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDerEmbed1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer, esmDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM1 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM1(smData);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheck(smDescBase) != smSuccess) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Create the embedded state machine */
	esmDesc = FwSmMakeTestSM1(NULL);

	/* Embed the state machine in state S1 */
	FwSmEmbed(smDescDer, STATE_S1, esmDesc);
	if (FwSmGetErrCode(smDescDer) != smSuccess) {
		FwSmRelease(smDescBase);
		FwSmRelease(esmDesc);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Embed the state machine in state S1 a second time */
	FwSmEmbed(smDescDer, STATE_S1, esmDesc);
	if (FwSmGetErrCode(smDescDer) != smEsmDefined) {
		FwSmRelease(smDescBase);
		FwSmRelease(esmDesc);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Attempt to embed the state machine in a base state machine */
	FwSmEmbed(smDescBase, 1, esmDesc);
	if (FwSmGetErrCode(smDescBase) != smNotDerivedSM) {
		FwSmRelease(smDescBase);
		FwSmRelease(esmDesc);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Attempt to embed the state machine in a non-existent state */
	FwSmEmbed(smDescDer, 0, esmDesc);
	if (FwSmGetErrCode(smDescDer) != smIllStateId) {
		FwSmRelease(smDescBase);
		FwSmRelease(esmDesc);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	smDescDer->errCode = smSuccess; 	/* Reset error code */

	FwSmEmbed(smDescDer, 2, esmDesc);
	if (FwSmGetErrCode(smDescDer) != smIllStateId) {
		FwSmRelease(smDescBase);
		FwSmRelease(esmDesc);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	FwSmRelease(esmDesc);
	FwSmRelease(smDescBase);
	FwSmReleaseDer(smDescDer);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseJunction1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM1 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 1;
	smData->logBase = 0;

	/* Create the state machine */
	smDesc = FwSmMakeTestSM14(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S2) */
	FwSmStart(smDesc);
	if ((smData->counter_2!=2) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Send transition command TR1 to state machine (this has no effect) */
	FwSmMakeTrans(smDesc, TR1);
	if ((smData->counter_2!=2) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Send transition command TR2 to state machine (this terminates it) */
	FwSmMakeTrans(smDesc, TR2);
	if ((smData->counter_2!=4) || (FwSmGetCurState(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer4() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* Instantiate the derived state machine */
	FW_SM_INST_DER(sSmDescDer, 2, 4, 2)
	smDescDer = &sSmDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM5 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM5(NULL);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Initialize the derived state machine */
	FwSmInitDer(smDescDer, smDescBase);

	/* Set the data structure for the derived state machine */
	FwSmSetData(smDescDer, smData);

	/* Check configuration of derived state machine */
	if (FwSmCheckRec(smDescDer) != smSuccess) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDescDer);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Send transition command TR4 to SM and check that this has no effect */
	fwSm_logIndex = 0;
	FwSmMakeTrans(smDescDer, TR4);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Set guard to prevent transition TR2 from S1 to S2 and then check that transition does not take place */
	smData->flag_1 = 0;
	FwSmMakeTrans(smDescDer, TR2);
	if ((smData->counter_1!=1) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1) || (fwSm_logIndex!=0)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Set guard to allow transition TR2 from S1 to S2 and then check that transition does take place */
	smData->flag_1 = 1;
	FwSmMakeTrans(smDescDer, TR2);
	if ((smData->counter_1!=6) || (smData->counter_2!=2) || (FwSmGetCurState(smDescDer)!=STATE_S2) || (fwSm_logIndex!=3)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}
	if ((fwSm_logMarker[0]!=3) || (fwSm_logMarker[1]!=4) || (fwSm_logMarker[2]!=1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmRelease(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDerErr1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase;

	/* Instantiate the first derived state machine (this one has the wrong
	 * number of actions - 3 instead of 4)
	 */
	FW_SM_INST_DER(sSmDescDer1, 2, 3, 2)
	FwSmDesc_t smDescDer1 = &sSmDescDer1;

	/* Instantiate the second derived state machine (this one has the wrong
	 * number of guards - 3 instead of 2)
	 */
	FW_SM_INST_DER(sSmDescDer2, 2, 4, 3)
	FwSmDesc_t smDescDer2 = &sSmDescDer2;


	/* Initialize data structures for the base state machine SM5 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM5(NULL);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Try to initialize the first derived state machine and
	 * check that an error is reported
	 */
	FwSmInitDer(smDescDer1, smDescBase);
	if (FwSmGetErrCode(smDescDer1) != smWrongNOfActions) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Try to initialize the second derived state machine and
	 * check that an error is reported
	 */
	FwSmInitDer(smDescDer2, smDescBase);
	if (FwSmGetErrCode(smDescDer2) != smWrongNOfGuards) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmRelease(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer5() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM1 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM1(smData);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmMakeTestSMDer1Static(smDescBase, smData);

	/* Check configuration of derived state machine */
	if (FwSmCheck(smDescDer) != smSuccess) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDescDer);
	if ((smData->counter_1!=8) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Execute state machine twice and check success */
	FwSmMakeTrans(smDescDer, FW_TR_EXECUTE);
	if ((smData->counter_1!=10) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmMakeTrans(smDescDer, FW_TR_EXECUTE);
	if ((smData->counter_1!=12) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Command transition from S1 to FPS and check that nothing happens (guard is not true) */
	FwSmMakeTrans(smDescDer, TR_S1_FPS);
	if ((smData->counter_1!=12) || (smData->counter_2!=1) || (FwSmGetCurState(smDescDer)!=STATE_S1)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	/* Command transition from S1 to FPS and check that transition is executed (guard is true) */
	smData->flag_2 = 1;
	FwSmMakeTrans(smDescDer, TR_S1_FPS);
	if ((smData->counter_1!=12) || (smData->counter_2!=2) || (FwSmGetCurState(smDescDer)!=0)) {
		FwSmRelease(smDescBase);
		return smTestCaseFailure;
	}

	FwSmRelease(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseConfigErr2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the test SM */
	smDesc = FwSmMakeTestSM13(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try re-defining state S1 and check that an error is reported */
	FwSmAddState(smDesc, STATE_S1, 2, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smStateIdInUse) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try adding a new state and check that an error is reported */
	FwSmAddState(smDesc, STATE_S2, 2, NULL, NULL, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smIllStateId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try re-defining choice pseudo-state CPS1 and check that an error is reported */
	FwSmAddChoicePseudoState(smDesc, CPS1, 2);
	if (FwSmGetErrCode(smDesc) != smChoiceIdInUse) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try adding a new choice pseudo-state and check that an error is reported */
	FwSmAddChoicePseudoState(smDesc, CPS2, 2);
	if (FwSmGetErrCode(smDesc) != smIllChoiceId) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try adding a new transition and check that an error is reported */
	FwSmAddTransStaToSta(smDesc, FW_TR_EXECUTE, STATE_S1, STATE_S1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Try re-defining an existing transition and check that an error is reported */
	FwSmAddTransStaToCps(smDesc, FW_TR_EXECUTE, STATE_S1, CPS1, NULL, NULL);
	if (FwSmGetErrCode(smDesc) != smTooManyTrans) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDerConfig1() {
	struct TestSmData sSmDataBase;
	struct TestSmData* smDataBase = &sSmDataBase;
	struct TestSmData sEsmDataBase;
	struct TestSmData* esmDataBase = &sEsmDataBase;
	FwSmDesc_t smDescBase, smDescDer, esmDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM6(smDataBase, esmDataBase);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Start the base state machine */
	FwSmStart(smDescBase);

	/* Force the error code of the base SM to a non-nominal value */
	smDescBase->errCode = smConfigErr;

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Get the SM embedded in state S2 of the derived SM */
	esmDescDer = FwSmGetEmbSm(smDescDer,2);

	/* check that derived SM is a clone of base SM */
	if (!IsClone(smDescBase, smDescDer)) {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Check the initial state of the derived state machine */
	if (FwSmIsStarted(smDescDer) == 1)  {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Check the initial value of the error code of the derived state machine */
	if (FwSmGetErrCode(smDescDer) != smConfigErr)  {
		FwSmReleaseDer(smDescDer);
		FwSmReleaseDer(esmDescDer);
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	FwSmReleaseDer(smDescDer);
	FwSmReleaseDer(esmDescDer);
	FwSmReleaseRec(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDerConfig2() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* Instantiate the derived state machine */
	FW_SM_INST_DER(sSmDescDer, 2, 4, 2)
	smDescDer = &sSmDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM5 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM5(NULL);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Force the error code of the base SM to a non-nominal value */
	smDescBase->errCode = smConfigErr;

	/* Initialize the derived state machine */
	FwSmInitDer(smDescDer, smDescBase);

	/* check that derived SM is a clone of base SM */
	if (!IsClone(smDescBase, smDescDer)) {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Check the initial state of the derived state machine */
	if (FwSmIsStarted(smDescDer) == 1)  {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	/* Check the initial value of the error code of the derived state machine */
	if (FwSmGetErrCode(smDescDer) != smConfigErr)  {
		FwSmReleaseRec(smDescBase);
		return smTestCaseFailure;
	}

	FwSmReleaseRec(smDescBase);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseExecCnt1() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 0;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create test SM */
	smDesc = FwSmMakeTestSM15(smData);
	if (smDesc==NULL)
		return smTestCaseFailure;

	/* Check configuration of state machine */
	if (FwSmCheck(smDesc) != smSuccess) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Check initial value of execution counters */
	if ((FwSmGetExecCnt(smDesc)!=0) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Start SM (this brings it to state S1) */
	FwSmStart(smDesc);
	if ((smData->counter_1!=0) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}

	/* Send command TR1 and check that nothing happens */
	FwSmMakeTrans(smDesc,TR1);
	if ((smData->counter_1!=0) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=0) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Execute SM and check that it makes a transition to S2 */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=1) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Send command TR1 and check that nothing happens */
	FwSmMakeTrans(smDesc,TR1);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=1) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Execute SM and check that counters are incremented but SM remains in SM2 */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=2) || (FwSmGetStateExecCnt(smDesc)!=1)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Send command TR1 and check that nothing happens */
	FwSmMakeTrans(smDesc,TR1);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=2) || (FwSmGetStateExecCnt(smDesc)!=1)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Execute SM and check that counters are incremented but SM remains in SM2 */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=1) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S2)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=3) || (FwSmGetStateExecCnt(smDesc)!=2)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Send command TR1 and check that SM moves to S1 */
	FwSmMakeTrans(smDesc,TR1);
	if ((smData->counter_1!=3) || (smData->counter_2!=0) || (FwSmGetCurState(smDesc)!=STATE_S1)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=3) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Execute SM and check that SM is stopped but counters are not reset */
	FwSmExecute(smDesc);
	if ((smData->counter_1!=3) || (smData->counter_2!=0) || (FwSmIsStarted(smDesc)!=0)) {
		FwSmRelease(smDesc);
		return smTestCaseFailure;
	}
	if ((FwSmGetExecCnt(smDesc)!=4) || (FwSmGetStateExecCnt(smDesc)!=1)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	/* Restart SM and check that counters are reset */
	FwSmStart(smDesc);
	if ((FwSmGetExecCnt(smDesc)!=0) || (FwSmGetStateExecCnt(smDesc)!=0)) {
			FwSmRelease(smDesc);
			return smTestCaseFailure;
	}

	FwSmRelease(smDesc);
	return smTestCaseSuccess;

}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseDer6() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDescBase, smDescDer;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structures for the base state machine SM5 */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 0;
	smData->logBase = 0;

	/* Create the base state machine */
	smDescBase = FwSmMakeTestSM11(NULL);
	if (smDescBase==NULL)
		return smTestCaseFailure;

	/* Check configuration of base state machine */
	if (FwSmCheckRec(smDescBase) != smSuccess) {
		return smTestCaseFailure;
	}

	/* Create the derived state machine */
	smDescDer = FwSmCreateDer(smDescBase);

	/* Set the data structure for the derived state machine */
	FwSmSetData(smDescDer, smData);

	/* Check configuration of derived state machine */
	if (FwSmCheckRec(smDescDer) != smSuccess) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	/* Start SM (this causes the SM to terminate) */
	FwSmStart(smDescDer);
	if ((smData->counter_2!=1) || (FwSmIsStarted(smDescDer)!=0)) {
		FwSmRelease(smDescBase);
		FwSmReleaseDer(smDescDer);
		return smTestCaseFailure;
	}

	FwSmReleaseDer(smDescDer);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans7() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc1, smDesc2, smDesc3;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 1;
	smData->flag_3 = 1;
	smData->logBase = 0;

	/* Create test SMs */
	smDesc1 = FwSmMakeTestSM16_1(smData);
	smDesc2 = FwSmMakeTestSM16_2(smData);
	smDesc3 = FwSmMakeTestSM16_3(smData);

	/* Check configuration of state machine */
	if ((FwSmCheck(smDesc1) != smSuccess) || (FwSmCheck(smDesc2) != smSuccess) ||
																(FwSmCheck(smDesc3) != smSuccess)) {
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc1);
	FwSmStart(smDesc2);
	FwSmStart(smDesc3);
	if ((FwSmGetCurState(smDesc1)!=STATE_S1) || (FwSmGetCurState(smDesc2)!=STATE_S1) ||
																(FwSmGetCurState(smDesc3)!=STATE_S1)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Send TR1 and check success */
	FwSmMakeTrans(smDesc1, TR1);
	FwSmMakeTrans(smDesc2, TR1);
	FwSmMakeTrans(smDesc3, TR1);
	if ((FwSmGetCurState(smDesc1)!=STATE_S2) || (FwSmGetCurState(smDesc2)!=STATE_S4) ||
																(FwSmGetCurState(smDesc3)!=STATE_S3)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Stop and Restart SM and check outcome of start operation */
	FwSmStop(smDesc1);
	FwSmStop(smDesc2);
	FwSmStop(smDesc3);
	FwSmStart(smDesc1);
	FwSmStart(smDesc2);
	FwSmStart(smDesc3);
	if ((FwSmGetCurState(smDesc1)!=STATE_S1) || (FwSmGetCurState(smDesc2)!=STATE_S1) ||
																(FwSmGetCurState(smDesc3)!=STATE_S1)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Set Flag_1 to false */
	smData->flag_1 = 0;

	/* Send TR1 and check success */
	FwSmMakeTrans(smDesc1, TR1);
	FwSmMakeTrans(smDesc2, TR1);
	FwSmMakeTrans(smDesc3, TR1);
	if ((FwSmGetCurState(smDesc1)!=STATE_S3) || (FwSmGetCurState(smDesc2)!=STATE_S4) ||
																(FwSmGetCurState(smDesc3)!=STATE_S3)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc1);
	FwSmRelease(smDesc2);
	FwSmRelease(smDesc3);
	return smTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwSmTestOutcome_t FwSmTestCaseTrans8() {
	struct TestSmData sSmData;
	struct TestSmData* smData = &sSmData;
	FwSmDesc_t smDesc1, smDesc2, smDesc3;

	/* reset log */
	fwSm_logIndex = 0;

	/* Initialize data structure holding the state machine data */
	smData->counter_1 = 0;
	smData->counter_2 = 0;
	smData->flag_1 = 1;
	smData->flag_2 = 1;
	smData->flag_3 = 1;
	smData->logBase = 0;

	/* Create test SMs */
	smDesc1 = FwSmMakeTestSM16_1(smData);
	smDesc2 = FwSmMakeTestSM16_2(smData);
	smDesc3 = FwSmMakeTestSM16_3(smData);

	/* Check configuration of state machine */
	if ((FwSmCheck(smDesc1) != smSuccess) || (FwSmCheck(smDesc2) != smSuccess) ||
																(FwSmCheck(smDesc3) != smSuccess)) {
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Start SM and check outcome of start operation */
	FwSmStart(smDesc1);
	FwSmStart(smDesc2);
	FwSmStart(smDesc3);
	if ((FwSmGetCurState(smDesc1)!=STATE_S1) || (FwSmGetCurState(smDesc2)!=STATE_S1) ||
																(FwSmGetCurState(smDesc3)!=STATE_S1)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Send TR2 and check success */
	FwSmMakeTrans(smDesc1, TR2);
	FwSmMakeTrans(smDesc2, TR2);
	FwSmMakeTrans(smDesc3, TR2);
	if ((FwSmGetCurState(smDesc1)!=STATE_S2) || (FwSmGetCurState(smDesc2)!=STATE_S4) ||
																(FwSmGetCurState(smDesc3)!=STATE_S3)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Stop and Restart SM and check outcome of start operation */
	FwSmStop(smDesc1);
	FwSmStop(smDesc2);
	FwSmStop(smDesc3);
	FwSmStart(smDesc1);
	FwSmStart(smDesc2);
	FwSmStart(smDesc3);
	if ((FwSmGetCurState(smDesc1)!=STATE_S1) || (FwSmGetCurState(smDesc2)!=STATE_S1) ||
																(FwSmGetCurState(smDesc3)!=STATE_S1)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	/* Set Flag_1 to false */
	smData->flag_1 = 0;

	/* Send TR1 and check success */
	FwSmMakeTrans(smDesc1, TR2);
	FwSmMakeTrans(smDesc2, TR2);
	FwSmMakeTrans(smDesc3, TR2);
	if ((FwSmGetCurState(smDesc1)!=STATE_S3) || (FwSmGetCurState(smDesc2)!=STATE_S4) ||
																(FwSmGetCurState(smDesc3)!=STATE_S3)){
		FwSmRelease(smDesc1);
		FwSmRelease(smDesc2);
		FwSmRelease(smDesc3);
		return smTestCaseFailure;
	}

	FwSmRelease(smDesc1);
	FwSmRelease(smDesc2);
	FwSmRelease(smDesc3);
	return smTestCaseSuccess;
}
