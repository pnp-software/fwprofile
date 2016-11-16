/**
 * @file
 * @ingroup prGroup
 * Declaration of the internal data structures of the FW Procedure
 * Module.
 * Users should not normally be concerned with these data structures.
 *
 * The data structures declared in this header file are used to define the
 * procedure descriptor.
 * A procedure descriptor holds all the information related to a certain procedure.
 * A procedure descriptor consists of two parts: the base descriptor and
 * the extension descriptor.
 *
 * The base descriptor holds the information which is not changed when the
 * procedure is extended.
 * This consists of:
 * - The list of action nodes in the procedure
 * - The list of decision nodes in the procedure
 * - The list of control flows in the procedure
 * .
 *
 * The extension descriptor holds the information which may be overridden when the
 * procedure is extended.
 * This consists of:
 * - The list of actions used in the procedure
 * - The list of guards used in the procedure
 * - The pointer to the procedure data (the data upon which the
 *   procedure actions operate)
 * - The current node of the procedure
 * - The execution counters of the procedure
 * - The error code for the procedure
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

#ifndef FWPR_PRIVATE_H_
#define FWPR_PRIVATE_H_

#include "FwPrConstants.h"

/**
 * Enumerated type for the type of a node in a procedure.
 */
typedef enum {
  /**
   * An action state in a procedure.
   */
  actionNode = 1,
  /**
   * A decision node in a procedure.
   */
  decisionNode = 2,
  /**
   * Either the initial or the final node.
   */
  stoppedNode = 3
} NodeType_t;

/**
 * Dummy guard which always returns true.
 * This guard is used where no control flow guard is specified.
 * @param prDesc procedure descriptor. This parameter is unused in this dummy guard.
 * The parameter is retained for compatibility with the <code>::FwPrGuard_t</code> type.
 * @return always return 1 (to signify that the guard is true).
 */
FwPrBool_t PrDummyGuard(FwPrDesc_t prDesc);

/**
 * Structure representing an action node in a procedure.
 * An action node is characterized by:
 * - the out-going control flow
 * - the action associated to the node
 * .
 * The out-going control flow is identified by an integer pointing at the location
 * in the control flow array in the base descriptor (see <code>::PrBaseDesc_t</code>).
 * Thus, for instance, if the <code>iFlow</code> field for node N is equal to 5,
 * then the out-going control flow of node N is the one stored at location 5 of the
 * control flow array of that procedure.
 *
 * The node action is identified by an integer pointing at the location in the
 * action array of the procedure descriptor where the action is stored.
 * Thus, for instance, if the <code>iAction</code> field for node N is equal to 5,
 * then the action of node N is the one stored at location 5 of the
 * action array of that procedure.
 *
 * By convention, the implementation treats an action node as uninitialized if its
 * <code>iFlow</code> field is equal to -1.
 */
typedef struct {
  /** index of out-going control flows */
  FwPrCounterS1_t iFlow;
  /** index of the action attached to the node */
  FwPrCounterS1_t iAction;
} PrANode_t;

/**
 * Structure representing a decision node in a procedure.
 * A decision node is characterized by the set of its out-going control flows.
 * The set of out-going control flows is defined as follows.
 * Control flows which originate from the same decision node are located in adjacent
 * locations in the control flow array of the base descriptor (see
 * <code>::PrBaseDesc_t</code>).
 * Field <code>outFlowIndex</code> identifies the first location of this set of adjacent
 * locations.
 * Thus, for instance, if a decision node has 3 out-going control flows and if its
 * <code>outFlowIndex</code> field has value 4, then the three out-going control flows
 * are located in elements 4, 5 and 6 of the array of control flows.
 *
 * By convention, the implementation treats a decision node as uninitialized if its
 * <code>outFlowIndex</code> field is equal to -1.
 */
typedef struct {
  /** index of first out-going control flow in control flow array */
  FwPrCounterS1_t outFlowIndex;
  /** number of outgoing control flows from the decision node */
  FwPrCounterS1_t nOfOutTrans;
} PrDNode_t;

/**
 * Structure representing a control flow.
 * A control flow is characterized by:
 * - the destination of the control flow
 * - the guard associated to the control flow
 * .
 * The destination of the control flow may be either an action node, or the final node,
 * or a decision node. The type of the control flow destination is identified as follows:
 * - a value of zero means that the destination is the final node
 * - a positive value means that the destination is an action node and the value of
 *   <code>dest</code> is the identifier of the destination node
 * - a negative value means that the destination is a decision node and the value of
 *   <code>-dest</code> is the identifier of the decision
 * .
 * The guard is an integer which identifies the location in the
 * guard array of the procedure descriptor (see <code>FwPrDesc</code>)
 * where the guard is stored as a function pointer.
 * Control flows which do not have a guard associated to them, set <code>iGuard</code>
 * equal to zero (the zero-th location in the guard array holds the "dummy
 * guard" <code>::PrDummyGuard</code> which always returns 1).
 *
 * By convention, the implementation treats a control flow as uninitialized if its
 * <code>iGuard</code> field is equal to -1.
 */
typedef struct {
  /** the index of the destination of the control flow */
  FwPrCounterS1_t dest;
  /** the index of the guard associated to the control flow */
  FwPrCounterS1_t iGuard;
} PrFlow_t;

