/**
 * @file
 * @ingroup tsPrGroup
 * Declaration of the test cases for the FW Procedure Module.
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

#ifndef FWPR_TESTCASES_H_
#define FWPR_TESTCASES_H_

/** Outcome of a procedure test case */
typedef enum {
	/**
	 * Test case success.
	 */
	prTestCaseSuccess = 1,
	/**
	 * Test case failure.
	 */
	prTestCaseFailure = 2,
} FwPrTestOutcome_t;

/**
 * Test the Start command for the test procedure PR2. This test case
 * also verifies the effect of the Start command on a procedure which has
 * already been started.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseStart1();

/**
 * Test the Stop command for the test procedure PR2. This test case
 * also verifies the effect of the Stop command on a procedure which is
 * already stopped.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseStop1();

/**
 * Test the Execute command on a procedure which is stopped.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute1();

/**
 * Test the Execute command on control flows from the initial nodes and between
 * action nodes. Control flows are executed one at a time.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute2();

/**
 * Test an Execute command which triggers the traversal of several nodes.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute3();

/**
 * Test an Execute command which triggers the execution of a loop in the procedure.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute4();

/**
 * Test the behaviour of a procedure created statically.
 * This test case is the same as <code>::FwPrTestCaseExecute4</code> except that the
 * procedure under test is created statically rather than dynamically.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute4Static();

/**
 * Test the behaviour of a procedure created by directly instantiating and configuring
 * its data structures.
 * This test case is the same as <code>::FwPrTestCaseExecute4</code> except that the
 * procedure under test is created by direct instantiation and configuration of
 * its data structures..
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute4Dir();

/**
 * Test a sequence of Execute command which triggers the execution of a linear sequence of
 * actions nodes.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute5();

/**
 * Test the behaviour of a procedure created statically.
 * This test case is the same as <code>::FwPrTestCaseExecute5</code> except that the
 * procedure under test is created statically rather than dynamically.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute5Static();

/**
 * Test the ability of the <code>::FwPrCheck</code> function to detect configuration errors.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck1();

/**
 * Test the ability of the <code>::FwPrCreate</code> function to catch configuration errors.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck2();

/**
 * Test the ability of the <code>::FwPrAddActionNode</code> and <code>::FwPrAddDecisionNode</code>
 * to catch configuration errors.
 */
FwPrTestOutcome_t FwPrTestCaseCheck3();

/**
 * Test a procedure which has one single action node and no guards.
 * The test verifies that execution of the procedure results in the procedure executing its
 * single action and then terminating.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute6();

/**
 * Test a procedure which has one decision node directly connected to the initial node.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute7();

/**
 * Test the response to a situation where all flows out of a decision node have a false guard.
 * The test verifies that this situation is reported as an error.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck4();

/**
 * Test the ability of the functions which add a control flow to a procedure to detect and
 * report a configuration error.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck5();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an unconfigured action node in a procedure.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck6();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an unconfigured decision node in a procedure.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck7();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an unconfigured control flow in a procedure.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck8();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is a control flow with an illegal action node destination.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck9();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is a control flow with an illegal decision node destination.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck10();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an undefined action.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck11();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an undefined guard.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck12();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an unreachable action node (an action node which is not the
 * destination of any control flow).
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck13();

/**
 * Verify the ability of the <code>::FwPrCheck</code> function to detect and report a
 * situation where there is an unreachable decision node (a decision node which is not the
 * destination of any control flow).
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseCheck14();

/**
 * Verify the Run command on a procedure.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseRun1();

/**
 * Create a derived procedure by extending state machine PR2
 * (see <code>::FwPrMakeTestPR2</code>) and then verify that the extended procedure
 * behaves like its base.
 * The extension of the base procedure is done using the dynamic
 * creation interface of <code>FwPrDCreate.h</code>.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDer1();

/**
 * Verify that a derived procedure is, at the time it is created, a structural
 * clone of its base procedure.
 * Procedure B is a structural clone of procedure A if the following
 * conditions are satisfied:
 * - A has the same action nodes with the same actions as B;
 * - A has the same decision nodes as B;
 * - A has the same control flows between the same nodes
 *   and with the same guards as B;
 * .
 * The check is done both in the case of a derived procedure which is created
 * dynamically (using <code>::FwPrCreate</code>) and of a derived
 * procedure which is created statically (using macro <code>#FW_PR_INST_DER</code>).
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDerCheck1();

/**
 * Verify the ability of the functions which override an action or a guard in a
 * derived procedure to detect and report and error.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDerCheck2();

/**
 * Create a derived procedure by extending state machine PR2
 * (see <code>::FwPrMakeTestPR2</code>) and by overriding one of its actions
 * and one of its guards and then verify the behaviour of the derived procedure.
 * The extension of the base procedure is done using the dynamic
 * creation interface of <code>FwPrDCreate.h</code>.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDer2();

/**
 * Create a derived procedure by extending state machine PR2
 * (see <code>::FwPrMakeTestPR2</code>) and by overriding one of its actions
 * and one of its guards and then verify the behaviour of the derived procedure.
 * The extension of the base procedure is done using the static
 * creation interface of <code>FwPrSCreate.h</code>.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDer3();

/**
 * Verify the correctness of the initial state of a derived procedure.
 * The procedure is derived dynamically from a base procedure which
 * has already been started and which has an error.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDerCheck3();

/**
 * Verify the ability of function <code>::FwPrInitDer</code> to detect and
 * report errors.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDerCheck4();

/**
 * Verify the functionalities associated to the Procedure Execution Counter and
 * to the Node Execution Counter.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute8();

/**
 * Verify the correctness of the initial state of a derived procedure.
 * The procedure is derived statically from a base procedure which
 * has already been started and which has an error.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseDerCheck5();

/**
 * Verify the order of evaluation of the control flow guards.
 * @return the success/failure code of the test case.
 */
FwPrTestOutcome_t FwPrTestCaseExecute9();

#endif /* FWPR_TESTCASES_H_ */
