/**
 * @file
 * @ingroup prGroup
 * Header file to define all constants and types for the procedure
 * module of the FW Profile.
 * This header file should be included by all applications which use the
 * procedure module of the FW Profile.
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

#ifndef FWPR_CONSTANTS_H_
#define FWPR_CONSTANTS_H_

/**
 * Forward declaration for the pointer to a procedure descriptor.
 * A procedure descriptor is a data structure which holds all the information
 * describing a procedure instance.
 * Users only manipulate the pointer to the procedure descriptor. The internal
 * definition of the procedure descriptor (see <code>FwPrPrivate.h</code>) is
 * kept hidden from users.
 */
typedef struct FwPrDesc* FwPrDesc_t;

/**
 * Type for a pointer to a procedure action.
 * A procedure action is a function which encapsulates an action of a
 * procedure node.
 * A procedure action takes the descriptor of the procedure as an argument.
 * 
 * Pointers to procedure actions are used when a new procedure is defined:
 * all the actions in the procedure must be defined as functions which conform
 * to the <code>FwPrAction_t</code> prototype and are loaded into the procedure
 * as function pointers.
 */
typedef void (*FwPrAction_t)(FwPrDesc_t);

/** Type used for booleans (0 is "false" and 1 is "true"). */
typedef int FwPrBool_t;

/**
 * Type for a pointer to a procedure guard.
 * A procedure guard is a function which encapsulates a transition guard in a
 * procedure.
 * The procedure guard takes the descriptor of the procedure as an argument
 * and it returns 0 to signify that the guard is false or a non-zero value
 * (typically 1) to signify that the guard is true.
 * 
 * Pointers to procedure guards are used when a new procedure is defined:
 * all the guards in the procedure must be defined as functions which conform
 * to the <code>FwPrGuard_t</code> prototype and are loaded into the procedure
 * as function pointers.
 */
typedef FwPrBool_t (*FwPrGuard_t)(FwPrDesc_t);

/** Type used for unsigned counters with a "short" range. */
typedef unsigned char FwPrCounterU1_t;

/** Type used for unsigned counters with a "medium" range. */
typedef unsigned short int FwPrCounterU2_t;

/** Type used for unsigned counters with a "long" range. */
typedef unsigned int FwPrCounterU3_t;

/** Type used for unsigned counters with a "long int" range. */
typedef long unsigned int FwPrCounterU4_t;

/** Type used for signed counters with a "short" range. */
typedef signed char FwPrCounterS1_t;

/** Error codes and function return codes for the procedure functions. */
typedef enum {
	/**
	 * Return codes of a function which has completed execution without errors.
	 */
	prSuccess = 1,
	/**
	 * A call to <code>malloc</code> has failed (it has returned a NULL pointer).
	 */
	prOutOfMemory = 2,
	/**
	 * The number of actions in the base procedure is not the same as in the derived
	 * procedure.
	 */
	prWrongNOfActions = 3,
	/**
	 * The number of guards in the base procedure is not the same as in the derived
	 * procedure.
	 */
	prWrongNOfGuards = 4,
	/**
	 * An action node is added to a procedure with an illegal (out-of-range) identifier.
	 */
	prIllActNodeId = 5,
	/**
	 * An action node is added twice to the same procedure.
	 */
	prActNodeIdInUse = 6,
	/**
	 * A decision node is added to a procedure with an illegal (out-of-range) identifier.
	 */
	prIllDecNodeId = 7,
	/**
	 * A decision node is added twice to the same procedure.
	 */
	prDecNodeIdInUse = 8,
	/**
	 * The number of actions added to the procedure exceeds the number of actions declared
	 * when the procedure descriptor was created.
	 */
	prTooManyActions = 9,
	/**
	 * The number of guards added to the procedure exceeds the number of guards declared
	 * when the procedure descriptor was created.
	 */
	prTooManyGuards = 10,
	/**
	 * An action node is defined with a null action.
	 */
	prNullAction = 11,
	/**
	 * A node is added to a procedure which has more
	 * out-going transitions than fit into the control flow array of the procedure descriptor.
	 */
	prTooManyOutFlows = 12,
	/**
	 * A choice pseudo-state is added to a procedure with less than 2 out-going control flows.
	 */
	prIllNOfOutFlows = 13,
	/**
	 * A control flow from a certain source is added
	 * to a procedure but there isn't space for it in the control flow array of the
	 * procedure descriptor.
	 */
	prTooManyFlows = 14,
	/**
	 * A control flow is added to a SM with a source which has an illegal value.
	 */
	prIllFlowSrc = 15,
	/**
	 * A configuration error has been detected during the procedure configuration process.
	 */
	prConfigErr = 16,
	/**
	 * There is an undefined action node in a procedure.
	 */
	prNullActNode = 17,
	/**
	 * There is an undefined decision node in a procedure.
	 */
	prNullDecNode = 18,
	/**
	 * There is an undefined control flow in a procedure.
	 */
	prNullFlow = 19,
	/**
	 * A control flow is added to a procedure with a source (either a state or a source
	 * choice pseudo-state) which has not yet been defined.
	 */
	prUndefinedFlowSrc = 20,
	/**
	 * A control flow is added to a procedure with an illegal (out-of-range) action node destination.
	 */
	prIllegalADest = 21,
	/**
	 * A control flow is added to a procedure with an illegal (out-of-range) decision node destination.
	 */
	prIllegalDDest = 22,
	/**
	 * The number of actions added to the procedure is smaller than the number of actions declared
	 * when the procedure descriptor was created.
	 */
	prTooFewActions = 23,
	/**
	 * The number of guards added to the procedure is smaller than the number of guards declared
	 * when the procedure descriptor was created.
	 */
	prTooFewGuards = 24,
	/**
	 * An error was encountered while executing a transition in a procedure (see
	 * <code>::FwPrExecute</code>).
	 */
	prFlowErr = 25,
	/**
	 * The overridden action in a derived procedure does not exist.
	 */
	prUndefAction = 26,
	/**
	 * The overridden guard in a derived procedure does not exist.
	 */
	prUndefGuard = 27,
	/**
	 * The procedure where an action or a guard is overridden or a procedure is embedded
	 * is not a derived procedure.
	 */
	prNotDerivedPr = 28,
	/**
	 * The procedure has an action node which is not a destination of any control flow
	 */
	prUnreachableANode = 29,
	/**
	 * The procedure has a decision node which is not a destination of any control flow
	 */
	prUnreachableDNode = 30
} FwPrErrCode_t;

#endif /* FWPR_CONSTANTS_H_ */
