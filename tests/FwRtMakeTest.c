/**
 * @file
 * @ingroup tsRtGroup
 * Implementation of test RT Containers for FW RT Container Module.
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

#include "FwRtMakeTest.h"
#include "FwProfile/FwRtConstants.h"
#include "FwProfile/FwRtConfig.h"
#include <stdlib.h>
#include <time.h>

/** This variable holds a time duration of 1 ms */
static struct timespec oneMs = {0,1000000};

/** The array of RT Container data for the RT1 containers */
static struct TestRtData rt1Data[MAX_RT_INDEX];
/** The array of RT Container data for the RT2 containers */
static struct TestRtData rt2Data[MAX_RT_INDEX];
/** The array of RT Container data for the RT3 containers */
static struct TestRtData rt3Data[MAX_RT_INDEX];
/** The array of RT Container data for the RT4 containers */
static struct TestRtData rt4Data[MAX_RT_INDEX];
/** The array of RT Container data for the RT5 containers */
static struct TestRtData rt5Data[MAX_RT_INDEX];

/** The array of RT Container descriptors for RT1 containers */
struct FwRtDesc rt1Desc[MAX_RT_INDEX];
/** The array of RT Container descriptors for RT2 containers */
struct FwRtDesc rt2Desc[MAX_RT_INDEX];
/** The array of RT Container descriptors for RT3 containers */
struct FwRtDesc rt3Desc[MAX_RT_INDEX];
/** The array of RT Container descriptors for RT4 containers */
struct FwRtDesc rt4Desc[MAX_RT_INDEX];
/** The array of RT Container descriptors for RT5 containers */
struct FwRtDesc rt5Desc[MAX_RT_INDEX];

/**
 * Initialization Action for Notification Procedure.
 * This operation increments <code>::npInitCounter</code> by 1.
 * @param rtDesc the RT Container descriptor
 * @return always return 1
 */
static FwRtOutcome_t npInitAction(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->npInitCounter++;
	return 1;
}

/**
 * Finalization Action for Notification Procedure.
 * This operation increments <code>::npFinalCounter</code> by 1.
 * @param rtDesc the RT Container descriptor
 * @return always return 1
 */
static FwRtOutcome_t npFinalAction(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->npFinalCounter++;
	return 1;
}

/**
 * Implementation of Notification Logic for Notification Procedure.
 * This operation increments <code>::npImplNotifLogicCounter</code> by 1
 * and returns the value of <code>::npImplNotifLogicFlag</code>.
 * @param rtDesc the RT Container descriptor
 * @return return the value of <code>::npImplNotifLogicFlag</code>
 */
static FwRtOutcome_t npImplNotifLogic(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->npImplNotifLogicCounter++;
	return rtData->npImplNotifLogicFlag;
}

/**
 * Implementation of Notification Logic for Notification Procedure.
 * This operation increments <code>::npImplNotifLogicCounter</code> by 1
 * and returns either 1 (with probability 50%) or 0 (with probability 50%).
 * @param rtDesc the RT Container descriptor
 * @return either 1 (with probability 50%) or 0 (with probability 50%)
 */
static FwRtOutcome_t npImplNotifLogicRnd(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	if (rand()<RAND_MAX/2)
		return 0;
	else {
		rtData->npImplNotifLogicCounter++;
		return 1;
	}
}

/**
 * Initialization Action for Activation Procedure.
 * This operation increments <code>::apInitCounter</code> by 1.
 * @param rtDesc the RT Container descriptor
 * @return always return 1
 */
static FwRtOutcome_t apInitAction(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apInitCounter++;
	return 1;
}

/**
 * Finalization Action for Activation Procedure.
 * This operation increments <code>::apFInalCounter</code> by 1.
 * @param rtDesc the RT Container descriptor
 * @return always return 1
 */
static FwRtOutcome_t apFinalAction(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apFinalCounter++;
	return 1;
}

/**
 * Setup Notification Action for Activation Procedure.
 * This operation increments <code>::apSetupNotifCounter</code> by 1.
 * @param rtDesc the RT Container descriptor
 * @return always return 1
 */
static FwRtOutcome_t apSetupNotif(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apSetupNotifCounter++;
	return 1;
}

/**
 * Implementation of Activation Logic for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1
 * and returns the value of <code>::apImplActivLogicFlag</code>.
 * @param rtDesc the RT Container descriptor
 * @return return the value of <code>::apImplActivLogicFlag</code>
 */
static FwRtOutcome_t apImplActivLogic(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apImplActivLogicCounter++;
	return rtData->apImplActivLogicFlag;
}

