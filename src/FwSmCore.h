/**
 * @file
 * @ingroup smGroup
 * Declaration of the execution interface for a FW State Machine.
 * The execution interface offers the functions to start and stop
 * a state machine and to command a transition to a state machine.
 * The functions declared in this header file, take a state machine descriptor
 * as their first argument.
 * This represents the state machine upon which the functions operate.
 * 
 * The functions declared in this header file can only be used after a
 * state machine has been fully configured.
 * This is normally done using the configuration functions declared
 * in <code>FwSmConfig.h</code>.
 * The basic mode of use of the functions declared in this file is as follows:
 * -# The state machine is started with function <code>::FwSmStart</code>.
 * -# Transitions in the state machine are commanded with functions
 *    <code>::FwSmExecute</code> or <code>::FwSmMakeTrans</code>.
 * -# The state machine is stopped with function <code>::FwSmStop</code>.
 * .
 * 
 * The functions declared in this header file assume that they are passed
 * a valid state machine descriptor representing a correctly configured
 * state machine.
 * Failure to comply with this assumption will result in undefined
 * behaviour.
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

#ifndef FWSM_CORE_H_
#define FWSM_CORE_H_

#include "FwSmConstants.h"

/**
 * Start a state machine. The semantics of this operation is defined by the activity
 * diagram on the left-hand side of the following figure
 * (this is figure 4.3-1 in the "FW Profile Definition Document"):
 * @image html SM_StartStop.png
 * This function executes a transition from the initial pseudo-state to some state.
 * It can set the error code to #smTransErr if either of the following non-nominal
 * situations occurs while performing the transition:
 * - the transition encounters a choice pseudo-state which has no out-going transitions
 *   with a true guard; or
 * - the transition encounters a transition which has a choice pseudo-state as both source
 *   and destination of the same transition.
 * .
 * @param smDesc the descriptor of the state machine to be started.
 */
void FwSmStart(FwSmDesc_t smDesc);

/**
 * Stop a state machine. The semantics of this operation is defined by the activity
 * diagram on the right-hand side of the following figure
 * (this is figure 4.3-1 in the "FW Profile Definition Document"):
 * @image html SM_StartStop.png
 * @param smDesc the descriptor of the state machine to be started.
 */
void FwSmStop(FwSmDesc_t smDesc);

/**
 * Trigger a transition in a state machine. The semantics of this function is defined
 * by the following activity diagram (this is taken from figure 4.3-2 in the "FW Profile
 * Definition Document"):
 * @image html SM_CmdProcessing.png
 * The logic to execute a transition is implemented by the following activity
 * diagram (this is taken from figure 4.3-3 in the "FW Profile
 * Definition Document"):
 * @image html SM_TransitionExecution.png
 * This operation is recursive because a transition request is propagated to the
 * embedded state machine of the source state. The maximum depth of recursion is the
 * maximum depth of nesting of embedded state machines.
 * 
 * If there are two transitions out of the current state which are both activated by
 * this operation, the transition that will actually be taken is the one which was
 * added first to the state machine (i.e. transitions are evaluated in the order in
 * which they were added to the state machine).
 * 
 * Similarly, if the transition goes through a choice pseudo-state (CPS) and if
 * there are two transitions out of the CPS which have guards which evaluate to true,
 * the transition to be taken is the one which was added first to the state machine.
 * 
 * The FW Profile stipulates that at least one of the transitions out of a CPS
 * must have a guard evaluating to true. This constraint is not enforced by the
 * State Machine Module. If the constraint is violated, the error code is set to
 * #smTransErr.
 * 
 * This operation sets the error code to #smTransErr if either of the following non-nominal
 * situations occurs:
 * - the transition encounters a choice pseudo-state which has no out-going transitions
 *   with a true guard; or
 * - the transition encounters a transition which has a choice pseudo-state as both source
 *   and destination of the same transition.
 * .
 * In both the above cases, the state machine may remain in an inconsistent state.
 * @param smDesc the descriptor of the state machine where the transition is triggered.
 * @param transId the identifier of the transition trigger.
 */
void FwSmMakeTrans(FwSmDesc_t smDesc, FwSmCounterU2_t transId);

