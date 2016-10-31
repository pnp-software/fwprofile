/**
 * @file
 * @ingroup tsSmGroup
 * Declaration of functions to create and configure test state machines.
 * The test state machines are used in the <i>test suite</i> to verify
 * the requirements of the FW State Machine Module.
 * 
 * The following acronyms are used in this file:
 * - IPS = Initial Pseudo-State
 * - CPS = Choice Pseudo-State
 * - FPS = Final Pseudo-State
 * - ESM = Embedded State Machine
 * .
 * The data for the state machines defined in this file are instances of
 * a data structure of type <code>::TestSmData</code>.
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

#ifndef FWSM_MAKETESTSM_H_
#define FWSM_MAKETESTSM_H_

#include "FwSmCore.h"

/** A state identifier */
#define STATE_S1 1
/** A state identifier */
#define STATE_S2 2
/** A state identifier */
#define STATE_S3 3
/** A state identifier */
#define STATE_S4 4
/** A choice pseudo-state identifier */
#define CPS1 1
/** A choice pseudo-state identifier */
#define CPS2 2
/** A transition identifier */
#define TR_S1_FPS 2
/** A transition identifier */
#define TR_IPS_CPS1 1
/** A transition identifier */
#define TR_CPS1_S1 2
/** A transition identifier */
#define TR_CPS1_FPS 3
/** A transition identifier */
#define TR1 30
/** A transition identifier */
#define TR2 12
/** A transition identifier */
#define TR3 13
/** A transition identifier */
#define TR4 14
/** A transition identifier */
#define TR5 15
/** A transition identifier */
#define TR6 20
/** A transition identifier */
#define TR_CPS_S1 1
/** A transition identifier */
#define TR_CPS_S2 1

/**
 * Define size of the log arrays.
 * The log arrays are static arrays to which the actions of the test state machines write
 * various outputs. They are used to check which actions have been executed and in which
 * sequence.
 */
#define LOG_ARRAY_SIZE 50

/**
 * Type for the state machine data for the test state machines.
 * This data structure defines <i>counters</i> and <i>flags</i>.
 * The counters are incremented by the state machine actions and the flags are used to
 * determine the outcome of the state machine guards.
 * 
 * The actions attached to the test state machines write a "marker" and the current state of
 * their state machine to two log arrays (<code>logMarker</code> and <code>logState</code>).
 * 
 * Thus, when a state action is executed it does the following:
 * -# it writes: <code>(marker+logBase)</code> at <code>logMarker[logIndex]</code>;
 * -# it writes: <code>(currentState+logBase)</code> at <code>logState[logIndex]</code>; and
 * -# it increments <code>logIndex</code> by 1.
 * 
 * The values of the markers are: 1 for the entry action; 2 for the do-action; 3 for the exit
 * action; and 4 for the transition action.
 * 
 * The log arrays are common to all test state machines. In order to allow the entries made
 * by different state machines to be distinguished, the <code>logBase</code> field provides
 * an offset which is specific to each state machine.
 */
struct TestSmData {
	/** Counter incremented by the state actions. */
	int counter_1;
	/** Counter incremented by the transition actions. */
	int counter_2;
	/** Flag checked by the transition guards. */
	int flag_1;
	/** Flag checked by the transition guards. */
	int flag_2;
	/** Flag checked by the transition guards. */
	int flag_3;
	/** Offset for markers written to log array. */
	int logBase;
};

