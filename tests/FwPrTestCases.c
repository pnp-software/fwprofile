/**
 * @file
 * @ingroup tsPrGroup
 * Implementation of test cases for FW Procedure Module.
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
#include "FwPrConstants.h"
#include "FwPrDCreate.h"
#include "FwPrSCreate.h"
#include "FwPrTestCases.h"
#include "FwPrMakeTest.h"

/** Array where procedure actions write their marker */
extern int fwPrLogMarker[LOG_ARRAY_SIZE];
/** Array where procedure actions write the state of their SM */
extern int fwPrLogNode[LOG_ARRAY_SIZE];
/** Index for write operations in log arrays */
extern int fwPrLogIndex;

/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * Check whether two procedures are structural clones of each other.
 * Procedure B is a structural clone of procedure A if the following
 * conditions are satisfied:
 * - A has the same action nodes with the same actions as B;
 * - A has the same decision nodes as B;
 * - A has the same control flows between the same nodes
 *   and with the same guards as B;
 * .
 * This function checks that the two procedures have the same structure
 * by checking that they share the same base descriptor (this guarantees that
 * they have the same nodes connected by the same control flows).
 * @param prDesc1 first procedure
 * @param prDesc2 second procedure
 * @return 1 if the two procedures are structural clones, zero otherwise
 */
static int IsClone(FwPrDesc_t prDesc1, FwPrDesc_t prDesc2);

