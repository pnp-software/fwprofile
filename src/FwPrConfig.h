/**
 * @file
 * @ingroup prGroup
 * Declaration of the configuration interface for a FW Procedure.
 * A FW Procedure is described by a procedure descriptor.
 * The functions declared in this interface allow a procedure descriptor
 * to be configured.
 * During the configuration process, the action nodes, the decision nodes and
 * the control flows of the procedure are defined.
 *
 * There are two types of procedures: newly created procedures
 * (i.e. procedures which are created from scratch using <code>::FwPrCreate</code>
 * or <code>#FW_PR_INST</code>) and derived procedures (i.e. procedures
 * which are created by extending another procedure through calls
 * to <code>::FwPrCreateDer</code> or <code>#FW_PR_INST_DER</code>).
 * The functions declared in this header file can be used to configure both types
 * of procedures.
 *
 * In the case of a newly created procedure, the mode of use of the
 * functions in this header file is as follows:
 * -# The action nodes of the procedure are added to its descriptor with
 *    the <code>::FwPrAddActionNode</code> function.
 * -# The decision nodes of the procedure are added to its
 *    descriptor with the <code>::FwPrAddDecisionNode</code> function.
 * -# The control flows of the procedures are added to the procedure
 *    with the <code>FwPrAddFlow*</code> functions (there are several of these
 *    functions, one for each type of control flow source and destination).
 * -# The pointer to the procedure data in the procedure descriptor
 *    is set with the <code>::FwPrSetData</code> function.
 * -# The consistency and completeness of the procedure configuration may
 *    optionally be verified with function <code>::FwPrCheck</code>.
 * .
 * The only constraint on the order in which the above functions are called is
 * that a control flow from an action node or decision node can only be defined
 * after the source action node or decision node has been defined.
 *
 * In the case of a derived procedure, the mode of use of the functions
 * declared in this header file is as follows:
 * -# An action can be overridden
 *    with the <code>::FwPrOverrideAction</code> function.
 * -# A guard can be overridden with the <code>::FwPrOverrideGuard</code> function.
 * -# The consistency and completeness of the configuration of the derived
 *    procedure may optionally be verified with function <code>::FwPrCheck</code>.
 * .
 * There are no constraints on the order in which the above functions are
 * called.
 *
 * Error handling is done as follows:
 * - Errors are reported through error codes. The range of error codes is
 *   defined in header file <code>FwPrConstant.h</code>. The error code is
 *   stored in the procedure descriptor (see <code>::FwPrGetErrCode</code>).
 * - If an illegal input value could cause a corruption of the internal data
 *   structures of a procedure descriptor, a check is performed on the
 *   input parameter value and the function is aborted if this is found to be
 *   illegal.
 * - When a function takes a procedure descriptor as an argument, no
 *   check is performed that the argument is non-NULL and valid (e.g. if
 *   a configuration function is called with a NULL value for the procedure
 *   descriptor argument, this error is not handled and the call
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

#ifndef FWPR_CONFIG_H_
#define FWPR_CONFIG_H_

#include "FwPrConstants.h"

/**
 * Set the pointer to the procedure data in the procedure descriptor.
 * The procedure data are data which are manipulated by the procedure
 * actions and guards.
 * The procedure descriptor holds a pointer to the procedure data.
 * This function allows this pointer to be defined.
 * The exact type of the procedure data is defined by applications for each
 * procedure.
 * It is therefore unknown at the level of the procedure module.
 * This function accordingly treats the pointer to the procedure data as a
 * pointer to <code>void</code>.
 * In most cases, the procedure data will take the form of a <code>struct</code>
 * whose fields represent the inputs and outputs for the procedure actions
 * and guards.
 *
 * The functions which implement the actions and guards access the procedure
 * data as follows:
 * - The functions implementing the actions and guards of a procedure
 *   take the procedure descriptor as an argument (because they must conform
 *   to the prototype defined by <code>::FwPrAction_t</code> and
 *   <code>::FwPrGuard_t</code>).
 * - The functions implementing the actions and guards of a procedure
 *   use the <code>::FwPrGetData</code> function to retrieve the pointer to the
 *   procedure data.
 * - The pointer returned by <code>::FwPrGetData</code> is defined as a pointer
 *   to <code>void</code> and will normally have to be cast to its true type.
 * .
 * @param prDesc the descriptor of the procedure to which the data structure is attached.
 * @param prData the pointer to the procedure data.
 * A value of NULL is legal (but note that the default value of the pointer to the
 * procedure data when the procedure is created is NULL).
 */
void FwPrSetData(FwPrDesc_t prDesc, void* prData);

/**
 * Get the pointer to the procedure data in the procedure descriptor.
 * This function returns the pointer which was set with the
 * <code>::FwPrSetData</code> function.
 * This function is normally used by the functions implementing a procedure
 * action or guard to access the procedure data.
 * See the description of the <code>::FwPrSetData</code> function for more details.
 * @param prDesc the descriptor of the procedure to which the data structure is attached.
 * @return the pointer to the procedure data or NULL if no procedure data
 * were defined for the procedure.
 */