/**
 * Operation to create and configure the state machine SM1.
 * This state machine has the following characteristics (see also figure below):
 * - One single state S1.
 * - No choice pseudo-states.
 * - Transition from the IPS to S1.
 * - Transition from S1 to the FPS with transition trigger TR_S1_FPS.
 * - The entry action of S1 increments <code>counter_1</code> by 1.
 * - The do action of S1 increments counter <code>counter_1</code> by 2.
 * - There is no exit action in S1.
 * - The transition actions increment counter <code>counter_2</code> by 1.
 * - The transition guard on the transition from S1 to the FPS returns the
 *   value of <code>flag_1</code>.
 * .
 * @image html SM1.png
 * @param smData the data structure upon which the state machine operates.
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM1(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM1 (see <code>::FwSmMakeTestSM1</code>
 * and figure below) statically using the interface defined in <code>FwSmSCreate.h</code>.
 * @image html SM1.png
 * @param smData the data structure upon which the state machine operates.
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM1Static(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM2.
 * This state machine has the following characteristics:
 * - One single state S1.
 * - One single choice pseudo-state CPS1.
 * - Four transitions:
 *   -# from IPS to CPS1,
 *   -# from CPS1 to S1,
 *   -# from CPS1 to FPS,
 *   -# from S1 to FPS (trigger: TR1_S1_FPS).
 * - State entry actions increment counter <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * - Guard on transition (2) returns the value of <code>flag_1</code>.
 * - Guard on transition (3) returns the value of <code>flag_2</code>.
 * - Guard on transition (4) returns the value of <code>flag_2</code>.
 * .
 * @image html SM2.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM2(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM3.
 * This state machine is the same as SM1 (see <code>::FwSmMakeTestSM1</code>) but it
 * has SM2 (see <code>::FwSmMakeTestSM2</code>) embedded within state S1 of SM1.
 * @param smData the data structure upon which the embedding state machine operates
 * @param esmData the data structure upon which the embedded state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM3(struct TestSmData* smData, struct TestSmData* esmData);

/**
 * Operation to create and configure the state machine SM4.
 * This state machine has the following characteristics (see also figure below):
 * - Two states S1 and S2.
 * - No choice pseudo-states.
 * - Transitions as follows:
 *   -# Transition from IPS to S1;
 *   -# "Execute" transitions from S1 to S2;
 *   -# "Execute" transition from S2 to S1;
 *   -# TR6 transition from S2 to S1;
 *   -# TR3 transition from S2 to FPS;
 *   -# TR4 self-transition on S2.
 *   .
 * - State entry actions increment <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * - Transition guards return the value of <code>flag_1</code>.
 * .
 * @image html SM4.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM4(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM5.
 * This state machine has the following characteristics:
 * - Two states S1 and S2.
 * - One choice pseudo-state CPS1.
 * - Transitions as follows:
 *   -# transition from IPS to S1;
 *   -# TR2 transitions from S1 to S2;
 *   -# TR6 transition from S2 to CPS1;
 *   -# transition from CPS1 to S1 with <code>flag_1</code> as guard;
 *   -# transition from CPS1 to S2 with <code>flag_2</code> as guard;
 *   -# TR5 transition from S2 to FPS;
 *   -# TR4 self-transition on S2.
 *   .
 * - State entry actions increment <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * - Transition guards return the value of <code>flag_1</code> with the exception of the transition
 * from S2 to CPS1 which has no guard and the transition from CPS1 to S2 which returns the
 * value of <code>flag_2</code>.
 * .
 * @image html SM5.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM5(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM5 (see <code>::FwSmMakeTestSM5</code>
 * and figure below) statically using the interface defined in <code>FwSmSCreate.h</code>.
 * @image html SM5.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine.
 */
