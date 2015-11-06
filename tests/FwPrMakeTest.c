/**
 * @file
 * @ingroup tsPrGroup
 * Implementation of test procedures for FW Procedure Module.
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

#include "FwPrMakeTest.h"
#include "FwProfile/FwPrSCreate.h"
#include <stdlib.h>

/**
 * Log array where the actions of the test procedures write their markers.
 */
int fwPrLogMarker[LOG_ARRAY_SIZE];
/**
 * Log array where the actions of the test procedures write the current state
 * of their procedure.
 */
int fwPrLogNode[LOG_ARRAY_SIZE];
/**
 * Location in the log array where the next entry is written.
 */
int fwPrLogIndex;

/**
 * Get the pointer to the procedure data of a test procedure.
 * This function is a wrapper for the <code>::FwPrGetData</code> function.
 * Function <code>::FwPrGetData</code> is defined at the level of the
 * Procedure Module and returns the pointer to the procedure data as a
 * pointer to <code>void</code>.
 * This function cast this pointer to pointer to
 * <code>struct TestPrData*</code>.
 * @param prDesc the state machine descriptor
 * @return the procedure data
 */
struct TestPrData* GetTestPrData(FwPrDesc_t prDesc);

/**
 * Operation used as node action in the test procedures.
 * This operation increments counter_1 by 1.
 * @param prDesc the procedure descriptor
 */
static void incrCnt1By1(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	prData->counter_1++;
	fwPrLogMarker[fwPrLogIndex] = prData->marker;
	fwPrLogNode[fwPrLogIndex] = FwPrGetCurNode(prDesc);
	fwPrLogIndex++;
}

/**
 * Operation used as node action in the test procedures.
 * This operation increments counter_1 by 8.
 * @param prDesc the procedure descriptor
 */
static void incrCnt1By8(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	prData->counter_1 = prData->counter_1 + 8;
	fwPrLogMarker[fwPrLogIndex] = prData->marker;
	fwPrLogNode[fwPrLogIndex] = FwPrGetCurNode(prDesc);
	fwPrLogIndex++;
}

/**
 * Operation used as node action in the test procedures.
 * This operation assigns the value of the Procedure
 * Execution Counter to counter_1.
 * @param prDesc the procedure descriptor
 */