/**
 * Implementation of Activation Logic for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1
 * and returns either 1 (with probability 50%) or 0 (with probability 50%).
 * @param rtDesc the RT Container descriptor
 * @return either 1 (with probability 50%) or 0 (with probability 50%).
 */
static FwRtOutcome_t apImplActivLogicRnd(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apImplActivLogicCounter++;
	if (rand()<RAND_MAX/2)
		return 0;
	else
		return 1;
}

/**
 * Implementation of Functional Behaviour for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1
 * and returns the value of <code>::apImplActivLogicFlag</code>.
 * @param rtDesc the RT Container descriptor
 * @return return the value of <code>::apImplActivLogicFlag</code>
 */
static FwRtOutcome_t apImplFuncBehaviour(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apExecFuncBehaviourCounter++;
	return rtData->apExecFuncBehaviourFlag;
}

/**
 * Implementation of Functional Behaviour for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1,
 * waits 1 millisecond and then returns the value of
 * <code>::apImplActivLogicFlag</code>.
 * @param rtDesc the RT Container descriptor
 * @return return the value of <code>::apImplActivLogicFlag</code>
 */
static FwRtOutcome_t apImplFuncBehaviourWithWait(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apExecFuncBehaviourCounter++;
	nanosleep(&oneMs,NULL);
	return rtData->apExecFuncBehaviourFlag;
}

/**
 * Implementation of Functional Behaviour for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1,
 * waits 1 millisecond with probability 50% and then returns 0.
 * @param rtDesc the RT Container descriptor
 * @return always return 0
 */
static FwRtOutcome_t apImplFuncBehaviourRnd(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apExecFuncBehaviourCounter++;
	if (rand()<RAND_MAX/2)
		nanosleep(&oneMs,NULL);
	rtData->apExecFuncBehaviourFlag = 0;
	return rtData->apExecFuncBehaviourFlag;
}

/**
 * Implementation of Functional Behaviour for Activation Procedure.
 * This operation increments <code>::apImplActivLogicCounter</code> by 1,
 * waits 1 millisecond with probability 50% and then either returns 0
 * with probability 99% or else it returns 1 with probability 1%.
 * @param rtDesc the RT Container descriptor
 * @return 0 with probability 99% or else it returns 1 with probability 1%
 */
static FwRtOutcome_t apImplFuncBehaviourRnd2(FwRtDesc_t rtDesc) {
	struct TestRtData* rtData = FwRtGetData(rtDesc);
	rtData->apExecFuncBehaviourCounter++;
	if (rand()<RAND_MAX/2)
		nanosleep(&oneMs,NULL);
	if (rand()<RAND_MAX/99)
		rtData->apExecFuncBehaviourFlag = 1;
	else
		rtData->apExecFuncBehaviourFlag = 0;
	return rtData->apExecFuncBehaviourFlag;
}
/*---------------------------------------------------------------------*/
FwRtDesc_t FwRtMakeTestRT1(unsigned int i) {
	if (i >= MAX_RT_INDEX)
		return NULL;

	rt1Data[i-1].apExecFuncBehaviourCounter = 0;
	rt1Data[i-1].apExecFuncBehaviourFlag = 0;
	rt1Data[i-1].apFinalCounter = 0;
	rt1Data[i-1].apImplActivLogicCounter = 0;
	rt1Data[i-1].apImplActivLogicFlag = 0;
	rt1Data[i-1].apInitCounter = 0;
	rt1Data[i-1].apSetupNotifCounter = 0;
	rt1Data[i-1].npFinalCounter = 0;
	rt1Data[i-1].npImplNotifLogicCounter = 0;
	rt1Data[i-1].npImplNotifLogicFlag = 0;
	rt1Data[i-1].npInitCounter = 0;

	/* Reset the RT Container */
	FwRtReset(&rt1Desc[i-1]);

	/* Load the container data into the RT Container */
	FwRtSetData(&rt1Desc[i-1],&rt1Data[i-1]);

	/* Define the actions of the RT Container */
	FwRtSetInitializeNotifPr(&rt1Desc[i-1],&npInitAction);
	FwRtSetFinalizeNotifPr(&rt1Desc[i-1],&npFinalAction);
	FwRtSetImplementNotifLogic(&rt1Desc[i-1],&npImplNotifLogic);
	FwRtSetInitializeActivPr(&rt1Desc[i-1],&apInitAction);
	FwRtSetFinalizeActivPr(&rt1Desc[i-1],&apFinalAction);
	FwRtSetSetUpNotif(&rt1Desc[i-1],&apSetupNotif);
	FwRtSetImplementActivLogic(&rt1Desc[i-1],&apImplActivLogic);
	FwRtSetExecFuncBehaviour(&rt1Desc[i-1],&apImplFuncBehaviour);

	/* Set Activation Thread, Mutex and Conditional Variable attributes */
	FwRtSetPosixAttr(&rt1Desc[i-1],NULL, NULL, NULL);

	/* Initialize the RT Container */
	FwRtInit(&rt1Desc[i-1]);

	return &rt1Desc[i-1];
}

