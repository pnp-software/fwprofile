/**
 * @file
 * @ingroup smGroup
 * Declaration of the configuration interface for a FW State Machine.
 * A FW State Machine is described by a state machine descriptor.
 * The functions declared in this interface allow a state machine descriptor
 * to be configured.
 * During the configuration process, the states, the choice pseudo-states and
 * the transitions of the state machine are defined.
 *
 * There are two types of state machines: newly created state machines
 * (i.e. state machines which are created from scratch using <code>::FwSmCreate</code>
 * or <code>#FW_SM_INST</code>) and derived state machines (i.e. state
 * machines which are created by extending another state machine through calls
 * to <code>::FwSmCreateDer</code> or <code>#FW_SM_INST_DER</code>).
 * The functions declared in this header file can be used to configure both types
 * of state machines.
 *
 * In the case of a newly created state machine, the mode of use of the
 * functions in this header file is as follows:
 * -# The states of the state machine are added to its descriptor with
 *    the <code>::FwSmAddState</code> function.
 * -# The choice pseudo-states of the state machine are added to its
 *    descriptor with the <code>::FwSmAddChoicePseudoState</code> function.
 * -# The transitions of the state machines are added to the state machine
 *    with the <code>FwSmAddFlow*</code> functions (there are several of these
 *    functions, one for each type of transition source and destination).
 * -# The pointer to the state machine data in the state machine descriptor
 *    is set with the <code>::FwSmSetData</code> function.
 * -# The consistency and completeness of the state machine configuration may
 *    be verified with function <code>::FwSmCheck</code> or
 *    <code>::FwSmCheckRec</code>. Use of this function is optional.
 * .
 * The only constraint on the order in which the above functions are called is
 * that a transition from a state or choice pseudo-state can only be defined
 * after the source state or choice pseudo-state has been defined.
 *
 * In the case of a derived state machine, the mode of use of the functions
 * declared in this header file is as follows:
 * -# An action (either a state or a transition action) can be overridden
 *    with the <code>::FwSmOverrideAction</code> function.
 * -# A guard can be overridden with the <code>::FwSmOverrideGuard</code> function.
 * -# A state machine can be embedded in the state of the derived state
 *    machine with the <code>::FwSmEmbed</code> function.
 * -# The consistency and completeness of the configuration of the derived
 *    state machine may be verified with function <code>::FwSmCheck</code> or
 *    <code>::FwSmCheckRec</code>. Use of this function is optional.
 * .
 * There are no constraints on the order in which the above functions are
 * called.
 *
 * Error handling is done as follows:
 * - Errors are reported through error codes. The range of error codes is
 *   defined in header file <code>FwSmConstant.h</code>. The error code is
 *   stored in the state machine descriptor (see <code>::FwSmGetErrCode</code>).
 * - If an illegal input value could cause a corruption of the internal data
 *   structures of a state machine descriptor, a check is performed on the
 *   input parameter value and the function is aborted if this is found to be
 *   illegal.
 * - When a function takes a state machine descriptor as an argument, no
 *   check is performed that the argument is non-NULL and valid (e.g. if
 *   a configuration function is called with a NULL value for the state
 *   machine descriptor argument, this error is not handled and the call
 *   will probably result in a segmentation fault).
 * .
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

#ifndef FWSM_CONFIG_H_
#define FWSM_CONFIG_H_

#include "FwSmCore.h"

/**
 * Set the pointer to the state machine data in the state machine descriptor.
 * The state machine data are data which are manipulated by the state machine
 * actions and guards.
 * The state machine descriptor holds a pointer to the state machine data.
 * This function allows this pointer to be defined.
 * The exact type of the state machine data is defined by applications for each
 * state machine.
 * It is therefore unknown at the level of the FW State Machine Module.
 * This function accordingly treats the pointer to the state machine data as a
 * pointer to <code>void</code>.
 * In most cases, the state machine data will take the form of a <code>struct</code>
 * whose fields represents the inputs and outputs for the state machine actions
 * and guards.
 *
 * The functions which implement the actions and guards access the state machine
 * data as follows:
 * - The functions implementing the actions and guards of a state machine
 *   take the state machine descriptor as an argument (because they must conform
 *   to the prototype defined by <code>::FwSmAction_t</code> and
 *   <code>::FwSmGuard_t</code>).
 * - The functions implementing the actions and guards of a state machine
 *   use the <code>::FwSmGetData</code> function to retrieve the pointer to the
 *   state machine data.
 * - The pointer returned by <code>::FwSmGetData</code> is defined as a pointer
 *   to <code>void</code> and will normally have to be cast to its true type.
 * @param smDesc the descriptor of the state machine to which the data structure is attached.
 * @param smData the pointer to the state machine data.
 * A value of NULL is legal (but note that the default value of the pointer to the
 * state machine data when the state machine is created is NULL).
 */