static int IsClone(FwPrDesc_t prDesc1, FwPrDesc_t prDesc2) {
	FwPrCounterS1_t i;

	/* Check that the two procedures share the same base descriptor */
	if (prDesc1->prBase != prDesc2->prBase)
		return 0;

	/* check that the two procedures have the same actions */
	if (prDesc1->nOfActions != prDesc2->nOfActions) {
		return 0;
	}
	for (i=0; i<prDesc1->nOfActions; i++)
		if (prDesc1->prActions[i] != prDesc2->prActions[i]) {
			return 0;
		}

	/* check that the two procedures have the same guards */
	if (prDesc1->nOfGuards != prDesc2->nOfGuards) {
		return 0;
	}
	for (i=0; i<prDesc1->nOfGuards; i++)
		if (prDesc1->prGuards[i] != prDesc2->prGuards[i]) {
			return 0;
		}

	return 1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * Dummy action which returns without doing anything.
 * @param prDesc the procedure descriptor
 */
static void DummyAction(FwPrDesc_t prDesc) {
	return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * Dummy action which returns without doing anything.
 * @param prDesc the procedure descriptor
 */
static void DummyActionBis(FwPrDesc_t prDesc) {
	return;
}

/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * Dummy guard which always returns 1.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t DummyGuard(FwPrDesc_t prDesc) {
	return 1;
}

/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * Dummy guard which always returns 1.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t DummyGuardBis(FwPrDesc_t prDesc) {
	return 1;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseStart1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Try re-starting the procedure and check that nothing happens */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseStop1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 1;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 1;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	};

	/* Stop procedure and check that nothing happens */
	FwPrStop(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start and execute procedure and check that it reaches node N3 */
	FwPrStart(procDesc);
	FwPrExecute(procDesc);
	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=N3) || (!FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Stop and check success */
	FwPrStop(procDesc);
	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that nothing happens */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute2() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that nothing happens (guard on control flow is false) */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Make first guard true, execute procedure again, and check that procedure moves to N1 */
	prData->flag_1 = 1;
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute3() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 1;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 1;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that procedure reaches node N3 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=N3)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N3)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute4() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure reaches node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Set flags to force execution of the loop: N2-D1-D2-N3-N2 */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 0;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 1;
	fwPrLogIndex = 0;	/* reset log */

	/* Execute procedure and check that procedure executes loop twice and then terminates */
	FwPrExecute(procDesc);

	if ((prData->counter_1!=6) || (fwPrLogIndex!=5) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N3)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[3]!=1) || (fwPrLogNode[3]!=N3)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[4]!=1) || (fwPrLogNode[4]!=N2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute4Static() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2Static(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure reaches node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		return prTestCaseFailure;
	}

	/* Set flags to force execution of the loop: N2-D1-D2-N3-N2 */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 0;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 1;
	fwPrLogIndex = 0;	/* reset log */

	/* Execute procedure and check that procedure executes loop twice and then terminates */
	FwPrExecute(procDesc);

	if ((prData->counter_1!=6) || (fwPrLogIndex!=5) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N2)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N3)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N2)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[3]!=1) || (fwPrLogNode[3]!=N3)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[4]!=1) || (fwPrLogNode[4]!=N2)) {
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute4Dir() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR2Dir(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure reaches node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		return prTestCaseFailure;
	}

	/* Set flags to force execution of the loop: N2-D1-D2-N3-N2 */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 0;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 1;
	fwPrLogIndex = 0;	/* reset log */

	/* Execute procedure and check that procedure executes loop twice and then terminates */
	FwPrExecute(procDesc);

	if ((prData->counter_1!=6) || (fwPrLogIndex!=5) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N2)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N3)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N2)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[3]!=1) || (fwPrLogNode[3]!=N3)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[4]!=1) || (fwPrLogNode[4]!=N2)) {
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute5() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR1(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure reaches node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure again and check that procedure remains in node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Set flags to force execution of procedure to its final node */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 0;

	/* Execute procedure and check that procedure terminates */
	FwPrExecute(procDesc);

	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N3)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute5Static() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR1Static(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure reaches node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		return prTestCaseFailure;
	}

	/* Execute procedure again and check that procedure remains in node N1 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=N1)) {
		return prTestCaseFailure;
	}

	/* Set flags to force execution of procedure to its final node */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 0;

	/* Execute procedure and check that procedure terminates */
	FwPrExecute(procDesc);

	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N1)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N2)) {
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[2]!=1) || (fwPrLogNode[2]!=N3)) {
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck1() {
	FwPrCounterS1_t nOfANodes = 2;
	FwPrCounterS1_t nOfDNodes = 0;
	FwPrCounterS1_t nOfFlows = 3;
	FwPrCounterS1_t nOfActions = 1;
	FwPrCounterS1_t nOfGuards = 1;
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create a procedure with two states but only add one state to it */
	procDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(procDesc, prData);

	FwPrAddActionNode(procDesc, 1, &DummyAction);

	FwPrAddFlowIniToAct(procDesc, 1, &DummyGuard);
	FwPrAddFlowActToAct(procDesc, 1, 2, &DummyGuard);

	if (FwPrCheck(procDesc)!=prNullActNode) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Now add the missing state but leave its action undefined */
	FwPrAddActionNode(procDesc, 2, NULL);

	if (FwPrGetErrCode(procDesc)!=prNullAction) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck2() {
	FwPrCounterS1_t nOfANodes = 1;
	FwPrCounterS1_t nOfDNodes = 0;
	FwPrCounterS1_t nOfFlows = 1;
	FwPrCounterS1_t nOfActions = 1;
	FwPrCounterS1_t nOfGuards = 0;
	FwPrDesc_t prDesc;

	/* Attempt to create a procedure with an illegal number of flows */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	/* Attempt to create a procedure with an illegal number of action nodes */
	nOfANodes = 0;
	nOfFlows = 2;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	/* Attempt to create a procedure with an illegal number of decision nodes */
	nOfANodes = 1;
	nOfDNodes = -1;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	/* Attempt to create a procedure with an illegal number of actions */
	nOfDNodes = 0;
	nOfActions = 0;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	nOfActions = 2;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	/* Attempt to create a procedure with an illegal number of guards */
	nOfActions = 1;
	nOfGuards = -1;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	nOfGuards = 3;
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc != NULL)
		return prTestCaseFailure;

	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck3() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 2;
	const FwPrCounterS1_t nOfDNodes = 1;
	const FwPrCounterS1_t nOfFlows = 4;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 4;

	/* Create a procedure with one action node and one decision node an illegal number of flows */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Attempt to add an action node with illegal identifier */
	FwPrAddActionNode(prDesc, 0, &DummyAction);
	if (FwPrGetErrCode(prDesc) != prIllActNodeId) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */
	FwPrAddActionNode(prDesc, nOfANodes+1, &DummyAction);
	if (FwPrGetErrCode(prDesc) != prIllActNodeId) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Add the action node and then add it again and check that an error is reported */
	FwPrAddActionNode(prDesc, 1, &DummyAction);
	FwPrAddActionNode(prDesc, 1, &DummyAction);
	if (FwPrGetErrCode(prDesc) != prActNodeIdInUse) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add a decision node with illegal identifier */
	FwPrAddDecisionNode(prDesc, 0, 2);
	if (FwPrGetErrCode(prDesc) != prIllDecNodeId) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */
	FwPrAddDecisionNode(prDesc, nOfDNodes+1, 2);
	if (FwPrGetErrCode(prDesc) != prIllDecNodeId) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add a decision node with an illegal number of outgoing flows */
	FwPrAddDecisionNode(prDesc, 1, 1);
	if (FwPrGetErrCode(prDesc) != prIllNOfOutFlows) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add a decision node with too many outgoing flows */
	FwPrAddDecisionNode(prDesc, 1, 3);
	if (FwPrGetErrCode(prDesc) != prTooManyOutFlows) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Add the decision node and then add it again and check that an error is reported */
	FwPrAddDecisionNode(prDesc, 1, 2);
	FwPrAddDecisionNode(prDesc, 1, 2);
	if (FwPrGetErrCode(prDesc) != prDecNodeIdInUse) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add an action node -- this will fail because all control flows are allocated */
	FwPrAddActionNode(prDesc, 2, &DummyAction);
	if (FwPrGetErrCode(prDesc) != prTooManyOutFlows) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute6() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create test procedure */
	prDesc = FwPrMakeTestPR3(prData);
	if (prDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(prDesc)!=prSuccess) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(prDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDesc)!=-1)) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure once and check that procedure increments counter and then terminates */
	FwPrExecute(prDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(prDesc)!=0) || (FwPrIsStarted(prDesc))) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute7() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create test procedure */
	procDesc = FwPrMakeTestPR4(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that procedure terminates after passing through N1  */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Set flags to force execution of procedure to its final node */
	prData->flag_1 = 0;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 1;
	prData->flag_5 = 0;
	prData->flag_6 = 0;

	/* Restart and execute procedure and check that it terminates */
	FwPrStart(procDesc);
	FwPrExecute(procDesc);

	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(procDesc)!=0) || (FwPrIsStarted(procDesc))) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck4() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create test procedure */
	procDesc = FwPrMakeTestPR4(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(procDesc)!=-1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that procedure generates an error  */
	FwPrExecute(procDesc);
	if (FwPrGetErrCode(procDesc)!=prFlowErr) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck5() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 2;
	const FwPrCounterS1_t nOfDNodes = 1;
	const FwPrCounterS1_t nOfFlows = 5;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 1;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Attempt adding a control flow from an action node which has not yet been defined */
	FwPrAddFlowActToFin(prDesc, N1, NULL);
	if (FwPrGetErrCode(prDesc) != prUndefinedFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt adding a control flow from a decision node which has not yet been defined */
	FwPrAddFlowDecToFin(prDesc, D1, &DummyGuard);
	if (FwPrGetErrCode(prDesc) != prUndefinedFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Add the first action node and the decision node to the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddDecisionNode(prDesc, D1, 2);

	/* Attempt adding a second action node with a new action (there is only room for one action) */
	FwPrAddActionNode(prDesc, N2, &DummyActionBis);
	if (FwPrGetErrCode(prDesc) != prTooManyActions) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Add the second action node */
	FwPrAddActionNode(prDesc, N2, &DummyAction);

	/* Attempt adding a control flow from an action node which does not exist */
	FwPrAddFlowActToFin(prDesc, 3, NULL);
	if (FwPrGetErrCode(prDesc) != prIllFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	FwPrAddFlowActToFin(prDesc, 0, NULL);
	if (FwPrGetErrCode(prDesc) != prIllFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt adding a control flow from a decision node which does not exist */
	FwPrAddFlowDecToFin(prDesc, 2, &DummyGuard);
	if (FwPrGetErrCode(prDesc) != prIllFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	FwPrAddFlowDecToFin(prDesc, 0, &DummyGuard);
	if (FwPrGetErrCode(prDesc) != prIllFlowSrc) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Add the first four control flows */
	FwPrAddFlowDecToAct(prDesc, D1, N1, &DummyGuard);
	FwPrAddFlowActToAct(prDesc, N1, N2, &DummyGuard);
	FwPrAddFlowActToFin(prDesc, N2, NULL);
	FwPrAddFlowIniToDec(prDesc, D1, NULL);
	if (FwPrGetErrCode(prDesc) != prSuccess) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Add the last control flow with a new guard (the flow will be added but an error will
	 * be reported because in the procedure descriptor there is space for only one guard) */
	FwPrAddFlowDecToFin(prDesc, D1, &DummyGuardBis);
	if (FwPrGetErrCode(prDesc) != prTooManyGuards) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add another control flow */
	FwPrAddFlowDecToFin(prDesc, D1, &DummyGuard);
	if (FwPrGetErrCode(prDesc) != prTooManyFlows) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Attempt to add another control flow */
	FwPrAddFlowActToFin(prDesc, N2, &DummyGuard);
	if (FwPrGetErrCode(prDesc) != prTooManyFlows) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck6() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 2;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 2;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Attempt adding a control flow from an action node which has not yet been defined and
	 * verify that configuration check fails */
	FwPrAddFlowActToFin(prDesc, N1, NULL);
	if (FwPrCheck(prDesc) != prConfigErr) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}
	prDesc->errCode = prSuccess;	/* Force error code back to "success" */

	/* Define the procedure but leave one action node undefined */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prNullActNode) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck7() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 1;
	const FwPrCounterS1_t nOfFlows = 2;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure but leave one decision node undefined */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prNullDecNode) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck8() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 3;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure but leave one control flow undefined */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prNullFlow) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck9() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 2;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1+1, NULL);		/* Illegal destination */
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prIllegalADest) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck10() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 1;
	const FwPrCounterS1_t nOfFlows = 4;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure but leave one control flow undefined */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddDecisionNode(prDesc, D1, 2);

	FwPrAddFlowIniToDec(prDesc, D1+1, NULL);	/* Illegal destination */
	FwPrAddFlowDecToAct(prDesc, D1, N1, NULL);
	FwPrAddFlowDecToFin(prDesc, D1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prIllegalDDest) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck11() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 2;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 3;
	const FwPrCounterS1_t nOfActions = 2;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddActionNode(prDesc, N2, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToAct(prDesc, N1, N2, NULL);
	FwPrAddFlowActToFin(prDesc, N2, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prTooFewActions) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck12() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 2;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 1;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prTooFewGuards) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseRun1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 1;
	prData->flag_5 = 1;
	prData->flag_6 = 1;
	prData->marker = 1;

	/* Create first test procedure */
	procDesc = FwPrMakeTestPR1(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Run procedure and check that all three action nodes have been executed and the procedure is terminated */
	FwPrRun(procDesc);
	if ((prData->counter_1!=3) || (fwPrLogIndex!=3) || (FwPrGetCurNode(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDer1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDescBase, prDescDer;

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the derive procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(NULL);
	if (prDescBase==NULL)
		return prTestCaseFailure;

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Create the derived procedure */
	prDescDer = FwPrCreateDer(prDescBase);

	/* Set the data structure for the derived procedure */
	FwPrSetData(prDescDer, prData);

	/* Check configuration of derived procedure */
	if (FwPrCheck(prDescDer) != prSuccess) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that nothing happens (guard on control flow is false) */
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Make first guard true, execute procedure again, and check that procedure moves to N1 */
	prData->flag_1 = 1;
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=1) || (fwPrLogIndex!=1) || (FwPrGetCurNode(prDescDer)!=N1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDescBase);
	FwPrReleaseDer(prDescDer);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDerCheck1() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDescBase, prDescDer;

	/* Instantiate data structure for statically created derived procedure */
	FW_PR_INST_DER(prDescDerStatic,1,8)		/* PR2 has 1 action and 8 guards */

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the derive procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(NULL);
	if (prDescBase==NULL)
		return prTestCaseFailure;

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Create the derived procedure dynamically */
	prDescDer = FwPrCreateDer(prDescBase);

	/* Initialize the statically created derived procedure */
	FwPrInitDer(&prDescDerStatic, prDescBase);

	/* Check that derived procedures are clones of their base */
	if (!IsClone(prDescBase,prDescDer)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}
	if (!IsClone(prDescBase,&prDescDerStatic)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	FwPrRelease(prDescBase);
	FwPrReleaseDer(prDescDer);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDerCheck2() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDescBase, prDescDer;
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 0;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 4;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 1;	/* Number of guards */

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the derived procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create and configure the base procedure */
	prDescBase = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(prDescBase, prData);

	FwPrAddActionNode(prDescBase, N1, &DummyAction);
	FwPrAddActionNode(prDescBase, N2, &DummyAction);
	FwPrAddActionNode(prDescBase, N3, &DummyAction);

	FwPrAddFlowIniToAct(prDescBase, N1, &DummyGuard);
	FwPrAddFlowActToAct(prDescBase, N1, N2, NULL);
	FwPrAddFlowActToAct(prDescBase, N2, N3, NULL);
	FwPrAddFlowActToFin(prDescBase, N3, NULL);

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Attempt to override an action in the base procedure and check that an error is reported */
	FwPrOverrideAction(prDescBase, &DummyAction, &DummyActionBis);
	if (FwPrGetErrCode(prDescBase)!=prNotDerivedPr) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Attempt to override a guard in the base procedure and check that an error is reported */
	FwPrOverrideGuard(prDescBase, &DummyGuard, &DummyGuardBis);
	if (FwPrGetErrCode(prDescBase)!=prNotDerivedPr) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}
	prDescBase->errCode = prSuccess;	/* Force error code back to "success" */

	/* Create the derived procedure dynamically */
	prDescDer = FwPrCreateDer(prDescBase);

	/* Check configuration of derived procedure */
	if (FwPrCheck(prDescDer) != prSuccess) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Attempt to override a non-existent action in the derived procedure */
	FwPrOverrideAction(prDescDer, &DummyActionBis, &DummyAction);
	if (FwPrGetErrCode(prDescDer)!=prUndefAction) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Attempt to override a non-existent guard in the derived procedure */
	FwPrOverrideGuard(prDescDer, &DummyGuardBis, &DummyGuard);
	if (FwPrGetErrCode(prDescDer)!=prUndefGuard) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	FwPrReleaseDer(prDescDer);
	FwPrRelease(prDescBase);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDer2() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	struct TestPrData sPrDataBase;
	struct TestPrData* prDataBase = &sPrDataBase;
	FwPrDesc_t prDescBase, prDescDer;

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the base and derived procedure */
	prDataBase->counter_1 = 0;
	prDataBase->flag_1 = 0;
	prDataBase->flag_2 = 0;
	prDataBase->flag_3 = 0;
	prDataBase->flag_4 = 0;
	prDataBase->flag_5 = 0;
	prDataBase->flag_6 = 0;
	prDataBase->marker = 1;

	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(NULL);
	if (prDescBase==NULL)
		return prTestCaseFailure;
	FwPrSetData(prDescBase, prDataBase);

	/* Start and execute once the base procedure */
	FwPrStart(prDescBase);
	FwPrExecute(prDescBase);

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Create the derived procedure (this replaces incrCnt1By1 with incrCnt1By8 and it
	 * replaces retFlag1 with retFlag2) */
	prDescDer = FwPrMakeTestPRDer1(prDescBase, prData);

	/* Check configuration of derived procedure */
	if (FwPrCheck(prDescDer) != prSuccess) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that nothing happens (guard on control flow out of initial
	 * node is false) */
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Make guard on control flow out of initial node true, execute procedure again,
	 * and check that procedure moves to N1 */
	prData->flag_2 = 1;
	prData->flag_3 = 1;	/* this causes the procedure to terminate on the first execution */
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=16) || (fwPrLogIndex!=2) || (FwPrGetCurNode(prDescDer)!=0)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N1)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N2)) {
		FwPrRelease(prDescBase);
		FwPrReleaseDer(prDescDer);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDescBase);
	FwPrReleaseDer(prDescDer);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDer3() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	struct TestPrData sPrDataBase;
	struct TestPrData* prDataBase = &sPrDataBase;
	FwPrDesc_t prDescBase, prDescDer;

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the base and derived procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 0;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	prDataBase->counter_1 = 0;
	prDataBase->flag_1 = 0;
	prDataBase->flag_2 = 0;
	prDataBase->flag_3 = 0;
	prDataBase->flag_4 = 0;
	prDataBase->flag_5 = 0;
	prDataBase->flag_6 = 0;
	prDataBase->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(NULL);
	if (prDescBase==NULL)
		return prTestCaseFailure;
	FwPrSetData(prDescBase, prDataBase);

	/* Start and execute once the base procedure */
	FwPrStart(prDescBase);
	FwPrExecute(prDescBase);

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Create the derived procedure (this replaces incrCnt1By1 with incrCnt1By8 and it
	 * replaces retFlag1 with retFlag2) */
	prDescDer = FwPrMakeTestPRDer1Static(prDescBase, prData);

	/* Check configuration of derived procedure */
	if (FwPrCheck(prDescDer) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Start procedure and check outcome of start operation */
	FwPrStart(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Execute procedure and check that nothing happens (guard on control flow out of initial
	 * node is false) */
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=0) || (fwPrLogIndex!=0) || (FwPrGetCurNode(prDescDer)!=-1)) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Make guard on control flow out of initial node true, execute procedure again,
	 * and check that procedure moves to N1 */
	prData->flag_2 = 1;
	prData->flag_3 = 1;	/* this causes the procedure to terminate on the first execution */
	FwPrExecute(prDescDer);
	if ((prData->counter_1!=16) || (fwPrLogIndex!=2) || (FwPrGetCurNode(prDescDer)!=0)) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[0]!=1) || (fwPrLogNode[0]!=N1)) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}
	if ((fwPrLogMarker[1]!=1) || (fwPrLogNode[1]!=N2)) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDescBase);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDerCheck3() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDescBase, prDescDer;

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the base procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(prData);

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Start and execute once the base procedure */
	FwPrStart(prDescBase);
	FwPrExecute(prDescBase);

	/* Force the error code of the base procedure */
	prDescBase->errCode = prTooManyActions;

	/* Create the derived procedure dynamically */
	prDescDer = FwPrCreateDer(prDescBase);

	/* Check configuration of derived procedure */
	if (FwPrCheck(prDescDer) != prConfigErr) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Check state of derived procedure */
	if (FwPrGetCurNode(prDescDer)!=0) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	if (FwPrGetErrCode(prDescDer)!=prTooManyActions) {
		FwPrReleaseDer(prDescDer);
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDescBase);
	FwPrReleaseDer(prDescDer);

	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDerCheck4() {

	/* Create the base procedure (this procedure has 1 action and 3 guards */
	FwPrDesc_t prDescBase = FwPrMakeTestPR3(NULL);

	/* Instantiate a derived procedure with 2 action and 3 two guards */
	FW_PR_INST_DER(prDescDer1, 2, 3)

	/* Instantiate a derived procedure with 1 action and 4 two guards */
	FW_PR_INST_DER(prDescDer2, 1, 4)

	/* Attempt to initialize the first derived procedure and check that this fails because
	 * the base and derived procedure do not have the same number of actions */
	FwPrInitDer(&prDescDer1, prDescBase);
	if (FwPrGetErrCode(&prDescDer1)!=prWrongNOfActions) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Attempt to initialize the second derived procedure and check that this fails because
	 * the base and derived procedure do not have the same number of guards */
	FwPrInitDer(&prDescDer2, prDescBase);
	if (FwPrGetErrCode(&prDescDer2)!=prWrongNOfGuards) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	FwPrRelease(prDescBase);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute8() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create test procedure */
	procDesc = FwPrMakeTestPR5(prData);
	if (procDesc==NULL)
		return prTestCaseFailure;

	/* Check configuration of procedure */
	if (FwPrCheck(procDesc)!=prSuccess) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure */
	FwPrStart(procDesc);
	if ((prData->counter_1!=0) || (FwPrGetExecCnt(procDesc)!=0) || (FwPrGetCurNode(procDesc)!=-1) || (FwPrGetNodeExecCnt(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure and check counters  */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (FwPrGetExecCnt(procDesc)!=1) || (FwPrGetCurNode(procDesc)!=N1) || (FwPrGetNodeExecCnt(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}
	FwPrExecute(procDesc);
	FwPrExecute(procDesc);
	if ((prData->counter_1!=1) || (FwPrGetExecCnt(procDesc)!=3) || (FwPrGetCurNode(procDesc)!=N1) || (FwPrGetNodeExecCnt(procDesc)!=2)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure again -- this should trigger transition to N2 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=4) || (FwPrGetExecCnt(procDesc)!=4) || (FwPrGetCurNode(procDesc)!=N2) || (FwPrGetNodeExecCnt(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Execute procedure again -- procedure remains in N2 */
	FwPrExecute(procDesc);
	if ((prData->counter_1!=4) || (FwPrGetExecCnt(procDesc)!=5) || (FwPrGetCurNode(procDesc)!=N2) || (FwPrGetNodeExecCnt(procDesc)!=1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Stop procedure and verify that counters are not reset */
	FwPrStop(procDesc);
	if ((FwPrGetExecCnt(procDesc)!=5) || (FwPrIsStarted(procDesc)!=0) || (FwPrGetNodeExecCnt(procDesc)!=1)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Start procedure and verify that counters are reset */
	FwPrStart(procDesc);
	if ((FwPrGetExecCnt(procDesc)!=0) || (FwPrIsStarted(procDesc)!=1) || (FwPrGetNodeExecCnt(procDesc)!=0)) {
		FwPrRelease(procDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseDerCheck5() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t prDescBase;

	/* Instantiate data structure for statically created derived procedure */
	FW_PR_INST_DER(prDescDerStatic,1,8)		/* PR2 has 1 action and 8 guards */

	/* reset log */
	fwPrLogIndex = 0;

	/* Initialize data structures for the base procedure */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 0;
	prData->flag_3 = 0;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create the base procedure */
	prDescBase = FwPrMakeTestPR2(prData);

	/* Check configuration of base procedure */
	if (FwPrCheck(prDescBase) != prSuccess) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Start and execute once the base procedure */
	FwPrStart(prDescBase);
	FwPrExecute(prDescBase);

	/* Force the error code of the base procedure */
	prDescBase->errCode = prTooManyActions;

	/* Initialize the derived procedure from the base procedure */
	FwPrInitDer(&prDescDerStatic, prDescBase);

	/* Check configuration of derived procedure */
	if (FwPrCheck(&prDescDerStatic) != prConfigErr) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Check state of derived procedure */
	if (FwPrGetCurNode(&prDescDerStatic)!=0) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	if (FwPrGetErrCode(&prDescDerStatic)!=prTooManyActions) {
		FwPrRelease(prDescBase);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDescBase);

	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseExecute9() {
	struct TestPrData sPrData;
	struct TestPrData* prData = &sPrData;
	FwPrDesc_t procDesc1;
	FwPrDesc_t procDesc2;
	FwPrDesc_t procDesc3;

	/* reset log */
	fwPrLogIndex = 0;

	/* Create and initialize data structure to hold the state machine data */
	prData->counter_1 = 0;
	prData->flag_1 = 1;
	prData->flag_2 = 1;
	prData->flag_3 = 1;
	prData->flag_4 = 0;
	prData->flag_5 = 0;
	prData->flag_6 = 0;
	prData->marker = 1;

	/* Create test procedures */
	procDesc1 = FwPrMakeTestPR6_1(prData);
	procDesc2 = FwPrMakeTestPR6_2(prData);
	procDesc3 = FwPrMakeTestPR6_3(prData);
	if ((procDesc1==NULL) || (procDesc2==NULL) || (procDesc3==NULL))
		return prTestCaseFailure;

	/* Start procedures */
	FwPrStart(procDesc1);
	FwPrStart(procDesc2);
	FwPrStart(procDesc3);

	/* Execute procedures once */
	FwPrExecute(procDesc1);
	FwPrExecute(procDesc2);
	FwPrExecute(procDesc3);
	if ((FwPrGetCurNode(procDesc1)!=N1) || (FwPrGetCurNode(procDesc2)!=N3) || (FwPrGetCurNode(procDesc3)!=N2)) {
		FwPrRelease(procDesc1);
		FwPrRelease(procDesc2);
		FwPrRelease(procDesc3);
		return prTestCaseFailure;
	}

	/* Reset flag_1 */
	prData->flag_1 = 0;

	/* Stop and restart procedures */
	FwPrStop(procDesc1);
	FwPrStop(procDesc2);
	FwPrStop(procDesc3);
	FwPrStart(procDesc1);
	FwPrStart(procDesc2);
	FwPrStart(procDesc3);

	/* Execute procedures once */
	FwPrExecute(procDesc1);
	FwPrExecute(procDesc2);
	FwPrExecute(procDesc3);
	if ((FwPrGetCurNode(procDesc1)!=N2) || (FwPrGetCurNode(procDesc2)!=N3) || (FwPrGetCurNode(procDesc3)!=N2)) {
		FwPrRelease(procDesc1);
		FwPrRelease(procDesc2);
		FwPrRelease(procDesc3);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(procDesc1);
	FwPrRelease(procDesc2);
	FwPrRelease(procDesc3);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck13() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 2;
	const FwPrCounterS1_t nOfDNodes = 0;
	const FwPrCounterS1_t nOfFlows = 3;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddActionNode(prDesc, N2, &DummyAction);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N2, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prUnreachableANode) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwPrTestOutcome_t FwPrTestCaseCheck14() {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;
	const FwPrCounterS1_t nOfDNodes = 1;
	const FwPrCounterS1_t nOfFlows = 4;
	const FwPrCounterS1_t nOfActions = 1;
	const FwPrCounterS1_t nOfGuards = 0;

	/* Create the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	if (prDesc == NULL)
		return prTestCaseFailure;

	/* Define the procedure */
	FwPrAddActionNode(prDesc, N1, &DummyAction);
	FwPrAddDecisionNode(prDesc, D1, 2);
	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);
	FwPrAddFlowDecToFin(prDesc, D1, NULL);
	FwPrAddFlowDecToFin(prDesc, D1, NULL);

	/* Verify that configuration check fails */
	if (FwPrCheck(prDesc) != prUnreachableDNode) {
		FwPrRelease(prDesc);
		return prTestCaseFailure;
	}

	/* Release memory used by test procedure and return */
	FwPrRelease(prDesc);
	return prTestCaseSuccess;
}

