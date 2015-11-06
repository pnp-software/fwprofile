/**
 * @file
 * @ingroup tsRtGroup
 * Implementation of test cases for RT Container Module.
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
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "FwProfile/FwRtConstants.h"
#include "FwProfile/FwRtCore.h"
#include "FwRtTestCases.h"
#include "FwRtMakeTest.h"

/** This variable holds a time duration of 1 ms */
static struct timespec oneMs = {0,1000000};

/** This variable holds a time duration of 10 ms */
static struct timespec tenMs = {0,10000000};

/** This variable holds a time duration of 9 ms */
static struct timespec nineMs = {0,9000000};

/** This variable holds a time duration of 100 ms */
static struct timespec hundredMs = {0,100000000};

/** Number of cycles in <code>TestCaseStressRun_Notify2</code> */
#define N_NOTIFY2 10000

/*--------------------------------------------------------------------------*/
/**
 * Send a sequence of notifications to the argument container until the
 * container is stopped.
 * @param rtDesc the container descriptor
 * @return always return NULL
 */
static void* TestCaseStressRun_Notify1(void* rtDesc);

/*--------------------------------------------------------------------------*/
/**
 * Send a sequence of <code>#N_NOTIFY2</code> notifications to the argument
 * container separated by random intervals.
 * @param rtDesc the container descriptor
 * @return always return NULL
 */
