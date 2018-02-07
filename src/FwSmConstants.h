/**
 * @file
 * @ingroup smGroup
 * Header file to define all constants and types for the state machine
 * module of the FW Profile.
 * This header file should be included by all applications which use the
 * state machine module of the FW Profile.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2011, All Rights Reserved
 *
 * This file is part of the FW Profile.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#ifndef FWSM_CONSTANTS_H_
#define FWSM_CONSTANTS_H_

/**
 * Forward declaration for the pointer to a state machine descriptor.
 * A state machine descriptor is a data structure which holds all the information
 * describing a state machine instance.
 * Users only manipulate the pointer to the state machine descriptor. The internal
 * definition of the state machine descriptor (see <code>FwSmPrivate.h</code>) is
 * kept hidden from users.
 */
typedef struct FwSmDesc* FwSmDesc_t;

/**
 * Type for a pointer to a state machine action.
 * A state machine action is a function which encapsulates one of the following:
 * the entry action of a state, the exit action of a state, the do-action of a state,
 * a transition action.
 * A state machine action takes the descriptor of the state machine as an argument.
 *
 * Pointers to state machine actions are used when a new state machine is defined:
 * all the actions in the state machine must be defined as functions which conform
 * to the <code>FwSmAction_t</code> prototype and are loaded into the state machine
 * as function pointers.
 */
typedef void (*FwSmAction_t)(FwSmDesc_t);

/** Type used for booleans (0 is "false" and 1 is "true"). */
typedef int FwSmBool_t;

/**
 * Type for a pointer to a state machine guard.
 * A state machine guard is a function which encapsulates a transition guard in a
 * state machine.
 * The state machine guard takes the descriptor of the state machine as an argument
 * and it returns 0 to signify that the guard is false or a non-zero value
 * (typically 1) to signify that the guard is true.
 *
 * Pointers to state machine guards are used when a new state machine is defined:
 * all the guards in the state machine must be defined as functions which conform
 * to the <code>FwSmGuard_t</code> prototype and are loaded into the state machine
 * as function pointers.
 */
typedef FwSmBool_t (*FwSmGuard_t)(FwSmDesc_t);

/** Type used for unsigned counters with a "short" range. */
typedef unsigned char FwSmCounterU1_t;

/** Type used for unsigned counters with a "medium" range. */
typedef unsigned short int FwSmCounterU2_t;

/** Type used for unsigned counters with a "long" range. */
typedef unsigned int FwSmCounterU3_t;

/** Type used for unsigned counters with a "long int" range. */
typedef long unsigned int FwSmCounterU4_t;

/** Type used for signed counters with a "short" range. */
typedef signed char FwSmCounterS1_t;

/** Error codes and function return codes for the state machine functions. */
typedef enum {
  /**
   * Return codes of a function which has completed execution without errors.
   */
  smSuccess = 1,
  /**
   * A call to <code>malloc</code> has failed (it has returned a NULL pointer).
   */
  smOutOfMemory = 2,
  /**
   * There is an undefined state in a state machine.
   */
  smNullPState = 9,
  /**
   * There is an undefined choice pseudo-state in a state machine.
   */
  smNullCState = 10,
  /**
   * There is an undefined transition in a state machine.
   */
  smNullTrans = 11,
  /**
   * A configuration error has been detected during the state machine configuration process.
   */
  smConfigErr = 16,
  /**
   * A state is added to a state machine with an illegal (out-of-range) identifier.
   */
  smIllStateId = 22,
  /**
   * A choice pseudo-state is added to a state machine with an illegal (out-of-range) identifier.
   */
  smIllChoiceId = 23,
  /**
   * A state is added twice to the same state machine.
   */
  smStateIdInUse = 26,
  /**
   * A choice pseudo-state is added twice to the same state machine.
   */
  smChoiceIdInUse = 27,
  /**
   * A transition is added to a state machine with a source (either a state or a
   * choice pseudo-state) which has not yet been defined.
   */
  smUndefinedTransSrc = 28,
  /**
   * A transition is added to a state machine with an illegal (out-of-range) state destination.
   */
  smIllegalPDest = 29,
  /**
   * A transition is added to a state machine with an illegal (out-of-range) choice pseudo-state destination.
   */
  smIllegalCDest = 30,
  /**
   * A choice pseudo-state is added to a state machine with less than 1 out-going transitions.
   */
  smIllNOfOutTrans = 31,
  /**
   * A transition is added to a state machine with a source which has an illegal identifier.
   */
  smIllTransSrc = 34,
  /**
   * An error was encountered while executing a transition in a state machine (see
   * <code>::FwSmMakeTrans</code>).
   */
  smTransErr = 35,
  /**
   * A transition from a certain source (either a state or a choice pseudo-state) is added
   * to a state machine but there isn't space for it in the transition array of the
   * state machine descriptor
   * (because all the locations reserved for transitions from that source are already full).
   * This error code is, for instance, generated if the same transition is added twice to
   * the state machine or if an insufficient number of out-going transitions has been
   * declared for a state or choice pseudo-state.
   */
  smTooManyTrans = 36,
  /**
   * A state or choice pseudo-state is added to a state machine which has more
   * out-going transitions than fit into the transition array of the state machine descriptor.
   */
  smTooManyOutTrans = 37,
  /**
   * The number of actions added to the state machine exceeds the number of actions declared
   * when the state machine descriptor was created.
   */
  smTooManyActions = 38,
  /**
   * The number of guards added to the state machine exceeds the number of guards declared
   * when the state machine descriptor was created.
   */
  smTooManyGuards = 39,
  /**
   * The number of actions added to the state machine is smaller than the number of actions declared
   * when the state machine descriptor was created.
   */
  smTooFewActions = 40,
  /**
   * The number of guards added to the state machine is smaller than the number of guards declared
   * when the state machine descriptor was created.
   */
  smTooFewGuards = 41,
  /**
   * A state is added with a negative number of outgoing transitions.
   */
  smNegOutTrans = 42,
  /**
   * The overridden action in a derived state machine does not exist.
   */
  smUndefAction = 43,
  /**
   * The overridden guard in a derived state machine does not exist.
   */
  smUndefGuard = 44,
  /**
   * The state in a derived state machine to which an embedded state machine is added
   * already holds an embedded state machine.
   */
  smEsmDefined = 45,
  /**
   * The state machine where an action or a guard is overridden or a state machine is embedded
   * is not a derived state machine.
   */
  smNotDerivedSM = 46,
  /**
   * The number of actions in the base state machine is not the same as in the derived
   * state machine.
   */
  smWrongNOfActions = 47,
  /**
   * The number of guards in the base state machine is not the same as in the derived
   * state machine.
   */
  smWrongNOfGuards = 48,
  /**
   * The state machine has a state which is not a destination of any transition
   */
  smUnreachablePState = 49,
  /**
   * The state machine has a choice pseudo-state which is not a destination of any transition
   */
  smUnreachableCState = 50
} FwSmErrCode_t;

/**
 * Identifier of "Execute" transition in a state machine.
 * Transition commands to a state machine are identified by a non-negative integer.
 * The value of zero is reserved for the "Execute" transition and should not be used
 * by applications.
 */
#define FW_TR_EXECUTE 0

#endif /* FWSM_CONSTANTS_H_ */
