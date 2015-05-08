/**
 * @file
 * @ingroup smGroup
 * Declaration of the internal data structures of the FW State Machine
 * Module.
 * Users should not normally be concerned with these data structures.
 * 
 * The data structures declared in this header file are used to define the state
 * machine descriptor.
 * A state machine descriptor holds all the information related to a certain state
 * machine.
 * A state machine descriptor consists of two parts: the base descriptor and
 * the extension descriptor.
 * 
 * The base descriptor holds the information which is not changed when the state
 * machine is extended.
 * This consists of:
 * - The list of states in the state machine
 * - The list of choice pseudo-states in the state machine
 * - The list of transitions in the state machine
 * .
 * 
 * The extension descriptor holds the information which may be overridden when the
 * state machine is extended.
 * This consists of:
 * - The list of actions used in the state machine (both the state actions and the
 *   transition actions)
 * - The list of transition guards used in the state machine
 * - The list of state machines embedded in the state machine
 * - The pointer to the state machine data (the data upon which the
 *   state machine actions and guards operate)
 * - The current state of the state machine
 * - The execution counters of the state machine
 * - The error code for the state machine
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

#ifndef FWSM_PRIVATE_H_
#define FWSM_PRIVATE_H_

#include "FwSmConstants.h"

/**
 * Enumerated type for the type of a state in a state machine.
 */
typedef enum {
	/**
	 * A proper state in a state machine.
	 */
	properState = 1,
	/**
	 * A choice pseudo-state in a state machine.
	 */
	choicePseudoState = 2,
	/**
	 * Either the initial or the final pseudo-state.
	 */
	stoppedState = 3
} StateType_t;

/**
 * Dummy action which returns without doing anything.
 * This action is used where nothing is specified for a state machine action (i.e. a
 * state entry action, or a state do-action, or a state exit action, or a transition
 * action).
 * @param smDesc state machine descriptor. This parameter is unused in this dummy action.
 * The parameter is retained for compatibility with the <code>::FwSmAction_t</code> type.
 */
void SmDummyAction(FwSmDesc_t smDesc);

/**
 * Dummy guard which always returns true.
 * This guard is used where no transition guard is specified.
 * @param smDesc state machine descriptor. This parameter is unused in this dummy guard.
 * The parameter is retained for compatibility with the <code>::FwSmGuard_t</code> type.
 * @return always return 1 (to signify that the guard is true).
 */
FwSmBool_t SmDummyGuard(FwSmDesc_t smDesc);

/**
 * Structure representing a proper state in state machine. A proper state is characterized by:
 * - the set of out-going transitions from the state
 * - the state machine embedded in the state
 * - the entry action for the state
 * - the do-action for the state
 * - the exit action for the state
 * .
 * The first item - the set of out-going transitions from the state - is defined as
 * follows.
 * Transitions which originate from the same state are located in adjacent locations in
 * the transition array of the base descriptor (see <code>::SmBaseDesc_t</code>).
 * Field <code>outTransIndex</code> identifies the first location of this set of adjacent
 * locations.
 * Thus, for instance, if a state has 3 out-going transitions and if its
 * <code>outTransIndex</code> field has value 4, then the three out-going transitions
 * are located in elements 4, 5 and 6 of the array of transitions.
 * 
 * The state actions are integers which identify locations in the action array
 * of the state machine descriptor where the action is stored.
 * Thus, for instance, if the <code>iEntryAction</code> field for state S is equal to 5,
 * then the entry action of state S is the one stored at location 5 of the
 * action array of that state machine.
 * If an action is not defined, a value of zero is used to point to the first
 * location of the action array where the dummy action <code>::SmDummyAction</code> is stored
 * which returns without doing anything.
 * 
 * By convention, the implementation treats a state as uninitialized if its
 * <code>outTransIndex</code> field is equal to zero.
 */
typedef struct {
	/** index of first out-going transition in the transition array of <code>::SmBaseDesc_t</code> */
	FwSmCounterS1_t outTransIndex;
	/** number of outgoing transitions */
	FwSmCounterS1_t nOfOutTrans;
	/** the entry action for the state */
	FwSmCounterS1_t iEntryAction;
	/** the do action for the state */
	FwSmCounterS1_t iDoAction;
	/** the exit action for the state */
	FwSmCounterS1_t iExitAction;
} SmPState_t;