static void* TestCaseStressRun_Notify2(void* rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t InitializeNotifPr(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t FinalizeNotifPr(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t ImplementNotifLogicPr(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t InitializeActivPr(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t FinalizeActivPr(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t SetUpNotification(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t ImplementActivLogic(FwRtDesc_t rtDesc);

/**
 * Dummy procedure action function.
 * @param rtDesc the container descriptor
 * @return always return 1
 */
FwRtOutcome_t ExecFuncBehaviour(FwRtDesc_t rtDesc);

/*------------------------------------------------------------------------------------------------- */
FwRtTestOutcome_t FwRtTestCaseSetAttr1() {
	struct FwRtDesc rtDesc;
	pthread_attr_t tattr;
	pthread_mutexattr_t mattr;
	pthread_condattr_t cattr;

	/* Reset the container descriptor */
	FwRtReset(&rtDesc);

	/* Set POSIX attributes and check */
	FwRtSetPosixAttr(&rtDesc, &tattr, &mattr, &cattr);
	if ((rtDesc.pThreadAttr != &tattr) || (FwRtGetActivThreadAttr(&rtDesc) != &tattr))
		return rtTestCaseFailure;

	if ((rtDesc.pMutexAttr != &mattr) || (FwRtGetMutexAttr(&rtDesc) != &mattr))
		return rtTestCaseFailure;

	if ((rtDesc.pCondAttr != &cattr) || (FwRtGetCondAttr(&rtDesc) != &cattr))
		return rtTestCaseFailure;

	/* Check error code */
	if (FwRtGetErrCode(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Initialize the container */
	FwRtInit(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;

	/* Attempt to set POSIX attributes and check error */
	FwRtSetPosixAttr(&rtDesc, &tattr, &mattr, &cattr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	/* Shutdown container and verify that the error condition has been lifted */
	FwRtShutdown(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContUninitialized)
		return rtTestCaseFailure;

	/* Reset container and verify that attribute settings have been lost */
	FwRtReset(&rtDesc);

	if ((rtDesc.pThreadAttr != NULL) || (FwRtGetActivThreadAttr(&rtDesc) != NULL))
		return rtTestCaseFailure;

	if ((rtDesc.pMutexAttr != NULL) || (FwRtGetMutexAttr(&rtDesc) != NULL))
		return rtTestCaseFailure;

	if ((rtDesc.pCondAttr != NULL) || (FwRtGetCondAttr(&rtDesc) != NULL))
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwRtTestOutcome_t FwRtTestCaseSetAction1() {
	struct FwRtDesc rtDesc;
	FwRtAction_t dummyAction;

	/* Reset the container descriptor */
	FwRtReset(&rtDesc);

	/* Set procedure actions and check success */
	FwRtSetInitializeNotifPr(&rtDesc, &InitializeNotifPr);
	if (rtDesc.initializeNotifPr != &InitializeNotifPr)
		return rtTestCaseFailure;

	FwRtSetFinalizeNotifPr(&rtDesc, &FinalizeNotifPr);
	if (rtDesc.finalizeNotifPr != &FinalizeNotifPr)
		return rtTestCaseFailure;

	FwRtSetImplementNotifLogic(&rtDesc, &ImplementNotifLogicPr);
	if (rtDesc.implementNotifLogic != &ImplementNotifLogicPr)
		return rtTestCaseFailure;

	FwRtSetInitializeActivPr(&rtDesc, &InitializeActivPr);
	if (rtDesc.initializeActivPr != &InitializeActivPr)
		return rtTestCaseFailure;

	FwRtSetFinalizeActivPr(&rtDesc, &FinalizeActivPr);
	if (rtDesc.finalizeActivPr != &FinalizeActivPr)
		return rtTestCaseFailure;

	FwRtSetSetUpNotif(&rtDesc, &SetUpNotification);
	if (rtDesc.setUpNotification != &SetUpNotification)
		return rtTestCaseFailure;

	FwRtSetImplementActivLogic(&rtDesc, &ImplementActivLogic);
	if (rtDesc.implementActivLogic != &ImplementActivLogic)
		return rtTestCaseFailure;

	FwRtSetExecFuncBehaviour(&rtDesc, &ExecFuncBehaviour);
	if (rtDesc.execFuncBehaviour != &ExecFuncBehaviour)
		return rtTestCaseFailure;

	/* Check error code */
	if (FwRtGetErrCode(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Initialize the container */
	FwRtInit(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;

	/* Attempt to set procedure actions and check error */
	FwRtSetInitializeNotifPr(&rtDesc, &InitializeNotifPr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetFinalizeNotifPr(&rtDesc, &FinalizeNotifPr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetImplementNotifLogic(&rtDesc, &ImplementNotifLogicPr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetInitializeActivPr(&rtDesc, &InitializeActivPr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetFinalizeActivPr(&rtDesc, &FinalizeActivPr);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetSetUpNotif(&rtDesc, &SetUpNotification);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetImplementActivLogic(&rtDesc, &ImplementActivLogic);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	FwRtSetExecFuncBehaviour(&rtDesc, &ExecFuncBehaviour);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	/* Shutdown container and verify that the error condition has been lifted */
	FwRtShutdown(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContUninitialized)
		return rtTestCaseFailure;

	/* Reset container and verify that action settings have been lost */
	FwRtReset(&rtDesc);

	dummyAction = rtDesc.initializeNotifPr;

	if (dummyAction == &InitializeNotifPr)
		return rtTestCaseFailure;

	if (rtDesc.finalizeNotifPr != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.implementNotifLogic != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.initializeActivPr != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.finalizeActivPr != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.setUpNotification != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.implementActivLogic != dummyAction)
		return rtTestCaseFailure;

	if (rtDesc.execFuncBehaviour != dummyAction)
		return rtTestCaseFailure;

	/* Initialize container twice and verify that second initialization fails */
	FwRtInit(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;

	FwRtInit(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtConfigErr)
		return rtTestCaseFailure;

	/* Shut down container */
	FwRtShutdown(&rtDesc);
	if (FwRtGetContState(&rtDesc) != rtContUninitialized)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwRtTestOutcome_t FwRtTestCaseRunDefault1() {
	FwRtDesc_t rtDesc;

	/* Create the RTD */
	rtDesc = (FwRtDesc_t)malloc(sizeof(struct FwRtDesc));

	/* Reset and initialize RT Container */
	FwRtReset(rtDesc);
	FwRtInit(rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Start RT Container a second time (this should have no effect) */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Notify RT Container and wait until Activation Thread has exited */
	FwRtNotify(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Stop RT Container (this should have no effect) */
	FwRtStop(rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Restart the RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Stop RT Container and wait until its activation thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Release the RTD */
	free(rtDesc);

	return rtTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwRtTestOutcome_t FwRtTestCaseRunNonNullAttr1() {
	struct FwRtDesc rtDesc;
	pthread_attr_t pThreadAttr;
	pthread_mutexattr_t pMutexAttr;
	pthread_condattr_t pCondAttr;

	/* Reset the container descriptor */
	FwRtReset(&rtDesc);

	/* Load the attributes in the container */
	FwRtSetPosixAttr(&rtDesc, &pThreadAttr, &pMutexAttr, &pCondAttr);

	/* Initialize RT Container */
	FwRtInit(&rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Start RT Container */
	FwRtStart(&rtDesc);

	/* Start RT Container a second time (this should have no effect) */
	FwRtStart(&rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Notify RT Container and wait until Activation Thread has exited */
	FwRtNotify(&rtDesc);
	FwRtWaitForTermination(&rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Stop RT Container (this should have no effect) */
	FwRtStop(&rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Restart the RT Container (this should have no effect) */
	FwRtStart(&rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Stop RT Container and wait until its activation thread has terminated */
	FwRtStop(&rtDesc);
	FwRtWaitForTermination(&rtDesc);

	/* Check default state of RT Container and its procedures */
	if (FwRtGetContState(&rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(&rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(&rtDesc) != 0)
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(&rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(&rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*------------------------------------------------------------------------------------------------- */
FwRtTestOutcome_t FwRtTestCaseRun1() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;

	/* Instantiate and initialize test container RT1 */
	rtDesc = FwRtMakeTestRT1(1);

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 0;	/* skip notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 1)
		return rtTestCaseFailure;

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 1)
		return rtTestCaseFailure;

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 2)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 1)
		return rtTestCaseFailure;

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 3)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 2)
		return rtTestCaseFailure;

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 0; /* skip functional behaviour */

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 4)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 2)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 3)
		return rtTestCaseFailure;

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do notification */
	rtData->apExecFuncBehaviourFlag = 1; /* terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Notify RT Container and wait until Activation Thread has terminated */
	FwRtNotify(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check state of counters */
	if (rtData->npFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 5)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 2)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 3)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 3)
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Retrieve the container again from its factory (this resets,
	 * reconfigures and re-initializes it) */
	rtDesc = FwRtMakeTestRT1(1);

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 0;	/* skip notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */
	rtData->apExecFuncBehaviourCounter = 0;
	rtData->apFinalCounter = 0;
	rtData->apImplActivLogicCounter = 0;
	rtData->apInitCounter = 0;
	rtData->apSetupNotifCounter = 0;
	rtData->npFinalCounter = 0;
	rtData->npImplNotifLogicCounter = 0;
	rtData->npInitCounter = 0;

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 1)
		return rtTestCaseFailure;

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 2)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 1)
		return rtTestCaseFailure;

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Notify RT Container and give Activation Thread a chance to run*/
	FwRtNotify(rtDesc);
	nanosleep(&tenMs,NULL);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 3)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 2)
		return rtTestCaseFailure;

	/* Configure RT1 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do notification */
	rtData->apExecFuncBehaviourFlag = 1; /* terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Notify RT Container and wait until Activation Thread has terminated */
	FwRtNotify(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check state of counters */
	if (rtData->npFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 4)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 2)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 2)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 2)
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseRun2() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;

	/* Instantiate and initialize test container RT2 */
	rtDesc = FwRtMakeTestRT2(1);

	/* Configure RT2 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do not skip notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Notify RT Container five consecutive times */
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);

	/* Check state of RT Container and its procedures */
	if ((FwRtGetNotifCounter(rtDesc) != 5) && (FwRtGetNotifCounter(rtDesc) != 4))
		return rtTestCaseFailure;
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;

	/* Wait 10 ms */
	nanosleep(&tenMs,NULL);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Stop RT Container and wait until Activation Thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check state of counters */
	if (rtData->npFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 5)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter != 5)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != 5)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != 6)
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseRun3() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;

	/* Instantiate and initialize test container RT2 */
	rtDesc = FwRtMakeTestRT2(1);

	/* Configure RT2 */
	rtData = (struct TestRtData*)rtDesc->rtData;
	rtData->npImplNotifLogicFlag = 1;	/* do not skip notification */
	rtData->apExecFuncBehaviourFlag = 0; /* do not terminate functional behaviour */
	rtData->apImplActivLogicFlag = 1; /* execute functional behaviour */

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Notify RT Container a few consecutive times */
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);
	FwRtNotify(rtDesc);

	/* Wait 3 ms (enough for some of the notification requests to be processed) */
	nanosleep(&oneMs,NULL);
	nanosleep(&oneMs,NULL);
	nanosleep(&oneMs,NULL);

	/* Stop RT Container and wait until Activation Thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check state of counters */
	if (rtData->npFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 9)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apExecFuncBehaviourCounter == 0)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter == 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter == 0)
		return rtTestCaseFailure;

	/* Check state of RT Container and its procedures */
	if (FwRtGetNotifCounter(rtDesc) == 0)
		return rtTestCaseFailure;
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun1() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	int i;

	/* Instantiate and initialize test container RT3 */
	rtDesc = FwRtMakeTestRT3(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Perform loop of notifications */
	for (i=1; i<10000; i++) {
		if (rand()<RAND_MAX/2)
			nanosleep(&oneMs,NULL);
		FwRtNotify(rtDesc);
	}

	/* Give Activation Thread a chance to process all pending notifications */
	nanosleep(&hundredMs,NULL);

	/* Stop RT Container and wait until Activation Thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Check state of counters */
	if (rtData->npFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != rtData->apImplActivLogicCounter)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter == 0)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter < rtData->apExecFuncBehaviourCounter)
		return rtTestCaseFailure;

	/* Check state of RT Container and its procedures */
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun2() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	int i, nStartStopCycles;

	/* Instantiate and initialize test container RT3 */
	rtDesc = FwRtMakeTestRT4(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Perform loop of notifications and start/stop cycles */
	nStartStopCycles = 0;
	for (i=1; i<10000; i++) {
		if (rand()<RAND_MAX/2)
			nanosleep(&oneMs,NULL);
		FwRtNotify(rtDesc);

		/* Check if Activation Procedure has terminated */
		if (rtData->apExecFuncBehaviourFlag != 0) {
			nStartStopCycles++;
			/* Wait until Activation Thread has terminated */
			FwRtWaitForTermination(rtDesc);
			/* Check state of counters */
			if (rtData->npFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->npImplNotifLogicCounter < rtData->apImplActivLogicCounter)
				return rtTestCaseFailure;
			if (rtData->npInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apImplActivLogicCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			if (rtData->apInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apSetupNotifCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			/* Check state of RT Container and its procedures */
			if (FwRtGetContState(rtDesc) != rtContStopped)
				return rtTestCaseFailure;
			if (FwRtIsActivPrStarted(rtDesc))
				return rtTestCaseFailure;
			if (FwRtIsNotifPrStarted(rtDesc))
				return rtTestCaseFailure;
			/* Reset the flag used to determine whether functional behaviour is terminated  */
			rtData->apExecFuncBehaviourFlag = 0;
			/* Re-start RT Container */
			FwRtStart(rtDesc);
		}

		/* Decide whether to force a stop/start on the container */
		if (rand()<RAND_MAX/99) {
			FwRtStop(rtDesc);
			nStartStopCycles++;
			/* Wait until Activation Thread has terminated */
			FwRtWaitForTermination(rtDesc);
			/* Check state of counters */
			if (rtData->npFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->npImplNotifLogicCounter < rtData->apImplActivLogicCounter)
				return rtTestCaseFailure;
			if (rtData->npInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apImplActivLogicCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			if (rtData->apInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apSetupNotifCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			/* Check state of RT Container and its procedures */
			if (FwRtGetContState(rtDesc) != rtContStopped)
				return rtTestCaseFailure;
			if (FwRtIsActivPrStarted(rtDesc))
				return rtTestCaseFailure;
			if (FwRtIsNotifPrStarted(rtDesc))
				return rtTestCaseFailure;
			/* Re-start RT Container */
			FwRtStart(rtDesc);
		}
	}

	/* Stop RT Container and wait until Activation Thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun3() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	int i, nStartStopCycles;
	pthread_t notifThread;
	void* status = 0;

	/* Instantiate and initialize test container RT4 */
	rtDesc = FwRtMakeTestRT4(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Start RT Container */
	FwRtStart(rtDesc);

	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;

	/* Create the thread which sends the notification requests */
	pthread_create( &notifThread, NULL, TestCaseStressRun_Notify1, rtDesc);
	nStartStopCycles = 1;

	for (i=0; i<10000; i++) {
		if (rand()<RAND_MAX/20) {
			/* Stop the RT Container */
			FwRtStop(rtDesc);
			/* Wait until the Activation and Notification Threads have terminated */
			FwRtWaitForTermination(rtDesc);
			pthread_join(notifThread,status);
			/* Check state of counters */
			if (rtData->npFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->npImplNotifLogicCounter < rtData->apImplActivLogicCounter)
				return rtTestCaseFailure;
			if (rtData->npInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apImplActivLogicCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			if (rtData->apInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apSetupNotifCounter < rtData->apExecFuncBehaviourCounter)
				return rtTestCaseFailure;
			/* Check state of RT Container and its procedures */
			if (FwRtGetContState(rtDesc) != rtContStopped)
				return rtTestCaseFailure;
			if (FwRtIsActivPrStarted(rtDesc))
				return rtTestCaseFailure;
			if (FwRtIsNotifPrStarted(rtDesc))
				return rtTestCaseFailure;
			/* Restart container and notification thread */
			nStartStopCycles++;
			FwRtStart(rtDesc);
			pthread_create( &notifThread, NULL, TestCaseStressRun_Notify1, rtDesc);
		}
	}

	/* Stop RT Container and wait until Activation Thread has terminated */
	FwRtStop(rtDesc);
	FwRtWaitForTermination(rtDesc);

	/* Wait until notification thread has terminated */
	pthread_join(notifThread,status);

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun4() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	int i, nStartStopCycles;
	pthread_t notifThread1, notifThread2;
	void* status = 0;

	/* Instantiate and initialize test container RT5 */
	rtDesc = FwRtMakeTestRT5(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Configure container's actions */
	rtData->apImplActivLogicFlag = 1;	/* Do not skip functional behaviour */
	rtData->npImplNotifLogicFlag = 1;	/* Do not skip notification */

	/* Create the threads which send the notification requests */
	pthread_create( &notifThread1, NULL, TestCaseStressRun_Notify2, rtDesc);
	pthread_create( &notifThread2, NULL, TestCaseStressRun_Notify2, rtDesc);
	nStartStopCycles = 0;

	for (i=0; i<20; i++) {
		/* Start container */
		nStartStopCycles++;
		FwRtStart(rtDesc);
		/* Wait until the Activation Thread has terminated */
		FwRtWaitForTermination(rtDesc);
		/* Check state of counters */
		if (rtData->npFinalCounter != nStartStopCycles)
			return rtTestCaseFailure;
		if (rtData->npImplNotifLogicCounter < rtData->apImplActivLogicCounter)
			return rtTestCaseFailure;
		if (rtData->npInitCounter != nStartStopCycles)
			return rtTestCaseFailure;
		if (rtData->apFinalCounter != nStartStopCycles)
			return rtTestCaseFailure;
		if (rtData->apImplActivLogicCounter != rtData->apExecFuncBehaviourCounter)
			return rtTestCaseFailure;
		if (rtData->apInitCounter != nStartStopCycles)
			return rtTestCaseFailure;
		if (rtData->apSetupNotifCounter != rtData->apExecFuncBehaviourCounter)
			return rtTestCaseFailure;
		/* Check state of RT Container and its procedures */
		if (FwRtGetContState(rtDesc) != rtContStopped)
			return rtTestCaseFailure;
		if (FwRtIsActivPrStarted(rtDesc))
			return rtTestCaseFailure;
		if (FwRtIsNotifPrStarted(rtDesc))
			return rtTestCaseFailure;
	}

	/* Wait until notification threads have terminated */
	pthread_join(notifThread1,status);
	pthread_join(notifThread2,status);

	/* Shutdown the RT Container */
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun5() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	pthread_t notifThread1, notifThread2;
	void* status = 0;

	/* Instantiate and initialize test container RT1 */
	rtDesc = FwRtMakeTestRT1(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Configure container's actions */
	rtData->apImplActivLogicFlag = 1;		/* Do not skip functional behaviour */
	rtData->npImplNotifLogicFlag = 1;		/* Do not skip notification */
	rtData->apExecFuncBehaviourFlag = 0;	/* Do not terminate functional behaviour */

	/* Start container */
	FwRtStart(rtDesc);

	/* Create the threads which send the notification requests */
	pthread_create( &notifThread1, NULL, TestCaseStressRun_Notify2, rtDesc);
	pthread_create( &notifThread2, NULL, TestCaseStressRun_Notify2, rtDesc);

	/* Wait until notification threads and Activation Thread have terminated */
	pthread_join(notifThread1,status);
	pthread_join(notifThread2,status);

	/* Give time to the Activation Thread to process pending notifications */
	sleep(3);

	/* Check state of counters */
	if (rtData->npFinalCounter != 0)
		return rtTestCaseFailure;
	if (FwRtGetNotifCounter(rtDesc) != 0)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != 2*N_NOTIFY2)
		return rtTestCaseFailure;
	if (rtData->npImplNotifLogicCounter != rtData->apImplActivLogicCounter)
		return rtTestCaseFailure;
	if (rtData->npInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apFinalCounter != 0)
		return rtTestCaseFailure;
	if (rtData->apImplActivLogicCounter != rtData->apExecFuncBehaviourCounter)
		return rtTestCaseFailure;
	if (rtData->apInitCounter != 1)
		return rtTestCaseFailure;
	if (rtData->apSetupNotifCounter != rtData->apExecFuncBehaviourCounter + 1)
		return rtTestCaseFailure;
	/* Check state of RT Container and its procedures */
	if (FwRtGetContState(rtDesc) != rtContStarted)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;

	/* Stop the RT Container */
	FwRtStop(rtDesc);
	if (FwRtGetContState(rtDesc) != rtContStopped)
		return rtTestCaseFailure;
	if (!FwRtIsActivPrStarted(rtDesc))
		return rtTestCaseFailure;
	if (!FwRtIsNotifPrStarted(rtDesc))
		return rtTestCaseFailure;

	/* Wait for Activation Thread termination and shutdown the RT Container */
	FwRtWaitForTermination(rtDesc);
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
FwRtTestOutcome_t FwRtTestCaseStressRun6() {
	FwRtDesc_t rtDesc;
	struct TestRtData* rtData;
	int i, nStartStopCycles;
	pthread_t notifThread1, notifThread2;
	void* status = 0;

	/* Instantiate and initialize test container RT1 */
	rtDesc = FwRtMakeTestRT1(1);
	rtData = (struct TestRtData*)FwRtGetData(rtDesc);

	/* Configure container's actions */
	rtData->apImplActivLogicFlag = 1;		/* Do not skip functional behaviour */
	rtData->npImplNotifLogicFlag = 1;		/* Do not skip notification */
	rtData->apExecFuncBehaviourFlag = 0;	/* Do not terminate functional behaviour */

	/* Start container */
	FwRtStart(rtDesc);
	nStartStopCycles = 1;

	/* Create the threads which send the notification requests */
	pthread_create( &notifThread1, NULL, TestCaseStressRun_Notify2, rtDesc);
	pthread_create( &notifThread2, NULL, TestCaseStressRun_Notify2, rtDesc);

	for (i=0; i<10000; i++) {
		if (rand()<RAND_MAX/2)
			nanosleep(&oneMs,NULL);
		if (rand()<RAND_MAX/99) {
			FwRtStop(rtDesc);
			FwRtWaitForTermination(rtDesc);
			if (rtData->npFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apFinalCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->npInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (rtData->apInitCounter != nStartStopCycles)
				return rtTestCaseFailure;
			if (FwRtGetContState(rtDesc) != rtContStopped)
				return rtTestCaseFailure;
			if (FwRtIsActivPrStarted(rtDesc))
				return rtTestCaseFailure;
			if (FwRtIsNotifPrStarted(rtDesc))
				return rtTestCaseFailure;
			/* Re-start container */
			FwRtStart(rtDesc);
			nStartStopCycles++;
		}
	}

	/* Stop the RT Container */
	FwRtStop(rtDesc);

	/* Wait for Notification and Activation Threads termination and shutdown the RT Container */
	FwRtWaitForTermination(rtDesc);
	pthread_join(notifThread1,status);
	pthread_join(notifThread2,status);
	FwRtShutdown(rtDesc);

	/* Check error code */
	if (FwRtGetErrCode(rtDesc) != 0)
		return rtTestCaseFailure;

	return rtTestCaseSuccess;
}

/*--------------------------------------------------------------------------*/
void* TestCaseStressRun_Notify1(void* rtDesc) {
	int i;
	for (i=1; i<10000; i++) {
		if (rand()<RAND_MAX/2)
			nanosleep(&nineMs,NULL);
		FwRtNotify((FwRtDesc_t)rtDesc);
		if (FwRtGetContState((FwRtDesc_t)rtDesc) != rtContStarted)
			break;
	}
	return NULL;
}

/*--------------------------------------------------------------------------*/
void* TestCaseStressRun_Notify2(void* rtDesc) {
	int i;
	for (i=0; i<N_NOTIFY2; i++) {
		if (rand()<RAND_MAX/2)
			nanosleep(&oneMs,NULL);
		FwRtNotify((FwRtDesc_t)rtDesc);
	}
	return NULL;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t InitializeNotifPr(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t FinalizeNotifPr(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t ImplementNotifLogicPr(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t InitializeActivPr(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t FinalizeActivPr(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t SetUpNotification(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t ImplementActivLogic(FwRtDesc_t rtDesc) {
	return 1;
}

/*--------------------------------------------------------------------------*/
FwRtOutcome_t ExecFuncBehaviour(FwRtDesc_t rtDesc) {
	return 1;
}


