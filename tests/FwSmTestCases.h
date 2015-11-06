/**
 * @file
 * @ingroup tsSmGroup
 * Declaration of test cases for the FW State Machine Module.
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

#ifndef FWSM_TESTCASES_H_
#define FWSM_TESTCASES_H_

/** Outcome of a state machine test case */
typedef enum {
	/**
	 * Test case success.
	 */
	smTestCaseSuccess = 1,
	/**
	 * Test case failure.
	 */
	smTestCaseFailure = 2,
} FwSmTestOutcome_t;

/**
 * Test the Start command on a simple state machine where the
 * initial pseudo-state is directly connected to a proper state
 * without an embedded state machine.
 * The test performs the following actions:
 * - it checks that, by default, a state machine is stopped
 * - it checks that the Start command brings the state machine in
 *   the expected state.
 * - it checks that re-starting a state machine which is already
 *   started has no effect.
 * .
 * The checks are performed on test state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>).
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStart1();

/**
 * Test the Start command on a state machine where the
 * initial pseudo-state is connected to a choice pseudo-state.
 * The choice pseudo-state has two out-going transitions.
 * The effect of the Start command is verified in two cases
 * corresponding to the state machine selecting first one
 * and then the other out-going transition from the choice
 * pseudo-state.
 * The checks are performed on test state machine SM2
 * (see <code>::FwSmMakeTestSM2</code>).
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStart2();

/**
 * Test the Start command on a simple state machine where the
 * initial pseudo-state is directly connected to a proper state
 * with an embedded state machine.
 * The test starts the state machine and verifies the state
 * of the outer and of the embedded state machine.
 * The checks are performed on test state machine SM3
 * (see <code>::FwSmMakeTestSM3</code>).
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStart3();

/**
 * Test the Stop command on a state machine without embedded state machines.
 * The test performs the following actions:
 * - it checks that stopping a state machine which is already
 *   stopped has no effect.
 * - it checks that the Stop command brings a state machine in
 *   the expected state.
 * .
 * The checks are performed on test state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>).
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStop1();

/**
 * Test the Stop command for a state machine with an embedded state machine.
 * The embedding state machine is test state machine SM3
 * (see <code>::FwSmMakeTestSM3</code>).
 * This state machine is brought to its state S1 which has an embedded
 * state machine. The embedded state machine is brought to its state S1.
 * The embedding state machine is stopped and it is verified that the Stop
 * command is correctly propagated to the embedded state machine.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStop2();

/**
 * Test the Stop command for a state machine with an embedded state machine.
 * The embedding state machine is test state machine SM3
 * (see <code>::FwSmMakeTestSM3</code>).
 * This state machine is brought to its state S1 which has an embedded
 * state machine. The embedded state machine terminates (it enters its
 * final pseudo-state).
 * The embedding state machine is stopped and it is verified that the
 * Stop command is correctly executed on the embedding state machine and
 * has no effect on the embedded state machine.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseStop3();

/**
 * Test the Execute command in a configuration where the command only causes
 * the do-action of a simple state to be executed.
 * This test is performed upon test state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>).
 * The test performs the following actions:
 * - It checks that executing the test state machine when it is still in
 *   the STOPPED state has no effect.
 * - It checks that the first execution of the test state machine after it
 *   has been started brings it in state S1.
 * - It checks that subsequent executions leave the state machine in S1
 *   but cause its do-action to be executed.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecute1();

/**
 * Test the Execute command in a configuration where the command only causes the do-action
 * of a state with an embedded state machine to be executed.
 * This test is performed upon test state machine SM3
 * (see <code>::FwSmMakeTestSM3</code>).
 * The test performs the following actions:
 * - It brings the state machine to state: [S1,S1].
 * - It executes the embedding state machine and checks that this results in the do-actions
 *   of both the embedding and the embedded state machine being executed.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecute2();

/**
 * Test the Execute command in a configuration where the command triggers a transition between
 * two proper states (state S1 and S2 of SM4).
 * This test is performed upon test state machine SM4 (see <code>::FwSmMakeTestSM4</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It executes the state machine and checks that this causes a transition to state S2.
 * - It executes again the state mchine and checks that this causes a transition back to
 *   state S1.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecute3();

/**
 * Test a self-transition on a simple state (state S2 of SM4).
 * This test is performed upon test state machine SM4 (see <code>::FwSmMakeTestSM4</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It executes the state machine and checks that this causes a transition to state S2.
 * - It sends transition command TR4 to the state machine and checks that this results
 *   in a self-transition on state S2.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseSelfTrans1();

/**
 * Test a direct transition between two simple states (states S1 and S2 of SM5).
 * This test is performed upon test state machine SM5 (see <code>::FwSmMakeTestSM5</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It sends it transition command TR4 and checks that this has no effect.
 * - It sends it transition command TR2 and checks that the guard between S1 and S2
 *   blocks the transition.
 * - It sends it again transition command TR2 and checks that the guard between S1 and S2
 *   allows the transition.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans1();

/**
 * Test a transition through a choice pseudo-state between two simple states (states S1 and S2 of SM5).
 * This test is performed upon test state machine SM5 (see <code>::FwSmMakeTestSM5</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It brings the state machine to state S2 by means of transition command TR2.
 * - It brings the state machine to state S1 by means of transition command TR6 and by passing
 *   through a choice pseudo-state.
 * - It brings the state machine to state S2 by means of transition command TR2.
 * - It triggers a self-transition back to state S2 by means of transition command TR6 and by passing
 *   through a choice pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans2();

/**
 * Test propagation of a transition command from an SM to its ESM in a situation where the command has
 * no effect on the SM (but may have an effect on the ESM).
 * This test is performed upon test state machine SM6 (see <code>::FwSmMakeTestSM6</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It executes the state machine and brings it to state S2. This causes the embedded state
 *   machine to be started and to enter its state S1.
 * - It sends transition command TR5 to the embedding state machine. This has no effect on either the embedding
 *   or the embedded state machine.
 * - It sends transition command TR2 to the embedding state machine. This has no effect on the embedding
 *   state machine but it causes the embedded state machine to go from state S1 to state S2.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans3();

/**
 * Test propagation of execution command to an ESM.
 * This test is performed upon test state machine SM6 (see <code>::FwSmMakeTestSM6</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It executes the state machine and brings it to state S2. This causes the embedded state
 *   machine to be started and to enter its state S1.
 * - It sends transition command TR2 to the embedding state machine. This has no effect on the embedding
 *   state machine but it causes the embedded state machine to go from state S1 to state S2.
 * - It executes the embedding state machine. This causes the do-action of the S2 state of both
 *   the embedding and embedded state machines to be executed and triggers a transition in the
 *   embedding state machine from state S2 to state S1. This also causes the embedded state
 *   machine to terminate.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecute4();

/**
 * Test a transition command which triggers a transition in both the embedding and in the
 * embedded state machine.
 * This test is performed upon test state machine SM6 (see <code>::FwSmMakeTestSM6</code>).
 * The test performs the following actions:
 * - It brings the state machine to its initial state S1.
 * - It executes the state machine and brings it to state S2. This causes the embedded state
 *   machine to be started and to enter its state S1.
 * - It sends transition command TR2 to the embedding state machine. This has no effect on the embedding
 *   state machine but it causes the embedded state machine to go from state S1 to state S2.
 * - It sends transition command TR4 to the embedding state machine. This triggers a self-transition
 *   in both the embedding and the embedded state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans4();

/**
 * Verify the correct behaviour of a state machine descriptor which was created by
 * directly instantiating and initializing its internal data structures.
 * This test is performed upon test state machine SM5 (see <code>::FwSmMakeTestSM5Dir</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It sends transition command TR4 to the state machine and checks that this has no effect.
 * - It sends transition command TR2 to the state machine when the guard on the transition from
 *   S1 to S2 is false and it checks that the transition does not take place.
 * - It sends transition command TR2 to the state machine when the guard on the transition from
 *   S1 to S2 is true and it checks that the transition takes place.
 * - It sends transition command TR6 with the guards set to trigger a self-transition from
 *   S2 back to S2.
 * - It sends transition command TR5 with the guards set to trigger a transition from S2
 *   to the final pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDescDir1();

/**
 * Verify the correct behaviour of a state machine descriptor which was created
 * statically using the services offered by <code>FwSmSCreate.h</code>.
 * This test is performed upon test state machine SM5 (see <code>::FwSmMakeTestSM5Static</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It sends transition command TR4 to the state machine and checks that this has no effect.
 * - It sends transition command TR2 to the state machine when the guard on the transition from
 *   S1 to S2 is false and it checks that the transition does not take place.
 * - It sends transition command TR2 to the state machine when the guard on the transition from
 *   S1 to S2 is true and it checks that the transition takes place.
 * - It sends transition command TR6 with the guards set to trigger a self-transition from
 *   S2 back to S2.
 * - It sends transition command TR5 with the guards set to trigger a transition from S2
 *   to the final pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDescStatic2();

/**
 * Test an application which attempts to perform illegal configuration operations
 * on a State Machine Descriptor.
 * This test attempts the following illegal configuration operations:
 * - Add the same state twice
 * - Add a state with a negative number of out-going transitions
 * - Add a state with an illegal identifier
 * - Add the same choice pseudo-state twice
 * - Add a choice pseudo-state with an illegal identifier
 * - Add a choice pseudo-state with too many out-going transitions
 * - Add too many transitions
 * - Add the transition out of the initial pseudo-state twice
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseConfigErr1();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due to a state in a state machine not having been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck1();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due to a choice pseudo-state in a state machine not
 * having been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck2();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due to a transition in a state machine not having been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck3();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due to initial pseudo-state in a state machine not having
 * been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck4();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due an out-going transition from a state not having
 * been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck5();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due an out-going transition from a state not having
 * been defined.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck6();

/**
 * Verify the ability of the configuration functions of a state machine
 * to detect and report the following situation: a state is defined as having
 * zero out-going transitions but then an attempt is made to add a transition
 * which has that state as its source.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck7();

/**
 * Verify the ability of the configuration functions of a state machine
 * to detect and report the following situation: a choice pseudo-state is defined
 * as having two out-going transitions but then an attempt is made to add three
 * transitions which have that choice pseudo-state as their source.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck8();

/**
 * Verify the ability of the configuration check to detect and report a configuration
 * error in an embedded state machine.
 * This test performs the following actions:
 * - A state machine ESM is left only partially configured.
 * - State machine ESM is embedded within another (correctly configured) state machine.
 * - It is checked that the <code>::FwSmCheck</code> function finds no configuration errors
 *   in the embedding state machine.
 * - It is checked that the <code>::FwSmCheckRec</code> function reports a configuration
 *   error (due to the embedded state machine being incorrectly configured).
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck9();

/**
 * Verify the ability of the configuration functions of a state machine
 * to detect and report the following situation: a transition is defined having
 * an illegal destination.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck10();

/**
 * Test a transition from a state to the final pseudo-state passing through a
 * choice pseudo-state.
 * This test is performed upon test state machine SM7 (see <code>::FwSmMakeTestSM7</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It sends transition command TR7 to the state machine and checks that this brings it
 *   to its final pseudo-state via a choice pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans5();

/**
 * Verify the behaviour of a degenerate state machine which has no states.
 * This test is performed upon test state machine SM8 (see <code>::FwSmMakeTestSM8</code>).
 * The state machine is started and it is verified that this brings it to the final
 * pseudo-state.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDummySM1();

/**
 * Verify the behaviour of a degenerate state machine which has no transitions other than the
 * transition out of the initial pseudo-state.
 * This test is performed upon test state machine SM9 (see <code>::FwSmMakeTestSM9</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It executes the state machine and verifies that its do-action is executed.
 * - it stops the state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDummySM9();

/**
 * Verify the behaviour of a degenerate state machine which has no transitions other than the
 * transition out of the initial pseudo-state. The degenerate state machine is created
 * statically using the services defined in <code>FwSmSCreate.h</code>.
 * This test is performed upon test state machine SM9 (see <code>::FwSmMakeTestSM9</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It executes the state machine and verifies that its do-action is executed.
 * - it stops the state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDummySM9Static();

/**
 * Verify the ability of the state machine configuration functions to detect and
 * report illegal configuration operations.
 * The following illegal configuration operations are attempted:
 * - Defining a choice pseudo-state with zero out-going transitions
 * - Defining a transition before having defined its source state
 * - Defining a transition before having defined its source choice pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck11();

/**
 * Verify the ability of <code>::FwSmCheck</code> to detect and report the
 * configuration error due a transition having an illegal destination.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck12();

/**
 * Test a transition from one state to another state with an embedded state machine.
 * This test is performed upon test state machine SM10 (see <code>::FwSmMakeTestSM10</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to its initial state S1.
 * - It sends transition command TR1 to the state machine. This brings it to state S1
 *   and starts the state machine embedded in S1.
 * - It stops the embedding state machine. This also stops the embedded state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans6();

/**
 * Verify the ability of the configuration functions of a state machine
 * to detect and report the following situation: a transition is defined having
 * an source (either a state or a choice pseudo-state).
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck14();

/**
 * Verify the ability of the state machine creation function <code>::FwSmCreate</code>
 * to detect illegal values of its parameters.
 * The following error situations are checked:
 * - Attempt to create a state machine with an illegal number of states.
 * - Attempt to create a state machine with an illegal number of transitions.
 * - Attempt to create a state machine with an illegal number of choice pseudo-states.
 * - Attempt to create a state machine with an illegal number of actions.
 * - Attempt to create a state machine with an illegal number of guards.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck15();

/**
 * Test a transition which goes through a choice pseudo-state without any out-going
 * transition with a true guard.
 * This test is performed upon test state machine SM2 (see <code>::FwSmMakeTestSM2</code>).
 * The test performs the following actions:
 * - It starts the state machine at a time when all guards on the transitions out of the
 *   initial pseudo-state are false.
 * - It checks that the error situation has been reported in the error code of the
 *   state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTransErr1();

/**
 * Verify the behaviour of a degenerate state machine which consists of a single direct
 * transition from initial to final pseudo-state.
 * This test is performed upon test state machine SM11 (see <code>::FwSmMakeTestSM11</code>).
 * The test starts the state machine and verifies that this results in the state machine
 * terminating.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDummySM3();

/**
 * Test a transition which has a choice pseudo-state as both source and destination.
 * This test is performed upon test state machine SM12 (see <code>::FwSmMakeTestSM12</code>).
 * The state machine is started and this results in a transition being executed
 * which has a choice pseudo-state as both source and destination (this is illegal).
 * The test verifies that an error is reported in the state machine error code.
 *
 * This test also verifies the ability of the <code>::FwSmCheck</code> to detect and
 * report a situation where a state is not reachable.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTransErr2();

/**
 * Verify the ability of the configuration functions of a state machine
 * to detect and report the following situations:
 * - a state is added with too many out-going transitions
 * - a choice pseudo-state is added with too many out-going transitions.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck16();

/**
 * Configure a state to exceed the maximum number of actions and check that an error
 * is reported.
 * This test performs the following actions:
 * - it creates a state machine with no actions
 * - it attempts to define a state with an entry action and checks that an error is
 *   reported
 * - it attempts to define a state with an exit action and checks that an error is
 *   reported
 * - it attempts to define a state with a do-action and checks that an error is
 *   reported
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck17();

/**
 * Configure a transitions to exceed the maximum number of actions and the maximum
 * number of guards and check that an error is reported.
 * This test performs the following actions:
 * - it creates a state machine with no actions and no guards
 * - it attempts to define a transition with an action and checks that an error is
 *   reported
 * - it attempts to define a transition with a guard and checks that an error is
 *   reported
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck18();

/**
 * Configure a state machine to have too few actions and check that <code>::FwSmCheck</code>
 * detects and reports this situation as an error.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck19();

/**
 * Configure a state machine to have too few actions and check that <code>::FwSmCheck</code>
 * detects and reports this situation as an error.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck20();

/**
 * Configure a state machine to have a state which is not reachable and check that
 * <code>::FwSmCheck</code> detects and reports this situation as an error.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck21();

/**
 * Configure a state machine to have a choice pseudo-state which is not reachable
 * and check that <code>::FwSmCheck</code> detects and reports this situation as an error.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseCheck22();

/**
 * Create state machine SM1 statically and then check that it behaves correctly.
 * This test is performed upon test state machine SM1
 * (see <code>::FwSmMakeTestSM1Static</code>).
 * The test performs the following actions:
 * - It checks that executing the test state machine when it is still in
 *   the STOPPED state has no effect.
 * - It checks that the first execution of the test state machine after it
 *   has been started brings it in state S1.
 * - It checks that subsequent executions leave the state machine in S1
 *   but cause its do-action to be executed.
 * - It stops the state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDescStatic3();

/**
 * Verify the ability of the <code>::FwSmPrintConfig</code> function to write to a file
 * the configuration of a state machine which is correctly and fully configured.
 * The configuration of the state machines is written to file <code>FwSmPrint.out</code>.
 * This test case always returns <code>::smSuccess</code>. Correctness of behaviour
 * requires an inspection of the output file.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCasePrint1();

/**
 * Verify the ability of the <code>::FwSmPrintConfig</code> function to write to a file
 * the configuration of a state machine which is either incorrectly or partially
 * configured or which is "degenerate" (e.g. has no proper states).
 * The configuration of the state machines is written to file <code>FwSmPrint.out</code>.
 * This test case always returns <code>::smSuccess</code>. Correctness of behaviour
 * requires an inspection of the output file.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCasePrint2();

/**
 * Verify the 'Execute' command when it triggers a transition from a state to a
 * choice pseudo-state and to the final pseudo-state.
 * This test is performed upon test state machine SM13
 * (see <code>::FwSmMakeTestSM13</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to state S1.
 * - It executes the state machine with its guards configured such that this triggers a self-
 *   transition on S1 through a choice pseudo-state.
 * - It executes the state machine with its guards configured such that this triggers a
 *   transition to the final pseudo-state through a choice pseudo-state.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecute5();

/**
 * Create a derived state machine by extending state machine SM5
 * (see <code>::FwSmMakeTestSM5</code>) and then verify that the extended state machine
 * behaves like its base.
 * The extension of the base state machine is done using the dynamic
 * creation interface of <code>FwSmDCreate.h</code>.
 * This test is performed upon a state machine derived from SM5 (see
 * <code>::FwSmMakeTestSM5</code>) without overriding any of its actions or guards.
 * The test performs the following actions on the derived state machine:
 * - It brings the state machine to its initial state S1.
 * - It sends it transition command TR4 and checks that this has no effect.
 * - It sends it transition command TR2 and checks that the guard between S1 and S2
 *   blocks the transition.
 * - It sends it again transition command TR2 and checks that the guard between S1 and S2
 *   allows the transition.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer1();

/**
 * Create a derived state machine by extending state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>), override one of its actions and one
 * of its guards, and then verify the correct behaviour of the derived state
 * machine.
 * The items which are overridden are as follows:
 * - The function <code>incrCnt1By1</code> (increment Counter_1 by 1) is overridden
 *   with function <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - The guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The resulting state machine has the following characteristics (see
 * also figure below):
 * - One single state S1.
 * - No choice pseudo-states.
 * - Transition from the IPS to S1.
 * - Transition from S1 to the FPS with transition trigger TR_S1_FPS.
 * - The entry action of S1 increments <code>counter_1</code> by 8.
 * - The do action of S1 increments counter <code>counter_1</code> by 2.
 * - There is no exit action in S1.
 * - The transition actions increment counter <code>counter_2</code> by 1.
 * - The transition guard on the transition from S1 to the FPS returns the
 *   value of <code>flag_2</code>.
 * .
 * The extension of the base state machine is done using the dynamic
 * creation interface of <code>FwSmDCreate.h</code>.
 * 
 * The following actions are performed on the derived state machine:
 * - The state machine is started. This brings it to state S1.
 * - The state machine is executed twice.
 * - The state machine is sent the transition command to the final pseudo-state
 *   when the guard on the transition is false.
 * - The state machine is sent the transition command to the final pseudo-state
 *   when the guard on the transition is true. This results in the state
 *   machine terminating.
 * .
 * @image html SM1Der1.png
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer2();

/**
 * Create a derived state machine by extending state machine SM6
 * (see <code>::FwSmMakeTestSM6</code>) and then verify that the extended state machine
 * behaves like its base.
 * The extension of the base state machine is done using the dynamic
 * creation interface of <code>FwSmDCreate.h</code>.
 * The base state machine is extended after having been started.
 * None of its actions or guards is overridden.
 * 
 * The following actions are performed on the derived state machine:
 * - The state machine is started and executed. This brings the embedding state machine
 *   in S2 and starts the embedded state machine.
 * - The state machine is sent command TR2. This has no effect on the embedding state
 *   machine but causes the embedded state machine to go from S1 to S2.
 * - The state machine is executed. This causes the do-actions of both the embedding
 *   and embedded state machine to be executed and triggers a transition from S2 to S1
 *   in the embedding state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer3();

/**
 * Verify the ability of the configuration operation on a derived state
 * machine to detect and report errors.
 * The derived state machine is created using the dynamic creation interface
 * of <code>FwSmDCreate.h</code>.
 * The following error situations are covered by this test:
 * - Attempt to override a non-existent action.
 * - Attempt to override a non-existent guard.
 * - Attempt to override an action in a state machine which is not a derived
 *   state machine.
 * - Attempt to override a guard in a state machine which is not a derived
 *   state machine.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDerConfigErr1();

/**
 * Verify the ability of the configuration functions of a derived state
 * machines to detect and report errors while embedding a state machine
 * within a state of a derived state machine.
 * The following error situations are covered:
 * - Attempt to embed the same state machine twice in the same state
 * - Attempt to embed a state machine in a non-existent state
 * - Attempt to embed a state machine in a state of a state machine
 *   which is not a derived state machine
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDerEmbed1();

/**
 * Verify the use of a choice pseudo-state acting as a "junction" to join two
 * branches from two different states.
 * This test is performed upon test state machine SM13
 * (see <code>::FwSmMakeTestSM13</code>).
 * The test performs the following actions:
 * - It starts the state machine and brings it to state S1.
 * - It sends the state machine transition command TR1. This has no effect.
 * - It sends the state machine transition command TR2. This causes the
 *   state machine to move to the final pseudo-state by going through
 *   a choice pseudo-state which acts as a junction.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseJunction1();

/**
 * Create a derived state machine by extending state machine SM5
 * (see <code>::FwSmMakeTestSM5</code>) and then verify that the extended state
 * machine behaves like its base.
 * The extension of the base state machine is done using the static
 * creation interface of <code>FwSmSCreate.h</code>.
 * This test is performed upon a state machine derived from test state machine SM5
 * (see <code>::FwSmMakeTestSM5</code>) without overriding any of its actions or
 * guards.
 * The test performs the following actions on the derived state machine:
 * - It brings the state machine to its initial state S1.
 * - It sends it transition command TR4 and checks that this has no effect.
 * - It sends it transition command TR2 and checks that the guard between S1 and S2
 *   blocks the transition.
 * - It sends it again transition command TR2 and checks that the guard between S1 and S2
 *   allows the transition.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer4();

/**
 * Instantiate a derived state machine and try to initialize it with a base
 * state machine with the wrong number of actions/guards and check that
 * the error is detected and reported.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDerErr1();

/**
 * Create a derived state machine by extending state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>), override one of its actions and one
 * of its guards, and then verify the correct behaviour of the derived state
 * machine.
 * The items which are overridden are as follows:
 * - The function <code>incrCnt1By1</code> (increment Counter_1 by 1) is overridden
 *   with function <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - The guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The resulting state machine has the following characteristics (see
 * also figure below):
 * - One single state S1.
 * - No choice pseudo-states.
 * - Transition from the IPS to S1.
 * - Transition from S1 to the FPS with transition trigger TR_S1_FPS.
 * - The entry action of S1 increments <code>counter_1</code> by 8.
 * - The do action of S1 increments counter <code>counter_1</code> by 2.
 * - There is no exit action in S1.
 * - The transition actions increment counter <code>counter_2</code> by 1.
 * - The transition guard on the transition from S1 to the FPS returns the
 *   value of <code>flag_2</code>.
 * .
 * The extension of the base state machine is done using the static
 * creation interface of <code>FwSmSCreate.h</code>.
 * 
 * This test is performed upon a state machine derived from test state machine SM1
 * (see <code>::FwSmMakeTestSM1</code>) without overriding any of its actions or
 * guards.
 * The test performs the following actions on the derived state machine:
 * - It starts the state machine and brings it to its initial state S1.
 * - It executes the state machine twice.
 * - It commands a transition from S1 to the final pseudo-state when the guard on the
 *   transition is false. This has no effect.
 * - It commands a transition from S1 to the final pseudo-state when the guard on the
 *   transition is true. This causes the state machine to terminate.
 * .
 * @image html SM1Der1.png
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer5();

/**
 * Configure a state machine and then try to re-configure it and check that
 * the reconfiguration attempt fails.
 * The SM13 state machine (see <code>::FwSmMakeTestSM1</code>) is first
 * created and configured and then it is checked
 * that the following operations fail:
 * - Attempt to add a new state.
 * - Attempt to re-define an existing state.
 * - Attempt to add a new choice pseudo-state.
 * - Attempt to re-define an existing choice pseudo-state.
 * - Attempt to add a new transition.
 * - Attempt to re-define an existing transition.
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseConfigErr2();

/**
 * Create a derived state machine dynamically and then check the configuration
 * of the newly created state machine.
 * The following checks are done:
 * - The derived state machine has the same actions as the base state machine.
 * - The derived state machine has the same guards as the base state machine.
 * - The derived state machine has the same error code as the base state machine.
 * - The derived state machine is in state STOPPED.
 * .
 * The base state machine is in the STARTED state at the time the extension
 * is made and its error code has been forced to a non-nominal value.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDerConfig1();

/**
 * Create a derived state machine statically and then check the configuration
 * of the newly created state machine.
 * The following checks are done:
 * - The derived state machine has the same actions as the base state machine.
 * - The derived state machine has the same guards as the base state machine.
 * - The derived state machine has the same error code as the base state machine.
 * - The derived state machine is in state STOPPED.
 * .
 * The base state machine is in the STOPPED state at the time the extension
 * is made and its error code has been forced to a non-nominal value.
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDerConfig2();

/**
 * Verify the functioning of the execution counters.
 * This test case uses the test state machine SM15 (see
 * <code>::FwSmMakeTestSM15</code> and performs the following actions:
 * - It verifies that the counters are equal to zero when the state machine is created
 * - It verifies that both counters are incremented by one when the state machine
 *   is executed
 * - It verifies that the State Execution Counter is reset when a new state is entered
 * - It verifies that the counters are not reset when the state machine terminates
 * - It verifies that the counters are reset when the state machine is started
 * .
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseExecCnt1();

/**
 * Create a derived state machine by extending state machine SM11
 * (see <code>::FwSmMakeTestSM11</code>), and then verify the correct
 * behaviour of the derived state machine.
 * Neither action nor guard is overridden in the derived state machine.
 * State Machine SM11 is a degenerate state machine with neither states
 * nor choice pseudo-states.
 *
 * The extension of the base state machine is done using the dynamic
 * creation interface of <code>FwSmDCreate.h</code>.
 *
 * The test performs the following actions on the derived state machine:
 * - It starts the state machine and checks that the state machine is
 *   terminated and that it has executed the transition guard from the IPS
 *   to the FPS.
 * .
 * @image html images/SM11.png
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseDer6();

/**
 * Verify the order of evaluation of guards on transitions out of a proper state.
 * The test is performed on state machine SM16 (see figure below).
 * Three different versions of the state machine are used which differ for the
 * order in which the transitions are added to the state machine during the
 * state machine configuration process.
 *
 * The test performs the following actions on the state machine SM16:
 * - It starts the state machine and checks that the state machine is
 *   in state S1.
 * - It sets all three guards to true, it sends transition command TR1 to the
 *   state machine and it checks that the transition that is executed is the
 *   first to have been added to the state machine.
 * - It stops and restarts the state machine and checks that it is in state S1.
 * - It sets two guards to true and one to false, it sends transition command TR1 to the
 *   state machine and it checks that, of the two transitions with a true guard, the
 *   one which is taken is the is the first to have been added to the state machine.
 * .
 * @image html images/SM16.png
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans7();

/**
 * Verify the order of evaluation of guards on transitions out of a choice pseudo-state.
 * The test is performed on state machine SM16 (see figure below).
 * Three different versions of the state machine are used which differ for the
 * order in which the transitions are added to the state machine during the
 * state machine configuration process.
 *
 * The test performs the following actions on the state machine SM16:
 * - It starts the state machine and checks that the state machine is
 *   in state S1.
 * - It sets all three guards to true, it sends transition command TR2 to the
 *   state machine and it checks that the transition that is executed is the
 *   first to have been added to the state machine.
 * - It stops and restarts the state machine and checks that it is in state S1.
 * - It sets two guards to true and one to false, it sends transition command TR2 to the
 *   state machine and it checks that, of the two transitions with a true guard, the
 *   one which is taken is the is the first to have been added to the state machine.
 * .
 * @image html images/SM16.png
 * @return the success/failure code of the test case.
 */
FwSmTestOutcome_t FwSmTestCaseTrans8();

#endif /* FWSM_TESTCASES_H_ */