FwSmDesc_t FwSmMakeTestSM5Static(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM5 (see <code>::FwSmMakeTestSM5</code>
 * and figure below) by directly instantiating and initializing the internal data structures
 * in the state machine descriptor.
 * The data structures for the state machine are created as <code>static</code> variables and are
 * initialized directly (i.e. without using the creation functions offered by the
 * <code>FwSmDCreate.h</code> or <code>FwSmSCreate</code> moduls and the configuration functions
 * offered by <code>FwSmConfig.h</code>).
 * 
 * The approach used in this function to initialize the state machine descriptor is non-nominal (normally
 * the state machine descriptor is created and configured using the operations made available
 * by the FW State Machine Module).
 * Applications which use this approach to create and initialize their state machine descriptors
 * will not need to link to the creation and configuration operations offered by the
 * FW State Machine Module and will therefore reduce the memory footprint of their
 * application.
 * @image html SM5.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine.
 */
FwSmDesc_t FwSmMakeTestSM5Dir(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM6.
 * This state machine is the same as state machine SM4 (see <code>::FwSmMakeTestSM4</code>)
 * with state machine SM5 (see <code>::FwSmMakeTestSM5</code>) embedded within its
 * state S2.
 * @param smData the data structure upon which the state machine operates
 * @param esmData the data structure upon which the embedded state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM6(struct TestSmData* smData, struct TestSmData* esmData);

/**
 * Operation to create and configure the state machine SM7.
 * This state machine has the following characteristics (see also figure
 * below):
 * - One state S1.
 * - One choice pseudo-state CPS1.
 * - Transitions as follows:
 *   -# transition from IPS to S1;
 *   -# TR1 transitions from S1 to CPS1 with no guard;
 *   -# transition from CPS1 to S1 with <code>flag_1</code> as guard;
 *   -# transition from CPS1 to FPS with <code>flag_2</code> as guard;
 *   .
 * - No entry action.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * - Transition guards return the value of either <code>flag_1</code> or
 *   <code>flag_2</code> as stated above.
 * .
 * @image html SM7.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM7(struct TestSmData* smData);

/**
 * Operation to create and configure a degenerate state machine with no states.
 * This state machine has the following characteristics (see also figure
 * below):
 * - No state.
 * - One choice pseudo-state CPS1.
 * - Transitions as follows:
 *   -# transition from IPS to CPS1;
 *   -# transition from CPS1 to FPS with <code>flag_1</code> as guard;
 *   -# transition from CPS1 to FPS with <code>flag_2</code> as guard;
 *   .
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * .
 * @image html SM8.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM8(struct TestSmData* smData);

/**
 * Operation to create and configure a degenerate state machine with no transitions
 * other than the initial transition.
 * This state machine has the following characteristics (see also figure
 * below):
 * - One state S1.
 * - No choice pseudo-states.
 * - Transition from initial pseudo-state to S1
 * - State entry actions increment <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * .
 * @image html images/SM9.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM9(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM5 (see <code>::FwSmMakeTestSM9</code>
 * and figure below) statically using the interface defined in <code>FwSmSCreate.h</code>.
 * @image html images/SM9.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM9Static(struct TestSmData* smData);

/**
 * Operation to create and configure the state machine SM10.
 * This state machine has the following characteristics (see also figure
 * below):
 * - Two states S1 and S2.
 * - State machine SM1 (see <code>::FwSmMakeTestSM1</code>) is embedded in S2.
 * - One choice pseudo-state CPS1.
 * - Transitions as follows:
 *   -# transition from IPS to S1;
 *   -# TR1 transitions from S1 to CPS1;
 *   -# transition from CPS1 to S2 with <code>flag_1</code> as guard;
 *   -# transition from CPS1 to FPS with <code>flag_2</code> as guard;
 *   .
 * - State entry actions increment <code>counter_1</code> by 1.
 * - No do actions.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * .
 * @image html images/SM10.png
 * @param smData the data structure upon which the state machine operates
 * @param esmData the data structure upon which the embedded state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM10(struct TestSmData* smData, struct TestSmData* esmData);

/**
 * Operation to create and configure a degenerate state machine with neither states
 * nor choice pseudo-states.
 * This state machine has the following characteristics (see also figure
 * below):
 * - No state.
 * - No choice pseudo-states.
 * - Transitions as follows:
 *   -# transition from IPS to FPS.
 *   .
 * - Transition action increments counter <code>counter_2</code> by 1.
 * .
 * The state machine is created by directly instantiating its internal data structures.
 * @image html images/SM11.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM11(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine where a transition has a
 * choice pseudo-state as both source and destination. Execution of the transition
 * with a choice pseudo-state as both source and destination causes an error.
 * This state machine has the following characteristics:
 * - One single state S1.
 * - One single choice pseudo-state CPS1.
 * - Four transitions:
 *   -# from IPS to CPS1,
 *   -# from CPS1 to CPS1 (illegal transition),
 *   -# from CPS1 to FPS,
 *   -# from S1 to FPS.
 * - State entry actions increment counter <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - Transition actions increment counter <code>counter_2</code> by 1.
 * - Guard on transition (2) returns the value of <code>flag_1</code>.
 * - Guard on transition (3) returns the value of <code>flag_2</code>.
 * - Guard on transition (4) returns the value of <code>flag_2</code>.
 * .
 * The transition from CPS1 to CPS1 is partly configured "by hand" by directly
 * assigning values to the internal data structures of the state machine
 * descriptor (this is an illegal transition and it is impossible to define
 * it using the standard configuration operations of <code>FwSmConfig.h</code>).
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM12(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine where the 'execute'
 * transition has a choice pseudo-state and the final pseudo-state as
 * destination.
 * This state machine has the following characteristics (see also figure
 * below):
 * - One single state S1.
 * - One single choice pseudo-state CPS1.
 * - Five transitions:
 *   -# from IPS to S1
 *   -# from S1 to CPS1 ("Execute" transition)
 *   -# from CPS1 to S1
 *   -# from CPS1 to FPS
 *   -# from S1 to FPS ("Execute" transition)
 *   .
 * - State entry actions increment counter <code>counter_1</code> by 1.
 * - State do actions increment counter <code>counter_1</code> by 2.
 * - State exit actions increment counter <code>counter_1</code> by 4.
 * - No transition actions.
 * - No guard on transition (1).
 * - Guard on transition (2) returns the value of <code>flag_1</code>.
 * - Guard on transition (3) returns the value of <code>flag_1</code>.
 * - Guard on transition (4) returns the value of <code>flag_2</code>.
 * - Guard on transition (5) returns the value of <code>flag_2</code>.
 * .
 * @image html images/SM13.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM13(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine where the choice
 * pseudo-state is used both to split a transition out of a state and
 * to connect two transitions from two different states ("junction").
 * This state machine has the following characteristics (see also figure
 * below):
 * - Two states S1 and S2.
 * - Two choice pseudo-state CPS1 and CPS2.
 * - Six transitions:
 *   -# from IPS to CPS1
 *   -# from CPS1 to S1
 *   -# from CPS1 to S2
 *   -# from S1 to CPS2 (transition trigger TR1)
 *   -# from S2 to CPS2 (transition trigger TR2)
 *   -# from CPS2 to FPS
 *   .
 * - No state actions.
 * - Transition actions increment counter <code>counter_2</code> by 4.
 * - No guard on transitions (1), (4), (5) and (6).
 * - Guard on transition (2) returns the value of <code>flag_1</code>.
 * - Guard on transition (3) returns the value of <code>flag_2</code>.
 * .
 * @image html images/SM14.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM14(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine which uses the
 * execution counters.
 * The value of the execution counters is stored in the procedure
 * data by the state machine actions and the value of the state
 * execution counter is used as a guard to control state transitions.
 * 
 * This state machine has the following characteristics (see also figure
 * below):
 * - Two states S1 and S2.
 * - One choice pseudo-state CPS1.
 * - Five transitions:
 *   -# from IPS to S1
 *   -# from S1 to CPS1 (Execute transition trigger)
 *   -# from CPS1 to FPS
 *   -# from CPS1 to S2
 *   -# from S2 to S1 (transition trigger TR1)
 *   .
 * - All states have an initial action which writes the state machine
 *   execution counter and the state execution counter to, respectively,
 *   counter_1 and counter_2 in the state machine data structure.
 * - No transition actions.
 * - Guard on transition (2) waits 1 execution cycle.
 * - Guard on transition (3) returns true if the SM Execution Counter is
 *   equal to 5.
 * - Guard on transition (4) returns true if the SM Execution Counter is
 *   smaller than 5.
 * - Guard on transition (5) waits two execution cycles.
 * .
 * @image html images/SM15.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM15(struct TestSmData* smData);

/**
 * Create a derived state machine and override one of its actions and
 * one of its guards.
 * The base state machine is passed as an argument to the function.
 * The function extends the base state machine and then it performs the
 * following overrides on the derived state machine:
 * - The action <code>incrCnt1By1</code> (increment Counter_1 by 1) is overridden
 *   with action <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - The guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The derived state machine is created using the dynamic creation
 * interface of <code>FwSmDCreate.h</code>.
 * @param smDescBase the base state machine
 * @param smData the data structure for the newly created state machine
 * @return the derived state machine
 */
FwSmDesc_t FwSmMakeTestSMDer1(FwSmDesc_t smDescBase, struct TestSmData* smData);

/**
 * Create a derived state machine and override one of its actions and
 * one of its guards.
 * The base state machine is passed as an argument to the function.
 * This function assumes that the base state machine has 1 state, 3
 * actions, and 1 guard.
 * The function extends the base state machine and then it performs the
 * following overrides on the derived state machine:
 * - The action <code>incrCnt1By1</code> (increment Counter_1 by 1) is overridden
 *   with action <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - The guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The derived state machine is created using the static creation
 * interface of <code>FwSmSCreate.h</code>.
 * @param smDescBase the base state machine
 * @param smData the data structure for the newly created state machine
 * @return the derived state machine
 */
FwSmDesc_t FwSmMakeTestSMDer1Static(FwSmDesc_t smDescBase, struct TestSmData* smData);

/**
 * Operation to create and configure a state machine to verify the order of
 * evaluation of guards.
 *
 * This state machine has the following characteristics (see also figure
 * below):
 * - Four states S1 to S4.
 * - One choice pseudo-state CPS1.
 * - Eight transitions:
 *   -# from IPS to S1
 *   -# from S1 to S2 (TR1 transition trigger and Flag_1 guard)
 *   -# from S1 to S3 (TR1 transition trigger and Flag_2 guard)
 *   -# from S1 to S4 (TR1 transition trigger and Flag_3 guard)
 *   -# from S1 to CPS1 (TR2 transition trigger)
 *   -# from CPS1 to S2 (Flag_1 guard)
 *   -# from CPS1 to S3 (Flag_2 guard)
 *   -# from CPS1 to S4 (Flag_3 guard)
 *   .
 * - Neither state nor transition actions.
 * .
 * Transitions are added to the state machine during the configuration process
 * in the order in which they are listed.
 * @image html images/SM16.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM16_1(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine to verify the order of
 * evaluation of guards.
 *
 * This state machine has the following characteristics (see also figure
 * below):
 * - Four states S1 to S4.
 * - One choice pseudo-state CPS1.
 * - Eight transitions:
 *   -# from IPS to S1
 *   -# from S1 to S4 (TR1 transition trigger and Flag_3 guard)
 *   -# from S1 to S3 (TR1 transition trigger and Flag_2 guard)
 *   -# from S1 to S2 (TR1 transition trigger and Flag_1 guard)
 *   -# from S1 to CPS1 (TR2 transition trigger)
 *   -# from CPS1 to S4 (Flag_3 guard)
 *   -# from CPS1 to S3 (Flag_2 guard)
 *   -# from CPS1 to S2 (Flag_1 guard)
 *   .
 * - Neither state nor transition actions.
 * .
 * Transitions are added to the state machine during the configuration process
 * in the order in which they are listed.
 * @image html images/SM16.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM16_2(struct TestSmData* smData);

/**
 * Operation to create and configure a state machine to verify the order of
 * evaluation of guards.
 *
 * This state machine has the following characteristics (see also figure
 * below):
 * - Four states S1 to S4.
 * - One choice pseudo-state CPS1.
 * - Eight transitions:
 *   -# from IPS to S1
 *   -# from S1 to S3 (TR1 transition trigger and Flag_2 guard)
 *   -# from S1 to S4 (TR1 transition trigger and Flag_3 guard)
 *   -# from S1 to S2 (TR1 transition trigger and Flag_1 guard)
 *   -# from S1 to CPS1 (TR2 transition trigger)
 *   -# from CPS1 to S3 (Flag_2 guard)
 *   -# from CPS1 to S4 (Flag_3 guard)
 *   -# from CPS1 to S2 (Flag_1 guard)
 *   .
 * - Neither state nor transition actions.
 * .
 * Transitions are added to the state machine during the configuration process
 * in the order in which they are listed.
 * @image html images/SM16.png
 * @param smData the data structure upon which the state machine operates
 * @return the descriptor of the created state machine or NULL if the creation
 * or configuration of the state machine failed.
 */
FwSmDesc_t FwSmMakeTestSM16_3(struct TestSmData* smData);



#endif /* FWSM_MAKETESTSM_H_ */