void FwSmSetData(FwSmDesc_t smDesc, void* smData);

/**
 * Get the pointer to the state machine data in the state machine descriptor.
 * This function returns the pointer which was set with the
 * <code>::FwSmSetData</code> function.
 * This function is normally used by the functions implementing a state machine
 * action or guard to access the state machine data.
 * See the description of the <code>::FwSmSetData</code> function for more details.
 * @param smDesc the descriptor of the state machine to which the data structure is attached.
 * @return the pointer to the state machine data or NULL if no state machine data
 * were defined for the state machine.
 */
void* FwSmGetData(FwSmDesc_t smDesc);

/**
 * Create a state with the given characteristics and add it to a state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllStateId: illegal value of the state identifier
 * - #smStateIdInUse: a state with this identifier has already been defined
 * - #smNegOutTrans: the number of out-going transitions is negative
 * - #smTooManyOutTrans: the state adds too many transitions
 * - #smTooManyActions: the state adds too many actions
 * .
 * @param smDesc the state machine to which the state is added.
 * @param stateId the identifier of the state to be added
 * (an integer in the range [1,N] where N is the number of states in the state machine).
 * @param nOfOutTrans the number of out-going transitions from the new state. This must be
 * a non-negative integer. A negative value is illegal but will not be reported as an
 * error by this function.
 * @param entryAction the entry action for the new state (a function pointer) or NULL if no
 * entry action is associated to this state.
 * @param doAction the do action for the new state (a function pointer) or NULL if no
 * do action is associated to this state.
 * @param exitAction the exit action for the new state (a function pointer) or NULL if no
 * exit action is associated to this state.
 * @param esmDesc the descriptor of the state machine embedded in the new state or NULL
 * if no state machine is embedded in the new state.
 */
void FwSmAddState(FwSmDesc_t smDesc, FwSmCounterS1_t stateId, FwSmCounterS1_t nOfOutTrans, FwSmAction_t entryAction,
                  FwSmAction_t exitAction, FwSmAction_t doAction, FwSmDesc_t esmDesc);

/**
 * Create a choice pseudo-state with the given characteristics and add it to a state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllChoiceId: illegal identifier for the choice pseudo-state
 * - #smChoiceIdInUse: a choice pseudo-state with this identifier has already been defined
 * - #smIllNOfOutTrans: the number of out-going transitions is smaller than 1
 * - #smTooManyOutTrans: the choice pseudo-state adds too many out-going transitions
 * .
 * @param smDesc the descriptor of the state machine to which the choice pseudo-state is added.
 * @param choiceId the identifier of the choice pseudo-state to be added (an integer
 * in the range [1,N] where N is the number of choice pseudo-states).
 * @param nOfOutTrans the number of outgoing transitions from the new state (a positive
 * integer).
 */
void FwSmAddChoicePseudoState(FwSmDesc_t smDesc, FwSmCounterS1_t choiceId, FwSmCounterS1_t nOfOutTrans);

/**
 * Create a transition from the initial pseudo-state to a proper state and add it
 * to a state machine.
 * Transitions from the initial pseudo-state have no trigger and no guard.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param destId the identifier of the destination state of the transition (an integer in the
 * range [1,N] where N is the number of states).
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 */
void FwSmAddTransIpsToSta(FwSmDesc_t smDesc, FwSmCounterS1_t destId, FwSmAction_t trAction);

/**
 * Create a transition from the initial pseudo-state to a choice pseudo-state and add
 * it to a state machine.
 * Transitions from the initial pseudo-state have no trigger and no guard.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param destId the identifier of the destination choice pseudo-state of the transition (an
 * integer in the range [1,N] where N is the number of choice pseudo-states).
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 */
void FwSmAddTransIpsToCps(FwSmDesc_t smDesc, FwSmCounterS1_t destId, FwSmAction_t trAction);