/**
 * Structure representing the base descriptor of a procedure.
 * The base descriptor holds the information which is not changed when the procedure
 * is extended.
 * This consists of:
 * - The list of action nodes in the procedure (array <code>aNodes</code>)
 * - The list of decision nodes in the procedure (array <code>dNodes</code>)
 * - The list of control flows in the procedure (array <code>flows</code>)
 * .
 * Array <code>aNodes</code> holds the action nodes in the procedure.
 * The action nodes are identified by an integer in the range [1,N] (N is the total
 * number of nodes).
 * The i-th action node is stored in the (i-1)-th location of <code>aNodes</code>.
 * The number of action nodes is stored in field <code>nOfANodes</code>.
 *
 * Array <code>dNodes</code> holds the decision nodes in the procedure.
 * The decision nodes are identified by an integer in the range [1,M] (M is the total
 * number of decision nodes).
 * The i-th decision node is stored in the (i-1)-th location of <code>dNodes</code>.
 * The number of decision nodes is stored in field <code>nOfDNodes</code>.
 *
 * Array <code>flows</code> holds the control flows in the procedure.
 * The control flows are stored in groups of adjacent locations where each group
 * holds the control flows out of the same node (see also
 * <code>::PrANode_t</code> and <code>::PrDNode_t</code>).
 * The number of control flows is stored in field <code>nOfFlows</code>.
 */
typedef struct {
  /** array holding the action nodes in the procedure */
  PrANode_t* aNodes;
  /** array holding the decision nodes in the procedure */
  PrDNode_t* dNodes;
  /** array holding the control flows in the procedure */
  PrFlow_t* flows;
  /** the number of action nodes in the procedure */
  FwPrCounterS1_t nOfANodes;
  /** the number of decision nodes in the procedure */
  FwPrCounterS1_t nOfDNodes;
  /** the number of control flows in the procedure (excluding control flow from initial node) */
  FwPrCounterS1_t nOfFlows;
} PrBaseDesc_t;

/**
 * Structure representing a procedure descriptor.
 * Field <code>prBase</code> points to the base descriptor for the procedure
 * which holds the information about the action nodes, decision nodes and the
 * control flows connecting them.
 *
 * Array <code>prActions</code> holds the list of all actions in the procedure.
 * Each distinct action only appears once in the action array.
 * If the same action is used several times in a procedure, only one instance is
 * registered in the action array.
 *
 * Array <code>prGuards</code> holds the list of all control flow guards in the
 * procedure.
 * If the same guard is used several times in a procedure, only one instance is
 * registered in the guard array.
 * The first location in the guard array (location 0) holds the "dummy guard"
 * <code>::PrDummyGuard</code> which always returns 1.
 *
 * When a new action node is added to the procedure, field
 * <code>flowCnt</code> holds the position in the control flow array where its
 * out-going control flow will be stored.
 *
 * The identifier of the current node is stored in <code>curNode</code>.
 * The following convention is used:
 * - A value of 0 indicates that the procedure is in the STOPPED state;
 * - A value of -1 indicates that the procedure is in the STARTED state but the
 *   guard on the flow out of the initial node was false and hence the
 *   procedure is waiting in the initial node;
 * - A value of i (a positive integer) indicates that the procedure is in the
 *   STARTED state and i is the identifier of its current node.
 * .
 *
 * If during the creation, configuration or execution of the procedure, an error is
 * encountered, the corresponding error code is stored in field <code>errCode</code>.
 * This field is initialized to <code>#prSuccess</code> and should nominally remain
 * unchanged throughout the life of the procedure.
 * If the error code has a value other than <code>#prSuccess</code>, the behaviour of
 * the procedure is undefined.
 *
 * There are two types of procedures: base procedures (i.e.
 * procedures which are created from scratch using <code>::FwPrCreate</code> or
 * <code>#FW_SM_INST</code>) and derived procedures (i.e. procedures
 * which are created by extending a base procedure through calls
 * to <code>::FwPrCreateDer</code> or <code>#FW_PR_INST_DER</code>).
 * By convention, a derived procedure is characterized by field
 * <code>flowCnt</code> being equal to zero.
 *
 * Two counters are associated to a procedure: the Procedure Execution Counter
 * and the Node Execution Counter.
 * The Procedure Execution Counter holds the number of execution cycles since
 * the procedure was started and the Node Execution Counter holds the number of cycle
 * since the current node was entered.
 */
struct FwPrDesc {
  /** pointer to the base descriptor */
  PrBaseDesc_t* prBase;
  /** the procedure actions */
  FwPrAction_t* prActions;
  /** the control flow guards in the procedure */
  FwPrGuard_t* prGuards;
  /** the number of actions in the procedure */
  FwPrCounterS1_t nOfActions;
  /** the number of guards in the procedure */
  FwPrCounterS1_t nOfGuards;
  /** the counter for the number of control flows added to the procedure */
  FwPrCounterS1_t flowCnt;
  /** the current node of the procedure */
  FwPrCounterS1_t curNode;
  /** either 'success' or the code of the last error encountered by the procedure */
  FwPrErrCode_t errCode;
  /** the procedure execution counter */
  FwPrCounterU3_t prExecCnt;
  /** the node execution counter */
  FwPrCounterU3_t nodeExecCnt;
  /** the pointer to the data manipulated by the procedure actions and guards */
  void* prData;
};

#endif /* FWPR_PRIVATE_H_ */