/*---------------------------------------------------------------------*/
FwRtDesc_t FwRtMakeTestRT2(unsigned int i) {
	if (i >= MAX_RT_INDEX)
		return NULL;

	rt2Data[i-1].apExecFuncBehaviourCounter = 0;
	rt2Data[i-1].apExecFuncBehaviourFlag = 0;
	rt2Data[i-1].apFinalCounter = 0;
	rt2Data[i-1].apImplActivLogicCounter = 0;
	rt2Data[i-1].apImplActivLogicFlag = 0;
	rt2Data[i-1].apInitCounter = 0;
	rt2Data[i-1].apSetupNotifCounter = 0;
	rt2Data[i-1].npFinalCounter = 0;
	rt2Data[i-1].npImplNotifLogicCounter = 0;
	rt2Data[i-1].npImplNotifLogicFlag = 0;
	rt2Data[i-1].npInitCounter = 0;

	/* Reset the RT Container */
	FwRtReset(&rt2Desc[i-1]);

	/* Load the container data into the RT Container */
	FwRtSetData(&rt2Desc[i-1],&rt2Data[i-1]);

	/* Define the actions of the RT Container */
	FwRtSetInitializeNotifPr(&rt2Desc[i-1],&npInitAction);
	FwRtSetFinalizeNotifPr(&rt2Desc[i-1],&npFinalAction);
	FwRtSetImplementNotifLogic(&rt2Desc[i-1],&npImplNotifLogic);
	FwRtSetInitializeActivPr(&rt2Desc[i-1],&apInitAction);
	FwRtSetFinalizeActivPr(&rt2Desc[i-1],&apFinalAction);
	FwRtSetSetUpNotif(&rt2Desc[i-1],&apSetupNotif);
	FwRtSetImplementActivLogic(&rt2Desc[i-1],&apImplActivLogic);
	FwRtSetExecFuncBehaviour(&rt2Desc[i-1],&apImplFuncBehaviourWithWait);

	/* Set Activation Thread, Mutex and Conditional Variable attributes */
	FwRtSetPosixAttr(&rt2Desc[i-1],NULL, NULL, NULL);

	/* Initialize the RT Container */
	FwRtInit(&rt2Desc[i-1]);

	return &rt2Desc[i-1];
}

/*---------------------------------------------------------------------*/
FwRtDesc_t FwRtMakeTestRT3(unsigned int i) {
	if (i >= MAX_RT_INDEX)
		return NULL;

	rt3Data[i-1].apExecFuncBehaviourCounter = 0;
	rt3Data[i-1].apExecFuncBehaviourFlag = 0;
	rt3Data[i-1].apFinalCounter = 0;
	rt3Data[i-1].apImplActivLogicCounter = 0;
	rt3Data[i-1].apImplActivLogicFlag = 0;
	rt3Data[i-1].apInitCounter = 0;
	rt3Data[i-1].apSetupNotifCounter = 0;
	rt3Data[i-1].npFinalCounter = 0;
	rt3Data[i-1].npImplNotifLogicCounter = 0;
	rt3Data[i-1].npImplNotifLogicFlag = 0;
	rt3Data[i-1].npInitCounter = 0;

	/* Reset the RT Container */
	FwRtReset(&rt3Desc[i-1]);

	/* Load the container data into the RT Container */
	FwRtSetData(&rt3Desc[i-1],&rt3Data[i-1]);

	/* Define the actions of the RT Container */
	FwRtSetInitializeNotifPr(&rt3Desc[i-1],&npInitAction);
	FwRtSetFinalizeNotifPr(&rt3Desc[i-1],&npFinalAction);
	FwRtSetImplementNotifLogic(&rt3Desc[i-1],&npImplNotifLogicRnd);
	FwRtSetInitializeActivPr(&rt3Desc[i-1],&apInitAction);
	FwRtSetFinalizeActivPr(&rt3Desc[i-1],&apFinalAction);
	FwRtSetSetUpNotif(&rt3Desc[i-1],&apSetupNotif);
	FwRtSetImplementActivLogic(&rt3Desc[i-1],&apImplActivLogicRnd);
	FwRtSetExecFuncBehaviour(&rt3Desc[i-1],&apImplFuncBehaviourRnd);

	/* Set Activation Thread, Mutex and Conditional Variable attributes */
	FwRtSetPosixAttr(&rt3Desc[i-1],NULL, NULL, NULL);

	/* Initialize the RT Container */
	FwRtInit(&rt3Desc[i-1]);

	return &rt3Desc[i-1];
}