/**
 * Create a transition from a proper state to a proper state and add it to a state machine.
 * This function should only be performed after the source state of the transition has been added
 * to the state state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllTransSrc: the identifier of the source of this transition has an illegal value
 * - #smUndefinedTransSrc: the source of this transition has not yet been added to the state machine
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyGuards: there is no space left in the guard array of the state machine
 *   descriptor for the guard defined by this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param transId the identifier of the trigger which is associated to the transition. This is an integer
 * which is unique for the state machine and for all its embedded state machines. The zero identifier is
 * reserved for the "Execute" transition.
 * @param srcId the identifier of the source state of the transition (an integer in the range
 * [1,N] where N is the number of states).
 * @param destId the identifier of the destination state of the transition.
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 * @param trGuard the transition guard (a function pointer) or NULL if no guard is associated
 * to this transition.
 */
void FwSmAddTransStaToSta(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmCounterS1_t destId,
                          FwSmAction_t trAction, FwSmGuard_t trGuard);

/**
 * Create a transition from a proper state to a choice pseudo-state and add it to a state machine.
 * This function should only be performed after the source state of the transition has been added
 * to the state state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllTransSrc: the identifier of the source of this transition has an illegal value
 * - #smUndefinedTransSrc: the source of this transition has not yet been added to the state machine
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyGuards: there is no space left in the guard array of the state machine
 *   descriptor for the guard defined by this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param transId the identifier of the trigger which is associated to the transition. This is a
 * non-negative integer but the value of zero is reserved for the "Execute" transition
 * (see #FW_TR_EXECUTE).
 * @param srcId the identifier of the source state of the transition (an integer in the range
 * [1,N] where N is the number of states)
 * @param destId the identifier of the destination choice pseudo-state of the transition (an integer
 * in the range [1,M] where M is the number of choice pseudo-states)
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 * @param trGuard the transition guard (a function pointer) or NULL if no guard is associated
 * to this transition.
 */
void FwSmAddTransStaToCps(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmCounterS1_t destId,
                          FwSmAction_t trAction, FwSmGuard_t trGuard);

/**
 * Create a transition from a choice pseudo-state to a proper state and add it to a state machine.
 * This function should only be performed after the source of the transition has been added
 * to the state state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllTransSrc: the identifier of the source of this transition has an illegal value
 * - #smUndefinedTransSrc: the source of this transition has not yet been added to the state machine
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyGuards: there is no space left in the guard array of the state machine
 *   descriptor for the guard defined by this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param srcId the identifier of the source choice pseudo-state of the transition (an integer
 * in the range [1,N] where N is the number of choice pseudo-states).
 * @param destId the identifier of the destination state of the transition (an integer in the range
 * [1,N] where N is the number of states).
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 * @param trGuard the transition guard (a function pointer) or NULL if no guard is associated
 * to this transition.
 */
void FwSmAddTransCpsToSta(FwSmDesc_t smDesc, FwSmCounterS1_t srcId, FwSmCounterS1_t destId, FwSmAction_t trAction,
                          FwSmGuard_t trGuard);

/**
 * Create a transition from a proper state to the final pseudo-state and add it to a state machine.
 * This function should only be performed after the source state of the transition has been added
 * to the state state machine.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllTransSrc: the identifier of the source of this transition has an illegal value
 * - #smUndefinedTransSrc: the source of this transition has not yet been added to the state machine
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyGuards: there is no space left in the guard array of the state machine
 *   descriptor for the guard defined by this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param transId the identifier of the trigger which is associated to the transition. This is a
 * non-negative integer but the value of zero is reserved for the "Execute" transition
 * (see #FW_TR_EXECUTE).
 * @param srcId the identifier of the source state of the transition (an integer in the range
 * [1,N] where N is the number of states).
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 * @param trGuard the transition guard (a function pointer) or NULL if no guard is associated
 * to this transition.
 */
void FwSmAddTransStaToFps(FwSmDesc_t smDesc, FwSmCounterU2_t transId, FwSmCounterS1_t srcId, FwSmAction_t trAction,
                          FwSmGuard_t trGuard);