void* FwPrGetData(FwPrDesc_t prDesc);

/**
 * Create an action node with the given characteristics and add it to a procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllActNodeId: illegal value of the action node identifier
 * - #prActNodeIdInUse: an action node with this identifier has already been defined
 * - #prTooManyOutFlows: there is no space left in the procedure descriptor for the
 *   out-going control flow of this node
 * - #prTooManyActions: there is no space left in the procedure descriptor for the
 *   action of this node
 * - #prNullAction: the action associated to this node is a NULL pointer
 * .
 * @param prDesc the procedure to which the action node is added.
 * @param nodeId the identifier of the action node to be added
 * (an integer in the range [1,N] where N is the number of action nodes in the procedure).
 * @param action the action for the new action node (a function pointer).
 */
void FwPrAddActionNode(FwPrDesc_t prDesc, FwPrCounterS1_t nodeId, FwPrAction_t action);

/**
 * Create a decision node with the given characteristics and add it to a procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllDecNodeId: illegal identifier for the decision node
 * - #prDecNodeIdInUse: a decision node with this identifier has already been defined
 * - #prIllNOfOutFlows: the number of out-going control flows is smaller than 2
 * - #prTooManyOutFlows: the decision node adds too many out-going control flows
 * .
 * @param prDesc the descriptor of the procedure to which the decision node is added.
 * @param nodeId the identifier of the decision node to be added (an integer
 * in the range [1,N] where N is the number of decision nodes).
 * @param nOfOutFlows the number of outgoing control flows from the new action node (a positive
 * integer greater than 1).
 */
void FwPrAddDecisionNode(FwPrDesc_t prDesc, FwPrCounterS1_t nodeId, FwPrCounterS1_t nOfOutFlows);

/**
 * Create a control flow from the initial node to an action node and add it
 * to a procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param destId the identifier of the destination action node of the control flow (an integer in the
 * range [1,N] where N is the number of action nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowIniToAct(FwPrDesc_t prDesc, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from the initial node to a decision node and add
 * it to a procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param destId the identifier of the destination decision node of the control flow (an
 * integer in the range [1,N] where N is the number of decision nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowIniToDec(FwPrDesc_t prDesc, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from an action node to another action node and add it
 * to a procedure.
 * This function should only be performed after the source action node of the control flow
 * has been added to the procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source action node of the control flow (an integer in the range
 * [1,N] where N is the number of action nodes).
 * @param destId the identifier of the destination action node of the control flow (an integer
 * in the range [1,N] where N is the number of action nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowActToAct(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from an action node to a decision node and add it to a procedure.
 * This function should only be performed after the source action node of the control flow
 * has been added to the action node procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source action node of the control flow (an integer in the range
 * [1,N] where N is the number of action nodes)
 * @param destId the identifier of the destination decision node of the control flow (an integer
 * in the range [1,M] where M is the number of decision nodes)
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowActToDec(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from a decision node to an action node and add it to a procedure.
 * This function should only be performed after the source of the control flow has been added
 * to the action node procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source decision node of the control flow (an integer
 * in the range [1,N] where N is the number of decision nodes).
 * @param destId the identifier of the destination action node of the control flow (an integer in the range
 * [1,N] where N is the number of action nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowDecToAct(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from a decision node to another decision node and add it to a procedure.
 * This function should only be performed after the source of the control flow has been added
 * to the action node procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source decision node of the control flow (an integer
 * in the range [1,N] where N is the number of decision nodes).
 * @param destId the identifier of the destination decision node of the control flow (an integer in the range
 * [1,N] where N is the number of decision nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowDecToDec(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrCounterS1_t destId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from an action node to the final node and add it to a procedure.
 * This function should only be performed after the source action node of the control flow
 * has been added to the action node procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source action node of the control flow (an integer in the range
 * [1,N] where N is the number of action nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowActToFin(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrGuard_t cfGuard);

/**
 * Create a control flow from a decision node to the final node and add it to a procedure.
 * This function should only be performed after the source action node of the control flow
 * has been added to the action node procedure.
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prIllFlowSrc: the identifier of the source of this control flow has an illegal value
 * - #prUndefinedFlowSrc: the source of this control flow has not yet been added to the procedure
 * - #prTooManyFlows: there is no space left in the control flow array of the procedure
 *   descriptor for this control flow
 * - #prTooManyGuards: there is no space left in the guard array of the procedure
 *   descriptor for the guard defined by this control flow
 * .
 * @param prDesc the descriptor of the procedure to which the control flow is added.
 * @param srcId the identifier of the source action node of the control flow (an integer in the range
 * [1,N] where N is the number of action nodes).
 * @param cfGuard the control flow guard (a function pointer) or NULL if no guard is attached to this
 * control flow.
 */
void FwPrAddFlowDecToFin(FwPrDesc_t prDesc, FwPrCounterS1_t srcId, FwPrGuard_t cfGuard);