/**
 * Structure representing a choice pseudo state in a state machine.
 * A choice pseudo-state is characterized by the set of its out-going transitions.
 * The set of out-going transitions is defined as follows.
 * Transitions which originate from the same choice pseudo-state are located in adjacent locations in
 * the transition array of the base descriptor (see <code>::SmBaseDesc_t</code>).
 * Field <code>outTransIndex</code> identifies the first location of this set of adjacent
 * locations.
 * Thus, for instance, if a choice pseudo-state has 3 out-going transitions and if its
 * <code>outTransIndex</code> field has value 4, then the three out-going transitions
 * are located in elements 4, 5 and 6 of the array of transitions.
 * 
 * By convention, the implementation treats a choice pseudo-state as uninitialized if its
 * <code>outTransIndex</code> field is equal to zero.
 */
typedef struct {
	/** index of first out-going transition in transition array of <code>::SmBaseDesc_t</code> */
	FwSmCounterS1_t outTransIndex;
	/** number of outgoing transitions from the choice pseudo-state */
	FwSmCounterS1_t nOfOutTrans;
} SmCState_t;

/**
 * Structure representing a transition.
 * A transition is characterized by:
 * - the destination of the transition
 * - the name of the transition
 * - the action associated to the transition
 * - the guard associated to the transition
 * .
 * The destination of the transition may be either a proper state, or the final pseudo-state,
 * or a choice pseudo-state. The type of the transition destination is identified as follows:
 * - a value of zero means that the destination is the final pseudo-state
 * - a positive value means that the destination is a proper state and the value of <code>dest</code>
 *   is the identifier of the target state
 * - a negative value means that the destination is a choice pseudo state and the value of
 *   <code>-dest</code> is the identifier of the choice pseudo-state
 * .
 * The name of a transition is an non-negative integer.
 * The value of zero is reserved for the pre-defined "Execute" transition (see
 * #FW_TR_EXECUTE) and should not be used for user-defined transitions.
 * The name of a transition is "don't care" in the case of transitions out of
 * pseudo-states.
 * 
 * The transition action is an integer which identifies the location in the
 * action array of the state machine descriptor (see <code>FwSmDesc</code>)
 * where the action is stored as a function pointer.
 * If no action is associated to the transition, a value of zero is used to
 * point to the first element of the action array which holds a dummy action
 * which returns without doing anything.
 * 
 * The guard is an integer which identifies the location in the
 * guard array of the state machine descriptor (see <code>FwSmDesc</code>)
 * where the guard is stored as a function pointer.
 * If no guard is associated to the transition, a value of zero is used to
 * point to the first element of the guard array which holds a dummy guard
 * which always returns "true".
 * 
 * By convention, the implementation treats a transition as uninitialized if its
 * <code>trAction</code> field is equal to -1.
 */
typedef struct {
	/** the index of the destination of the transition */
	FwSmCounterS1_t dest;
	/** the identifier (the "name") of the transition */
	FwSmCounterU2_t id;
	/** the index of the action associated to the transition */
	FwSmCounterS1_t iTrAction;
	/** the index of the guard associated to the transition */
	FwSmCounterS1_t iTrGuard;
} SmTrans_t;

/**
 * Structure representing the base descriptor of a state machine.
 * The base descriptor holds the information which is not changed when the state
 * machine is extended.
 * This consists of:
 * - The list of states in the state machine (array <code>pStates</code>)
 * - The list of choice pseudo-states in the state machine (array <code>cStates</code>)
 * - The list of transitions in the state machine (array <code>trans</code>)
 * .
 * Array <code>pStates</code> holds the proper states in the state machine.
 * The proper states are identified by an integer in the range [1,N] (N is the total
 * number of proper states).
 * The i-th state is stored in the (i-1)-th location of <code>pStates</code>.
 * The number of states is stored in field <code>nOfPStates</code>.
 * 
 * Array <code>cStates</code> holds the choice pseudo-states in the state machine.
 * The choice pseudo-states are identified by an integer in the range [1,M] (M is the total
 * number of choice pseudo-states).
 * The i-th choice pseudo-state is stored in the (i-1)-th location of <code>cStates</code>.
 * The number of choice pseudo-states is stored in field <code>nOfCStates</code>.
 * 
 * Array <code>trans</code> holds the transitions in the state machine.
 * The transition out of the initial pseudo-state is stored in the first location of
 * array <code>trans</code>.
 * The other transitions are stored in groups of adjacent locations where each group
 * holds the transitions out of the same state or choice pseudo-state (see also
 * <code>::SmPState_t</code> and <code>::SmPState_t</code>).
 * The number of transitions is stored in field <code>nOfTrans</code>.
 */