/**
 * Create a transition from a choice pseudo-state to the final pseudo-state and add it to a state machine.
 * This function should only be performed after the source state of the transition has been added
 * to the state state machine.
 * Transitions from a choice pseudo-state have no trigger.
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smIllTransSrc: the identifier of the source of this transition has an illegal value
 * - #smUndefinedTransSrc: the source of this transition has not yet been added to the state machine
 * - #smTooManyTrans: there is no space left in the transition array of the state machine
 *   descriptor for this transition
 * - #smTooManyGuards: there is no space left in the guard array of the state machine
 *   descriptor for the guard defined by this transition
 * - #smTooManyActions: there is no space left in the action array of the state machine
 *   for the transition action defined by this transition
 * .
 * @param smDesc the descriptor of the state machine to which the transition is added.
 * @param srcId the identifier of the source state of the transition (an integer in the range
 * [1,N] where N is the number of states).
 * @param trAction the transition action (a function pointer) or NULL if no action is associated
 * to this transition.
 * @param trGuard the transition guard (a function pointer) or NULL if no guard is associated
 * to this transition
 */
void FwSmAddTransCpsToFps(FwSmDesc_t smDesc, FwSmCounterS1_t srcId, FwSmAction_t trAction, FwSmGuard_t trGuard);

/**
 * Check the correctness and completeness of the configuration of a state machine descriptor.
 * This function may be called on a state machine descriptor after it has been created and
 * configured.
 * This function does not modify the configuration of the state machine.
 * It checks the error conditions which may arise when an application configures a
 * state machine using the configuration functions declared in this header file.
 * More specifically, this function performs the following checks:
 * -# Check that no configuration errors have occurred (i.e. check that the error code
 *    in the state machine descriptor is equal to: <code>::smSuccess</code>).
 * -# Check that all states have been defined.
 * -# Check that all choice pseudo-states have been defined.
 * -# Check that all transitions have been defined.
 * -# Check that the destinations of the transitions out of a state represent legal entities.
 * -# Check that the destinations of the transitions out of a choice pseudo-state represent
 *    legal entities.
 * -# Check that the number of actions declared when the state machine was created is the same
 *    as the number of actions defined during the state machine configuration process.
 * -# Check that the number of guards declared when the state machine was created is the same
 *    as the number of guards defined during the state machine configuration process.
 * -# Check that all states are reachable (i.e. they are at the end of at least one transition).
 * -# Check that all choice pseudo-states are reachable (i.e. they are at the end of at least
 *    one transition).
 * .
 * Note that there are configuration errors which are not covered by this function because
 * they cannot occur if the state machine is configured using the functions declared in
 * this header file.
 * Examples of this kind of errors include:
 * - Defining a guard on a transition out of the initial pseudo-state;
 * - Defining a transition which has a choice pseudo-state as both source and destination
 *   of the transition.
 * .
 * @param smDesc the descriptor of the state machine to be checked.
 * @return the outcome of the check. The outcome of the check is one of the following:
 * - #smSuccess: all checks have been passed.
 * - #smConfigErr: check 1 has failed.
 * - #smNullPState: check 2 has failed.
 * - #smNullCState: check 3 has failed.
 * - #smNullTrans: check 4 has failed.
 * - #smIllegalPDest: check 5 has failed
 * - #smIllegalCDest: check 6 has failed
 * - #smTooFewActions: check 7 has failed
 * - #smTooFewGuards: check 8 has failed
 * - #smUnreachablePState: check 9 has failed
 * - #smUnreachableCState: check 10 has failed
 * .
 * This function returns when it encounters the first error. Hence, the function return value
 * is determined by the first error encountered by the function.
 */
FwSmErrCode_t FwSmCheck(FwSmDesc_t smDesc);

/**
 * Recursively check the configuration of a state machine and all its embedded state
 * machines.
 * This function performs the same checks as <code>::FwSmCheck</code> but the checks are
 * also performed (recursively) on all the embedded state machines.
 * The function returns the first error code it encounters while checking the
 * outer state machine and its embedded state machine.
 * Thus, the error reported by this function may have arisen either in the outer
 * state machine or in one of its embedded state machines.
 * @param smDesc the descriptor of the state machine to be checked.
 * @return the outcome of the check (this is the same as for <code>::FwSmCheck</code>).
 */
FwSmErrCode_t FwSmCheckRec(FwSmDesc_t smDesc);