/**
 * Check the correctness and completeness of the configuration of a procedure descriptor.
 * This function may be called on a procedure descriptor after it has been created and
 * configured.
 * This function does not modify the configuration of the procedure.
 * It checks the error conditions which may arise when an application configures a
 * procedure using the configuration functions declared in this header file.
 * More specifically, this function performs the following checks:
 * -# Check that no configuration errors have occurred (i.e. check that the error code
 *    in the procedure descriptor is equal to: <code>::prSuccess</code>).
 * -# Check that all action nodes have been defined.
 * -# Check that all decision nodes have been defined.
 * -# Check that all control flows have been defined.
 * -# Check that the destinations of the control flows out of a action node represent
 *    legal entities.
 * -# Check that the destinations of the control flows out of a decision node represent
 *    legal entities.
 * -# Check that the number of actions declared when the procedure was created is the same
 *    as the number of actions defined during the procedure configuration process.
 * -# Check that the number of guards declared when the procedure was created is the same
 *    as the number of guards defined during the procedure configuration process.
 * -# Check that all action nodes are reachable (i.e. they are a destination for a
 *    control flow).
 * -# Check that all decision nodes are reachable (i.e. they are a destination for a
 *    control flow).
 * .
 * Note that there are configuration errors which are not covered by this function because
 * they cannot occur if the procedure is configured using the functions declared in
 * this header file.
 * @param prDesc the descriptor of the procedure to be checked.
 * @return the outcome of the check. The outcome of the check is one of the following:
 * - #prSuccess: all checks have been passed.
 * - #prConfigErr: check 1 has failed.
 * - #prNullActNode: check 2 has failed.
 * - #prNullDecNode: check 3 has failed.
 * - #prNullFlow: check 4 has failed.
 * - #prIllegalADest: check 5 has failed
 * - #prIllegalDDest: check 6 has failed
 * - #prTooFewActions: check 7 has failed
 * - #prTooFewGuards: check 8 has failed
 * - #prUnreachableANode: check 9 has failed
 * - #prUnreachableDNode: check 10 has failed
 * .
 * This function returns when it encounters the first error. Hence, the function return value
 * is determined by the first error encountered by the function.
 */
FwPrErrCode_t FwPrCheck(FwPrDesc_t prDesc);

/**
 * Override an action in a derived procedure.
 * By default a derived procedure has the same actions as the base procedure
 * from which it was derived.
 * This function overrides one of the actions of the derived procedure.
 *
 * As an example consider the case of a base procedure B and suppose that action
 * a1 is attached to node S1 and node S2.
 * If an application creates a derived procedure D from B (for instance, through
 * function <code>::FwPrCreateDer</code>), then, after creation, the action of
 * action nodes S1 and S2 in procedure D is still a1.
 * This function can be used to change a1.
 * Note that a single invocation of this function will change the action of both
 * S1 and S2.
 *
 * If an attempt is made to override a non-existent action, an error is declared
 * and the function returns without doing anything.
 *
 * The override mechanism is only available for derived procedures.
 * If this function is called on a procedure which was not derived by extending
 * some other procedure, an error is declared.
 *
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prUndefAction: the action to be overridden does not exist
 * - #prNotDerivedPr: the procedure is not a derived procedure
 * .
 * @param prDesc the descriptor of the derived procedure.
 * @param oldAction the action to be overridden
 * @param newAction the new action which overrides the old action
 */
void FwPrOverrideAction(FwPrDesc_t prDesc, FwPrAction_t oldAction, FwPrAction_t newAction);

/**
 * Override a guard in a derived procedure.
 * By default a derived procedure has the same guards as the base procedure
 * from which it was derived.
 * This function overrides one of the guards of the derived procedure.
 *
 * As an example consider the case of a base procedure B and suppose that
 * the control flows F1 and F2 both have the same guard g1.
 * If an application creates a derived procedure D from B (for instance, through
 * function <code>::FwPrCreateDer</code>), then, after creation, the guard of the
 * control flows F1 and F2 in D is still g1.
 * This function can be used to change g1.
 * Note that a single invocation of this function will change the guard on both
 * control flows.
 *
 * If an attempt is made to override a non-existent guard, the function declares
 * an error and returns.
 *
 * The override mechanism is only available for derived procedures.
 * If this function is called on a procedure which was not derived by extending
 * some other procedure, an error is declared.
 *
 * This function reports the following errors in the error code of the procedure
 * descriptor:
 * - #prUndefGuard: the guard to be overridden does not exist
 * - #prNotDerivedPr: the procedure is not a derived procedure
 * .
 * @param prDesc the descriptor of the derived procedure.
 * @param oldGuard the guard to be overridden
 * @param newGuard the new guard which overrides the old guard
 */
void FwPrOverrideGuard(FwPrDesc_t prDesc, FwPrGuard_t oldGuard, FwPrGuard_t newGuard);

#endif /* FWPR_CONFIG_H_ */
