/**
 * @file
 * @ingroup tsRtGroup
 * Declaration of the test cases for the RT Container Module.
 *
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2013, All Rights Reserved
 *
 * This file is part of the FW Profile.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#ifndef FWRT_TESTCASES_H_
#define FWRT_TESTCASES_H_

/** Outcome of a procedure test case */
typedef enum {
	/**Test case success. */
	rtTestCaseSuccess = 1,
	/** Test case failure. */
	rtTestCaseFailure = 2,
} FwRtTestOutcome_t;

/**
 * Verify the attribute setting interface of a RT Container.
 * This test case performs the following actions:
 * - Set attributes of the Activation Thread, then retrieve
 *   them, and verify that they are correct.
 * - Set attributes of the Container Mutex, then retrieve
 *   them, and verify that they are correct.
 * - Set attributes of the Container Condition Variable, then
 *   retrieve them, and verify that they are correct.
 * - Set the functions implementing the container actions, then
 *   retrieve them and verify that they are correct.
 * - Initialize the container and then verify that attempts to set
 *   the POSIX attributes result in errors.
 * - Shut down and reset the container and verify that the error condition
 *   and the attribute settings are removed.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseSetAttr1();

/**
 * Verify the action setting interface of a RT Container.
 * This test case performs the following actions:
 * - Set the procedure actions and then verify that they were
 *   correctly set.
 * - Initialize the container and then verify that attempts to set
 *   the procedure actions result in errors.
 * - Shut down and reset the container and verify that the error condition
 *   and the action settings are removed.
 * - Initialize the container twice and verify that the second initialization
 *   results in an error.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseSetAction1();

/**
 * Verify a nominal run of the RT Container with default configuration.
 * This test case performs the following actions:
 * - Instantiate a RTD on the heap and initialize it with default attributes.
 * - Check the initial state of the RT Container and its procedures.
 * - Start the RT Container and verify that the container and its procedures
 *   are started.
 * - Try to start again the RT Container (this should have no effect)
 * - Execute the Notification Procedure and check that RT Container and
 *   its procedures are stopped.
 * - Try to stop the RT Container (this should have no effect)
 * - Wait until the container thread has terminated and then reset the
 *   RT Container, start it again, and verify that it is
 *   correctly started.
 * - Stop the RT Container and verify that it is correctly stopped.
 * - Wait until the container thread has terminated and then shut it down it.
 * - Release the memory allocated to the RTD.
 * .
 * This test case verifies the following control flows in the
 * Notification Procedure:
 * - Initialization branch
 * - Else branch of first decision node
 * - Else branch of second decision node
 * - Finalization branch
 * .
 * This test case verifies the following control flows in the
 * Activation Procedure:
 * - Initialization branch
 * - Else branch of first decision node (RT Container is not STOPPED)
 * - Execution of functional behaviour authorized
 * - Functional behaviour has terminated
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseRunDefault1();

/**
 * Verify a nominal run of the RT Container with non-NULL values for
 * the attributes of the POSIX objects.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container with non-NULL values for
 *   thread attributes, for the mutex attributes and for the conditiona
 *   variable attributes.
 * - Check the initial state of the RT Container and its procedures.
 * - Start the RT Container and verify that the container and its procedures
 *   are started.
 * - Execute the Notification Procedure and check that RT Container and
 *   its procedures are stopped.
 * - Wait until the container thread has terminated and then reset the
 *   RT Container, start it again, and verify that it is
 *   correctly started.
 * - Stop the RT Container and verify that it is correctly stopped.
 * - Wait until the container thread has terminated and then shut it down.
 * .
 * This test case verifies the following control flows in the
 * Notification Procedure:
 * - Initialization branch
 * - Else branch of first decision node
 * - Else branch of second decision node
 * - Finalization branch
 * .
 * This test case verifies the following control flows in the
 * Activation Procedure:
 * - Initialization branch
 * - Else branch of first decision node (RT Container is not STOPPED)
 * - Execution of functional behaviour authorized
 * - Functional behaviour has terminated
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseRunNonNullAttr1();

/**
 * Verify the logic to skip a notification in the Notification Procedure.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT1.
 * - Configure the Notification Procedure to skip the notification and
 *   the Activation Procedure to execute the functional behavior without
 *   terminating.
 * - Start the RT Container, make a notification request and verify that
 *   the Activation Procedure is not executed.
 * - Repeat the previous step a second time.
 * - Configure the Notification Procedure to perform the notification, make a
 *   notification request, and verify that the Activation Procedure
 *   is executed once.
 * - Configure the Activation Procedure to skip execution of functional
 *   behaviour, make a notification request, and verify that the Activation
 *   Procedure is note executed.
 * - Configure the Activation Procedure to terminate its functional
 *   behaviour, make a notification request and verify that the container
 *   is stopped.
 * - Shut down the container and then configure and initialize it again
 *   and repeat the previous steps to verify that it still works correctly.
 * .
 * This test case verifies the "skip notification" branch of the
 * Notification Procedure and all branches of the Activation Procedure.
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseRun1();

/**
 * Verify the logic to handle bursts of notifications.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT2 (this container implements
 *   a functional behaviour with a "long" execution time).
 * - Configure the Notification Procedure to issue the notification and
 *   the Activation Procedure to execute the functional behavior without
 *   terminating.
 * - Start the RT Container, make five consecutive notification requests
 *   and verify that they remain pending in the container
 *   (this test may fail if the thread on which the main program runs is
 *   suspended by the host platform).
 * - Wait for a few ms and then check that all pending notifications have
 *   been processed.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseRun2();

/**
 * Verify the logic to handle a container stop request while notifications
 * are pending.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT2 (this container implements
 *   a functional behaviour with a "long" execution time).
 * - Configure the Notification Procedure to issue the notification and
 *   the Activation Procedure to execute the functional behavior without
 *   terminating.
 * - Start the RT Container, make several consecutive notification requests
 *   and then stop the container while some of these notifications are
 *   still pending.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseRun3();

/**
 * Verify the RT Container logic in a stress test with a long sequences of
 * notifications and random outcome of the container actions.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT3 (this container implements
 *   a functional behaviour with an execution time which may be long or
 *   short and actions with random outcomes).
 * - Start a loop with a long sequence of notifications which are separated by
 *   random intervals of time.
 * - Give the Activation Thread enough time to process all pending notification
 *   and then stop the RT Container and check its internal state. This check
 *   may fail if the hosting platform does not give the Activation Thread enough
 *   of a fair chance to run.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun1();

/**
 * Verify the RT Container logic in a stress test with a long sequences of
 * notifications and start/stop cycles and random outcome of the container actions.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT4 (this container implements
 *   a functional behaviour with an execution time which may be long or
 *   short and actions with random outcomes).
 * - Start a loop with a long sequence of notifications which are separated by
 *   random intervals of time and by random start/stop cycles.
 * - The state of the container is checked at the end of each start/stop cycle.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun2();

/**
 * Verify the RT Container logic in a stress test where two different threads send
 * random sequences of notification and stop requests and where the outcomes of
 * the container actions are random.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT4 (this container implements
 *   a functional behaviour with an execution time which may be long or
 *   short and actions with random outcomes).
 * - Start a separate thread which sends a sequence of notifications to the container
 *   which are separated by random intervals of time.
 * - After a random delay, send a stop request to the container and then check
 *   its state.
 * - Restart the container and repeat the above operations.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun3();

/**
 * Verify the RT Container logic in a stress test where two threads send
 * random sequences of notifications and the main thread waits for the Activation
 * Thread to terminate.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT5 (this container implements
 *   a functional behaviour with an execution time which may be long or
 *   short and actions with settable outcomes).
 * - Configure the RT Container such that its Implement Notification Logic and
 *   Implement Activation Logic actions always return 1 (i.e. notification are
 *   never skipped and always result in the container's functional behaviour being
 *   executed).
 * - Start two separate threads which send a sequence of notifications to the container
 *   which are separated by random intervals of time.
 * - Wait until the Activation Thread has terminated (termination occurs when the
 *   container's activation behaviour terminates) and then check the container state.
 * - Restart the container and repeat the above operations.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun4();

/**
 * Verify the RT Container logic in a stress test where two threads send
 * random sequences of notifications and the main thread waits until the sequence
 * of notifications has terminated.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT1 (the actions of this container increment
 *   a counter and return the value of a settable flag).
 * - Configure the RT Container such that: (a) notifications always result in the functional
 *   behaviour being executed and (b) functional behaviour never terminates.
 * - Start two separate threads which send a sequence of notifications to the container
 *   which are separated by random intervals of time.
 * - Wait until the notification threads have terminated and then check the container state.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun5();

/**
 * Verify the RT Container logic in a stress test where two threads send
 * random sequences of notifications and the main thread sends a random sequence of
 * stop requests.
 * This test case performs the following actions:
 * - Instantiate and initialize a RT Container RT2 (the actions of this container increment
 *   a counter and return the value of a settable flag and the functional behaviour takes a
 *   random amount of time to complete).
 * - Configure the RT Container such that: (a) notifications always result in the functional
 *   behaviour being executed and (b) functional behaviour never terminates.
 * - Start two separate threads which send a sequence of notifications to the container
 *   which are separated by random intervals of time.
 * - Send a random sequence of stop requests to the container and, after each stop request,
 *   wait until the Activation Thread has terminated and then check the container's state.
 * .
 * @return the success/failure code of the test case.
 */
FwRtTestOutcome_t FwRtTestCaseStressRun6();

#endif /* FWRT_TESTCASES_H_ */