static void assignProcExecCntToCnt1(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	prData->counter_1 = prDesc->prExecCnt;
	fwPrLogMarker[fwPrLogIndex] = prData->marker;
	fwPrLogNode[fwPrLogIndex] = FwPrGetCurNode(prDesc);
	fwPrLogIndex++;
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag1(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_1 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag2(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_2 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag3(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_3 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag4(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_4 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag5(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_5 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t retFlag6(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->flag_6 == 1);
}

/**
 * Operation used as control flow guard in the test procedures.
 * This guard returns TRUE after three cycles have
 * elapsed in the current node.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t wait3Cycles(FwPrDesc_t prDesc) {
	return (prDesc->nodeExecCnt == 3);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t returnCounter1GreaterThan5(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->counter_1 > 5);
}

/**
 * Operation used as control flow guard in the test procedures.
 * @param prDesc the procedure descriptor
 */
static FwPrBool_t returnCounter1SmallerThan6(FwPrDesc_t prDesc) {
	struct TestPrData* prData = GetTestPrData(prDesc);
	return (prData->counter_1 < 6);
}

struct TestPrData* GetTestPrData(FwPrDesc_t prDesc) {
	return (struct TestPrData*)FwPrGetData(prDesc);
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR1(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 0;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 4;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 3;	/* Number of guards */
	FwPrDesc_t p_pr;

	/* Create and configure the procedure */
	p_pr = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(p_pr, prData);

	FwPrAddActionNode(p_pr, N1, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N2, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N3, &incrCnt1By1);

	FwPrAddFlowIniToAct(p_pr, N1, &retFlag1);
	FwPrAddFlowActToAct(p_pr, N1, N2, &retFlag2);
	FwPrAddFlowActToAct(p_pr, N2, N3, &retFlag3);
	FwPrAddFlowActToFin(p_pr, N3, &retFlag3);

	if (FwPrCheck(p_pr)==prSuccess)
		return p_pr;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR1Static(struct TestPrData* prData) {

	/* Create the procedure */
	FW_PR_INST_NODEC(prDesc, 3, 4, 1, 3)
	FwPrInit(&prDesc);

	/* Configure the procedure */
	FwPrSetData(&prDesc, prData);

	FwPrAddActionNode(&prDesc, N1, &incrCnt1By1);
	FwPrAddActionNode(&prDesc, N2, &incrCnt1By1);
	FwPrAddActionNode(&prDesc, N3, &incrCnt1By1);

	FwPrAddFlowIniToAct(&prDesc, N1, &retFlag1);
	FwPrAddFlowActToAct(&prDesc, N1, N2, &retFlag2);
	FwPrAddFlowActToAct(&prDesc, N2, N3, &retFlag3);
	FwPrAddFlowActToFin(&prDesc, N3, &retFlag3);

	if (FwPrCheck(&prDesc)==prSuccess)
		return &prDesc;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR2(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 2;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 9;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 8;	/* Number of guards */
	FwPrDesc_t prDesc;

	/* Create and configure the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(prDesc, prData);

	FwPrAddActionNode(prDesc, N1, &incrCnt1By1);
	FwPrAddActionNode(prDesc, N2, &incrCnt1By1);
	FwPrAddActionNode(prDesc, N3, &incrCnt1By1);

	FwPrAddDecisionNode(prDesc, D1, 3);
	FwPrAddDecisionNode(prDesc, D2, 2);

	FwPrAddFlowIniToAct(prDesc, N1, &retFlag1);
	FwPrAddFlowActToAct(prDesc, N1, N2, &retFlag2);
	FwPrAddFlowActToDec(prDesc, N2, D1, NULL);
	FwPrAddFlowDecToFin(prDesc, D1, &retFlag3);
	FwPrAddFlowDecToDec(prDesc, D1, D2, &retFlag4);
	FwPrAddFlowDecToAct(prDesc, D1, N3, &retFlag5);
	FwPrAddFlowActToAct(prDesc, N3, N2, &retFlag6);
	FwPrAddFlowDecToAct(prDesc, D2, N3, &returnCounter1SmallerThan6);
	FwPrAddFlowDecToFin(prDesc, D2, &returnCounter1GreaterThan5);

	if (FwPrCheck(prDesc)==prSuccess)
		return prDesc;
	else
		return NULL;
}


/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR2Dir(struct TestPrData* prData) {

	static PrANode_t aNodes[3];		/* 3 Action Nodes */
	static PrDNode_t dNodes[2]; 	/* 2 Decision Nodes */
	static PrFlow_t flows[9];		/* 9 Flows */
	static FwPrAction_t actions[1]; /* 1 Action */
	static FwPrGuard_t guards[9];	/* 9 Guards (including dummy guard) */
	static PrBaseDesc_t prBase;
	static struct FwPrDesc prDesc;

	/* Configure the array of actions */
	actions[0] = &incrCnt1By1;

	/* Configure the array of guards */
	guards[0] = &PrDummyGuard;
	guards[1] = &retFlag1;
	guards[2] = &retFlag2;
	guards[3] = &retFlag3;
	guards[4] = &retFlag4;
	guards[5] = &retFlag5;
	guards[6] = &retFlag6;
	guards[7] = &returnCounter1GreaterThan5;
	guards[8] = &returnCounter1SmallerThan6;

	/* Configure the array of action nodes */
	aNodes[0].iAction = 0;
	aNodes[0].iFlow = 1;
	aNodes[1].iAction = 0;
	aNodes[1].iFlow = 2;
	aNodes[2].iAction = 0;
	aNodes[2].iFlow = 3;

	/* Configure the array of decision nodes */
	dNodes[0].nOfOutTrans = 3;
	dNodes[0].outFlowIndex = 4;
	dNodes[1].nOfOutTrans = 2;
	dNodes[1].outFlowIndex = 7;

	/* Configure the array of flows */
	flows[0].dest = 1;
	flows[0].iGuard = 1;
	flows[1].dest = 2;
	flows[1].iGuard = 2;
	flows[2].dest = -1;
	flows[2].iGuard = 0;
	flows[3].dest = 2;
	flows[3].iGuard = 6;
	flows[4].dest = 3;
	flows[4].iGuard = 5;
	flows[5].dest = -2;
	flows[5].iGuard = 4;
	flows[6].dest = 0;
	flows[6].iGuard = 3;
	flows[7].dest = 3;
	flows[7].iGuard = 8;
	flows[8].dest = 0;
	flows[8].iGuard = 7;

	/* Configure the procedure descriptor */
	prBase.aNodes = aNodes;
	prBase.dNodes = dNodes;
	prBase.flows = flows;
	prBase.nOfANodes = 3;
	prBase.nOfDNodes = 2;
	prBase.nOfFlows = 9;
	prDesc.curNode = 0;
	prDesc.errCode = prSuccess;
	prDesc.flowCnt = 0;
	prDesc.nOfActions = 1;
	prDesc.nOfGuards = 9;
	prDesc.prActions = actions;
	prDesc.prBase = &prBase;
	prDesc.prData = prData;
	prDesc.prGuards = guards;
	prDesc.prData = prData;

	return &prDesc;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR2Static(struct TestPrData* prData) {

	/* Create the procedure */
	FW_PR_INST(prDesc, 3, 2, 9, 1, 8)
	FwPrInit(&prDesc);

	/* Configure the procedure */
	FwPrSetData(&prDesc, prData);

	FwPrAddActionNode(&prDesc, N1, &incrCnt1By1);
	FwPrAddActionNode(&prDesc, N2, &incrCnt1By1);
	FwPrAddActionNode(&prDesc, N3, &incrCnt1By1);

	FwPrAddDecisionNode(&prDesc, D1, 3);
	FwPrAddDecisionNode(&prDesc, D2, 2);

	FwPrAddFlowIniToAct(&prDesc, N1, &retFlag1);
	FwPrAddFlowActToAct(&prDesc, N1, N2, &retFlag2);
	FwPrAddFlowActToDec(&prDesc, N2, D1, NULL);
	FwPrAddFlowDecToFin(&prDesc, D1, &retFlag3);
	FwPrAddFlowDecToDec(&prDesc, D1, D2, &retFlag4);
	FwPrAddFlowDecToAct(&prDesc, D1, N3, &retFlag5);
	FwPrAddFlowActToAct(&prDesc, N3, N2, &retFlag6);
	FwPrAddFlowDecToAct(&prDesc, D2, N3, &returnCounter1SmallerThan6);
	FwPrAddFlowDecToFin(&prDesc, D2, &returnCounter1GreaterThan5);

	if (FwPrCheck(&prDesc)==prSuccess)
		return &prDesc;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR3(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 1;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 0;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 2;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 0;	/* Number of guards */
	FwPrDesc_t prDesc;

	/* Create and configure the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(prDesc, prData);

	FwPrAddActionNode(prDesc, N1, &incrCnt1By1);

	FwPrAddFlowIniToAct(prDesc, N1, NULL);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	if (FwPrCheck(prDesc)==prSuccess)
		return prDesc;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR4(struct TestPrData* prData) {
	FwPrDesc_t prDesc;
	const FwPrCounterS1_t nOfANodes = 1;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 1;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 4;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 2;	/* Number of guards */

	/* Create and configure the procedure */
	prDesc = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(prDesc, prData);

	FwPrAddActionNode(prDesc, N1, &incrCnt1By1);

	FwPrAddDecisionNode(prDesc, D1, 2);

	FwPrAddFlowIniToDec(prDesc, D1, NULL);
	FwPrAddFlowDecToAct(prDesc, D1, N1, &retFlag1);
	FwPrAddFlowDecToFin(prDesc, D1, &retFlag2);
	FwPrAddFlowActToFin(prDesc, N1, NULL);

	if (FwPrCheck(prDesc)==prSuccess)
		return prDesc;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR5(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 0;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 4;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 1;	/* Number of guards */
	FwPrDesc_t p_pr;

	/* Create and configure the procedure */
	p_pr = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(p_pr, prData);

	FwPrAddActionNode(p_pr, N1, &assignProcExecCntToCnt1);
	FwPrAddActionNode(p_pr, N2, &assignProcExecCntToCnt1);
	FwPrAddActionNode(p_pr, N3, &assignProcExecCntToCnt1);

	FwPrAddFlowIniToAct(p_pr, N1, NULL);
	FwPrAddFlowActToAct(p_pr, N1, N2, &wait3Cycles);
	FwPrAddFlowActToAct(p_pr, N2, N3, &wait3Cycles);
	FwPrAddFlowActToFin(p_pr, N3, NULL);

	if (FwPrCheck(p_pr)==prSuccess)
		return p_pr;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR6_1(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 1;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 7;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 4;	/* Number of guards */
	FwPrDesc_t p_pr;

	/* Create and configure the procedure */
	p_pr = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(p_pr, prData);

	FwPrAddActionNode(p_pr, N1, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N2, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N3, &incrCnt1By1);
	FwPrAddDecisionNode(p_pr, D1, 3);

	FwPrAddFlowIniToDec(p_pr, D1, NULL);
	FwPrAddFlowDecToAct(p_pr, D1, N1, &retFlag1);
	FwPrAddFlowDecToAct(p_pr, D1, N2, &retFlag2);
	FwPrAddFlowDecToAct(p_pr, D1, N3, &retFlag3);
	FwPrAddFlowActToFin(p_pr, N1, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N2, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N3, &retFlag4);

	if (FwPrCheck(p_pr)==prSuccess)
		return p_pr;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR6_2(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 1;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 7;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 4;	/* Number of guards */
	FwPrDesc_t p_pr;

	/* Create and configure the procedure */
	p_pr = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(p_pr, prData);

	FwPrAddActionNode(p_pr, N1, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N2, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N3, &incrCnt1By1);
	FwPrAddDecisionNode(p_pr, D1, 3);

	FwPrAddFlowIniToDec(p_pr, D1, NULL);
	FwPrAddFlowDecToAct(p_pr, D1, N3, &retFlag3);
	FwPrAddFlowDecToAct(p_pr, D1, N2, &retFlag2);
	FwPrAddFlowDecToAct(p_pr, D1, N1, &retFlag1);
	FwPrAddFlowActToFin(p_pr, N1, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N2, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N3, &retFlag4);

	if (FwPrCheck(p_pr)==prSuccess)
		return p_pr;
	else
		return NULL;
}

/* ----------------------------------------------------------------------------------------------------------------- */
FwPrDesc_t FwPrMakeTestPR6_3(struct TestPrData* prData) {
	const FwPrCounterS1_t nOfANodes = 3;	/* Number of action nodes */
	const FwPrCounterS1_t nOfDNodes = 1;	/* Number of decision nodes */
	const FwPrCounterS1_t nOfFlows = 7;		/* Number of control flows */
	const FwPrCounterS1_t nOfActions = 1;	/* Number of actions */
	const FwPrCounterS1_t nOfGuards = 4;	/* Number of guards */
	FwPrDesc_t p_pr;

	/* Create and configure the procedure */
	p_pr = FwPrCreate(nOfANodes,nOfDNodes,nOfFlows,nOfActions,nOfGuards);
	FwPrSetData(p_pr, prData);

	FwPrAddActionNode(p_pr, N1, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N2, &incrCnt1By1);
	FwPrAddActionNode(p_pr, N3, &incrCnt1By1);
	FwPrAddDecisionNode(p_pr, D1, 3);

	FwPrAddFlowIniToDec(p_pr, D1, NULL);
	FwPrAddFlowDecToAct(p_pr, D1, N2, &retFlag2);
	FwPrAddFlowDecToAct(p_pr, D1, N3, &retFlag3);
	FwPrAddFlowDecToAct(p_pr, D1, N1, &retFlag1);
	FwPrAddFlowActToFin(p_pr, N1, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N2, &retFlag4);
	FwPrAddFlowActToFin(p_pr, N3, &retFlag4);

	if (FwPrCheck(p_pr)==prSuccess)
		return p_pr;
	else
		return NULL;
}
/*-------------------------------------------------------------------------------------------------*/
FwPrDesc_t FwPrMakeTestPRDer1(FwPrDesc_t prDescBase, struct TestPrData* prData) {
	FwPrDesc_t p_pr;

	/* Create and configure the derived state machine */
	p_pr = FwPrCreateDer(prDescBase);
	FwPrOverrideAction(p_pr, &incrCnt1By1, &incrCnt1By8);
	FwPrOverrideGuard(p_pr, &retFlag1, &retFlag2);
	FwPrSetData(p_pr, prData);

	return p_pr;
}

/*-------------------------------------------------------------------------------------------------*/
FwPrDesc_t FwPrMakeTestPRDer1Static(FwPrDesc_t prDescBase, struct TestPrData* prData) {

	/* Instantiate a derived procedure. The number of actions and of guards
	 * of the derived procedure must be the same as number of actions and of
	 * guards of the base procedure.
	 */
	FW_PR_INST_DER(prDescDer, 1, 8)
	FwPrDesc_t p_pr = &prDescDer;

	/* Initialize the derived procedure */
	FwPrInitDer(p_pr, prDescBase);

	/* Configure the derived procedure */
	FwPrOverrideAction(p_pr, &incrCnt1By1, &incrCnt1By8);
	FwPrOverrideGuard(p_pr, &retFlag1, &retFlag2);
	FwPrSetData(p_pr, prData);

	return p_pr;
}