/*---------------------------------------------------------------------*/
FwRtDesc_t FwRtMakeTestRT4(unsigned int i) {
	if (i >= MAX_RT_INDEX)
		return NULL;

	rt4Data[i-1].apExecFuncBehaviourCounter = 0;
	rt4Data[i-1].apExecFuncBehaviourFlag = 0;
	rt4Data[i-1].apFinalCounter = 0;
	rt4Data[i-1].apImplActivLogicCounter = 0;
	rt4Data[i-1].apImplActivLogicFlag = 0;
	rt4Data[i-1].apInitCounter = 0;
	rt4Data[i-1].apSetupNotifCounter = 0;
	rt4Data[i-1].npFinalCounter = 0;
	rt4Data[i-1].npImplNotifLogicCounter = 0;
	rt4Data[i-1].npImplNotifLogicFlag = 0;
	rt4Data[i-1].npInitCounter = 0;

	/* Reset the RT Container */
	FwRtReset(&rt4Desc[i-1]);

	/* Load the container data into the RT Container */
	FwRtSetData(&rt4Desc[i-1],&rt4Data[i-1]);

	/* Define the actions of the RT Container */
	FwRtSetInitializeNotifPr(&rt4Desc[i-1],&npInitAction);
	FwRtSetFinalizeNotifPr(&rt4Desc[i-1],&npFinalAction);
	FwRtSetImplementNotifLogic(&rt4Desc[i-1],&npImplNotifLogicRnd);
	FwRtSetInitializeActivPr(&rt4Desc[i-1],&apInitAction);
	FwRtSetFinalizeActivPr(&rt4Desc[i-1],&apFinalAction);
	FwRtSetSetUpNotif(&rt4Desc[i-1],&apSetupNotif);
	FwRtSetImplementActivLogic(&rt4Desc[i-1],&apImplActivLogicRnd);
	FwRtSetExecFuncBehaviour(&rt4Desc[i-1],&apImplFuncBehaviourRnd2);

	/* Set Activation Thread, Mutex and Conditional Variable attributes */
	FwRtSetPosixAttr(&rt4Desc[i-1],NULL, NULL, NULL);

	/* Initialize the RT Container */
	FwRtInit(&rt4Desc[i-1]);

	return &rt4Desc[i-1];
}

/*---------------------------------------------------------------------*/
FwRtDesc_t FwRtMakeTestRT5(unsigned int i) {
	if (i >= MAX_RT_INDEX)
		return NULL;

	rt5Data[i-1].apExecFuncBehaviourCounter = 0;
	rt5Data[i-1].apExecFuncBehaviourFlag = 0;
	rt5Data[i-1].apFinalCounter = 0;
	rt5Data[i-1].apImplActivLogicCounter = 0;
	rt5Data[i-1].apImplActivLogicFlag = 0;
	rt5Data[i-1].apInitCounter = 0;
	rt5Data[i-1].apSetupNotifCounter = 0;
	rt5Data[i-1].npFinalCounter = 0;
	rt5Data[i-1].npImplNotifLogicCounter = 0;
	rt5Data[i-1].npImplNotifLogicFlag = 0;
	rt5Data[i-1].npInitCounter = 0;

	/* Reset the RT Container */
	FwRtReset(&rt5Desc[i-1]);

	/* Load the container data into the RT Container */
	FwRtSetData(&rt5Desc[i-1],&rt5Data[i-1]);

	/* Define the actions of the RT Container */
	FwRtSetInitializeNotifPr(&rt5Desc[i-1],&npInitAction);
	FwRtSetFinalizeNotifPr(&rt5Desc[i-1],&npFinalAction);
	FwRtSetImplementNotifLogic(&rt5Desc[i-1],&npImplNotifLogic);
	FwRtSetInitializeActivPr(&rt5Desc[i-1],&apInitAction);
	FwRtSetFinalizeActivPr(&rt5Desc[i-1],&apFinalAction);
	FwRtSetSetUpNotif(&rt5Desc[i-1],&apSetupNotif);
	FwRtSetImplementActivLogic(&rt5Desc[i-1],&apImplActivLogic);
	FwRtSetExecFuncBehaviour(&rt5Desc[i-1],&apImplFuncBehaviourRnd2);

	/* Set Activation Thread, Mutex and Conditional Variable attributes */
	FwRtSetPosixAttr(&rt5Desc[i-1],NULL, NULL, NULL);

	/* Initialize the RT Container */
	FwRtInit(&rt5Desc[i-1]);

	return &rt5Desc[i-1];
}