typedef struct {
	/** array holding the proper states in the state machine */
	SmPState_t* pStates;
	/** array holding the choice pseudo-states in the state machine */
	SmCState_t* cStates;
	/** array holding the transitions in the state machine */
	SmTrans_t* trans;
	/** the number of states in the state machine */
	FwSmCounterS1_t nOfPStates;
	/** the number of choice pseudo-states in the state machine */
	FwSmCounterS1_t nOfCStates;
	/** the number of transitions in SM */
	FwSmCounterS1_t nOfTrans;
} SmBaseDesc_t;

/**
 * Structure representing a state machine descriptor.
 * Field <code>smBase</code> points to the base descriptor for the state machine
 * which holds the information about the states, choice pseudo-states and the
 * transitions connecting them.
 * 
 * Array <code>smActions</code> holds the list of all actions in the state machine
 * (including both state and transition actions).
 * The first element of the array holds a dummy action which returns without
 * doing anything.
 * Each distinct action only appears once in the action array.
 * If the same action is used several times in a state machine, only one instance is
 * registered in the action array.
 * 
 * Array <code>smGuards</code> holds the list of all transition guards in the
 * state machine.
 * The first element of the array holds a dummy guard which always returns true.
 * Each distinct guard only appears once in the guard array.
 * If the same guard is used several times in a state machine, only one instance is
 * registered in the guard array.
 * 
 * The i-th element of array <code>esmDesc</code> holds the state machine embedded
 * in the i-th state of the state machine (or NULL if no state machine is embedded).
 * 
 * When a new state or choice pseudo-state is added to the state machine, field
 * <code>transCnt</code> holds the position in the transition array where its
 * first out-going transition will be stored.
 * 
 * The identifier of the current state (which can only be a proper state) is
 * stored in <code>curState</code>.
 * If this is equal to zero, then the state machine is stopped.
 * 
 * If during the creation, configuration or execution of the state machine, an error is
 * encountered, the corresponding error code is stored in field <code>errCode</code>.
 * This field is initialized to <code>#smSuccess</code> and should nominally remain
 * unchanged throughout the life of the state machine.
 * If the error code has a value other than <code>#smSuccess</code>, the behaviour of
 * the state machine is undefined.
 * 
 * There are two types of state machines: base state machines (i.e.
 * state machines which are created from scratch using <code>::FwSmCreate</code> or
 * <code>#FW_SM_INST</code>) and derived state machines (i.e. state
 * machines which are created by extending a base state machine through calls
 * to <code>::FwSmCreateDer</code> or <code>#FW_SM_INST_DER</code>).
 * By convention, a derived state machine is characterized by field
 * <code>transCnt</code> being equal to zero.
 * 
 * Two counters are associated to a sttae machine: the State Machine Execution Counter
 * and the State Execution Counter.
 * The State Machine Execution Counter holds the number of execution cycles since
 * the state machine was started and the State Execution Counter holds the number of cycle
 * since the current state was entered.
 */
struct FwSmDesc {
	/** pointer to the base descriptor */
	SmBaseDesc_t* smBase;
	/** the state machine actions (state and transition actions) */
	FwSmAction_t* smActions;
	/** the transition guards in the state machine */
	FwSmGuard_t* smGuards;
	/** the state machines embedded in the state machine */
	struct FwSmDesc** esmDesc;
	/** the number of actions (state actions + transition actions) in the state machine */
	FwSmCounterS1_t nOfActions;
	/** the number of guards in the state machine */
	FwSmCounterS1_t nOfGuards;
	/** the counter for the number of transitions added to the state machine */
	FwSmCounterS1_t transCnt;
	/** the current state of the state machine */
	FwSmCounterS1_t curState;
	/** the state machine execution counter */
	FwSmCounterU3_t smExecCnt;
	/** the state execution counter */
	FwSmCounterU3_t stateExecCnt;
	/** either 'success' or the code of the last error encountered by the state machine */
	FwSmErrCode_t errCode;
	/** the pointer to the data manipulated by the state machine actions and guards */
	void* smData;
};

#endif /* FWSM_PRIVATE_H_ */