/**
 * Convenience method to execute a state machine. Executing a state machine is equivalent
 * to sending it the "Execute" transition command with function
 * <code>::FwSmMakeTrans</code>.
 * The identifier of the "Execute" transition command is stored in #FW_TR_EXECUTE.
 * @param smDesc the descriptor of the state machine which is executed.
 */
void FwSmExecute(FwSmDesc_t smDesc);

/**
 * Return the state machine embedded in the current state.
 * @param smDesc the descriptor of the state machine.
 * @return the descriptor of the state machine embedded in the current state
 * or NULL if there is no embedded state machine.
 */
FwSmDesc_t FwSmGetEmbSmCur(FwSmDesc_t smDesc);

/**
 * Return the state machine embedded in the i-th state of the argument
 * state machine.
 * No check is made on the legality of the value of argument 'i'.
 * If this does not correspond to an existing state in the argument state
 * machine, the behaviour of this function is undefined.
 * @param smDesc the descriptor of the state machine.
 * @param i the identifier of the state whose embedded state machine is
 * returned
 * @return the descriptor of the state machine embedded in the i-th state
 * or NULL if there is no embedded state machine in the i-th state .
 */
FwSmDesc_t FwSmGetEmbSm(FwSmDesc_t smDesc, FwSmCounterS1_t i);

/**
 * Return the identifier of the current state in a state machine (or zero
 * if the state machine is stopped).
 * @param smDesc the descriptor of the state machine.
 * @return the identifier of the current state of the state machine (or zero
 * if the state machine is stopped).
 */
FwSmCounterS1_t FwSmGetCurState(FwSmDesc_t smDesc);

/**
 * Return the identifier of the current state of the state machine
 * embedded in the current state (the sub-state).
 * If the current state is S and if there is a state machine embedded in this
 * state, then this function returns the current state of the embedded
 * state machine.
 * More precisely, this function implements the following logic:
 * - If the outer state machine is stopped, then the function returns -1.
 * - If the current state of the outer state machine has an embedded
 *   state machine, then this function returns the current state of the
 *   embedded state machine (or zero if the embedded state machine is
 *   stopped).
 * - If the current state of the outer state machine has no embedded state
 *   machine, then this function returns -1.
 * .
 * @param smDesc the descriptor of the state machine.
 * @return the identifier of the current state of the embedded state machine
 * (or zero if the embedded state machine is stopped, or -1 if the current
 * state has no embedded state machine).
 */
FwSmCounterS1_t FwSmGetCurStateEmb(FwSmDesc_t smDesc);

/**
 * Check whether the state machine is started.
 * @param smDesc the descriptor of the state machine.
 * @return 1 if the state machine is STARTED or 0 if it is STOPPED.
 */
FwSmBool_t FwSmIsStarted(FwSmDesc_t smDesc);

/**
 * Return the error code of the argument state machine.
 * The error code of a state machine holds either <code>#smSuccess</code> if the state
 * machine never encountered an error or else it holds the code of the last error
 * encountered by the state machine.
 * If the error code is different from <code>#smSuccess</code>, the behaviour of
 * the state machine is undefined.
 * @param smDesc the descriptor of the state machine to be checked.
 * @return either <code>#smSuccess</code> or the code of the last error encountered
 * by the state machine.
 */
FwSmErrCode_t FwSmGetErrCode(FwSmDesc_t smDesc);

/**
 * Return the State Machine Execution Counter.
 * The State Machine Execution Counter holds the number of execution cycles since
 * the state machine was started.
 * Note that the State Machine Execution Counter is not reset when the state
 * machine is stopped (it is only reset when it is started).
 * @param smDesc the descriptor of the state machine.
 * @return the value of the State Machine Execution Counter.
 */
FwSmCounterU3_t FwSmGetExecCnt(FwSmDesc_t smDesc);

/**
 * Return the State Execution Counter.
 * The State Execution Counter holds the number of execution cycles since
 * the current state was entered.
 * Note that the State Execution Counter is not reset when the state machine
 * is stopped (it is only reset when the state machine is started).
 * @param smDesc the descriptor of the state machine.
 * @return the value of the Node Execution Counter.
 */
FwSmCounterU3_t FwSmGetStateExecCnt(FwSmDesc_t smDesc);

#endif /* FWSM_CORE_H_ */