/**
 * Override an action (either a state action or a transition action) in a derived
 * state machine.
 * By default a derived state machine has the same actions as the base state machine
 * from which it was derived.
 * This function overrides one of the actions of the derived state machine.
 *
 * As an example consider the case of a base state machine B and suppose that action
 * a1 is used as entry action of state S1 and as exit action of state S2.
 * If an application creates a derived state machine D from B (for instance, through
 * function <code>::FwSmCreateDer</code>), then, after creation, the entry action of
 * state S1 and the exit action of state S2 in state machine D is still a1.
 * This function can be used to change a1.
 * Note that a single invocation of this function will change both the entry action
 * of S1 and the exit action of S2.
 *
 * A call to this function on a state machine D has no effect on the actions of
 * the state machines which are embedded in the states of D.
 *
 * If an attempt is made to override a non-existent action, an error is declared
 * and the function returns without doing anything.
 *
 * The override mechanism is only available for derived state machines.
 * If this function is called on a state machine which was not derived by extending
 * some other state machine, an error is declared.
 *
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smUndefAction: the action to be overridden does not exist
 * - #smNotDerivedSM: the state machine is not a derived state machine
 * .
 * @param smDesc the descriptor of the derived state machine.
 * @param oldAction the action to be overridden
 * @param newAction the new action which overrides the old action
 */
void FwSmOverrideAction(FwSmDesc_t smDesc, FwSmAction_t oldAction, FwSmAction_t newAction);

/**
 * Override a guard in a derived state machine.
 * By default a derived state machine has the same guards as the base state machine
 * from which it was derived.
 * This function overrides one of the guards of the derived state machine.
 *
 * As an example consider the case of a base state machine B and suppose that
 * the transitions from state S1 to state S2 and the transition from state S3 to
 * state S4 both have the same guard g1.
 * If an application creates a derived state machine D from B (for instance, through
 * function <code>::FwSmCreateDer</code>), then, after creation, the guard of the
 * transition from S1 to S2 and from S3 to S4 is still g1.
 * This function can be used to change g1.
 * Note that a single invocation of this function will change the guard on both
 * transitions.
 *
 * A call to this function on a state machine D has no effect on the guards of
 * the state machines which are embedded in the states of D.
 *
 * If an attempt is made to override a non-existent guard, the function declares
 * an error and returns.
 *
 * The override mechanism is only available for derived state machines.
 * If this function is called on a state machine which was not derived by extending
 * some other state machine, an error is declared.
 *
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smUndefGuard: the guard to be overridden does not exist
 * - #smNotDerivedSM: the state machine is not a derived state machine
 * .
 * @param smDesc the descriptor of the derived state machine.
 * @param oldGuard the guard to be overridden
 * @param newGuard the new guard which overrides the old guard
 */
void FwSmOverrideGuard(FwSmDesc_t smDesc, FwSmGuard_t oldGuard, FwSmGuard_t newGuard);

/**
 * Embed a state machine in a state of a derived state machine.
 * By default a derived state machine has the same embedded state machines as
 * the base state machine from which it was derived.
 * This function allows a state machine to be embedded in one of the states
 * of the derived state machine.
 * A state machine can only be embedded in an "empty" state (i.e. a state which,
 * in the base state machine, had no embedded state).
 *
 * As an example consider the case of a base state machine B and suppose that
 * state S1 in this state machine has not embedded state machine.
 * If an application creates a derived state machine D from B (for instance, through
 * function <code>::FwSmCreateDer</code>), then, after creation, state S1 in the
 * derived state machine has no embedded state machine.
 * This function allows a state machine to be embedded in S1.
 *
 * If an attempt is made to embed a state machine in a non-empty state, the
 * function returns with an error but does not modify in any way the state
 * machine descriptor.
 *
 * The override mechanism is only available for derived state machines.
 * If this function is called on a base state machine, an error is returned
 * and the function returns without doing anything.
 *
 * This function reports the following errors in the error code of the state
 * machine descriptor:
 * - #smUndefGuard: the action to be overridden does not exist
 * - #smIllStateId: the state identifier is illegal
 * - #smNotDerivedSM: the state machine is a base state machine
 * - #smEsmDefined: an embedded state machine is already defined for the target
 *   state
 * .
 * @param smDesc the descriptor of the derived state machine.
 * @param stateId the identifier of the state where the state machine is to be
 * embedded.
 * @param esmDesc the descriptor of the state machine to be embedded.
 */
void FwSmEmbed(FwSmDesc_t smDesc, FwSmCounterS1_t stateId, FwSmDesc_t esmDesc);

#endif /* FWSM_CONFIG_H_ */
