/**
 * @file
 * @ingroup prGroup
 * Declaration of the static creation interface for a FW Procedure.
 * A FW Procedure is described by a procedure descriptor.
 * This interface allows a procedure descriptor to be created
 * statically (i.e. without using dynamic memory allocation).
 * In this sense, this interface is alternative to the dynamic creation
 * interface defined in <code>FwPrDCreate.h</code>.
 *
 * A procedure can be created in two ways:
 * - It can be created from scratch, or
 * - It can be created by extending an existing procedure.
 * .
 * In both cases, creation of a procedure descriptor is done in two
 * steps:
 * - The procedure descriptor and its internal data structures are
 *   instantiated.
 * - The procedure descriptor and its internal data structures are
 *   initialized.
 * .
 * Instantiation is done by means of the following macros.
 * - <code>#FW_PR_INST</code> should be used to instantiate from scratch
 *   a descriptor for a procedure with one or more choice
 *   pseudo-states.
 * - <code>#FW_PR_INST_NODEC</code> should be used to instantiate from
 *   scratch a descriptor for a procedure with no choice pseudo-states.
 * - <code>#FW_PR_INST_DER</code> should be used to instantiate a
 *   descriptor for a procedure which is derived by extending another
 *   procedure.
 * .
 * Initialization is done by means of the following functions:
 * - <code>::FwPrInit</code> should be used to initialize a descriptor
 *   which has been created from scratch with either
 *   <code>#FW_PR_INST</code> or <code>#FW_PR_INST_NODEC</code>.
 * - <code>::FwPrInitDer</code> should be used to initialize a descriptor
 *   of a procedure which is derived by extending another procedure
 * .
 * After a procedure descriptor has been instantiated and
 * initialized, it will normally need to be configured.
 * Configuration of a procedure descriptor can be done using the
 * functions described in the <code>FwPrConfig.h</code> file.
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

#ifndef FWPR_SCREATE_H_
#define FWPR_SCREATE_H_

#include "FwPrPrivate.h"

/**
 * Instantiate a procedure descriptor and its internal data structure.
 * This macro compiles correctly only if the number of action nodes N, the number
 * of decision NDEC and the number of control flows NFLOWS is positive.
 * If there is a need to define a procedure with zero decision nodes,
 * the <code>::FW_PR_INST_NODEC</code> macro should be used.
 *
 * The macro generates code that does the following:
 * - It defines an array of N elements of type <code>PrANode_t</code> to
 *   represent the array holding the procedure action nodes.
 * - It defines an array of NDEC elements of type <code>PrDNodes_t</code> to
 *   represent the array holding the procedure decision nodes.
 * - It defines an array of NFLOWS elements of type <code>PrFlow_t</code> to
 *   represent the array holding the procedure control flows.
 * - It defines an array of NA elements of type <code>PrAction_t</code> to
 *   represent the array holding the procedure actions.
 * - It defines an array of (NG+1) elements of type <code>PrGuard_t</code> to
 *   represent the array holding the procedure guards (the extra guard is
 *   the "dummy guard" <code>::PrDummyGuard"</code> to be attached to control flows
 *   which have no guard).
 * - It defines and initializes a variable of type <code>FwPrDescBase_t</code>
 *   to represent the base part of the procedure descriptor.
 * - It defines and initializes a variable with the name PR_DESC of type
 *   <code>struct FwPrDesc</code> to represent the procedure descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The procedure descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwPrInit</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 * @param PR_DESC the variable holding the procedure descriptor
 * @param N a positive integer representing the number of action nodes
 * @param NDEC a positive integer the number of decision nodes
 * @param NFLOWS a positive integer representing the number of control flows
 * @param NA a positive integer representing the number of actions
 * @param NG a non-negative integer representing the number of guards (i.e. the
 * number of transition actions which are defined on the procedure)
 */
#define FW_PR_INST(PR_DESC, N, NDEC, NFLOWS, NA, NG)                                                                   \
  static PrANode_t    PR_DESC##_aNodes[(N)];                                                                           \
  static PrDNode_t    PR_DESC##_dNodes[(NDEC)];                                                                        \
  static PrFlow_t     PR_DESC##_flows[(NFLOWS)];                                                                       \
  static FwPrAction_t PR_DESC##_actions[(NA)];                                                                         \
  static FwPrGuard_t  PR_DESC##_guards[(NG) + 1];                                                                      \
  static PrBaseDesc_t PR_DESC##_base = {(PR_DESC##_aNodes), (PR_DESC##_dNodes), (PR_DESC##_flows), N, NDEC, NFLOWS};   \
  static struct FwPrDesc(PR_DESC) = {                                                                                  \
      &(PR_DESC##_base), (PR_DESC##_actions), (PR_DESC##_guards), NA, NG + 1, 1, 0, prSuccess, 0, 0, NULL};

/**
 * Instantiate a procedure descriptor and its internal data structure.
 * This macro compiles correctly only if the number of action nodes N
 * and the number of control flows is positive.
 * This macro instantiates a descriptor for a procedure without decision
 * nodes.
 * If there is a need to define a procedure with one or more decision
 * nodes, the <code>::FW_PR_INST</code> macro should be used.
 *
 * The macro generates code that does the following:
 * - It defines an array of N elements of type <code>PrANode_t</code> to
 *   represent the array holding the procedure action nodes.
 * - It defines an array of NFLOWS elements of type <code>PrFlow_t</code> to
 *   represent the array holding the procedure control flows.
 * - It defines an array of NA elements of type <code>PrAction_t</code> to
 *   represent the array holding the procedure actions.
 * - It defines an array of (NG+1) elements of type <code>PrGuard_t</code> to
 *   represent the array holding the procedure guards (the extra guard is
 *   the "dummy guard" <code>::PrDummyGuard"</code> to be attached to control flows
 *   which have no guard).
 * - It defines and initializes a variable of type <code>FwPrDescBase_t</code>
 *   to represent the base part of the procedure descriptor.
 * - It defines and initializes a variable with the name PR_DESC of type
 *   <code>struct FwPrDesc</code> to represent the procedure descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The procedure descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwPrInit</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 *
 * @param PR_DESC the variable holding the procedure descriptor
 * @param N a positive integer representing the number of action nodes
 * @param NFLOWS a positive integer representing the number of control flows
 * @param NA a positive integer representing the number of actions
 * @param NG a non-negative integer representing the number of guards (i.e. the
 * number of transition actions which are defined on the procedure)
 */
#define FW_PR_INST_NODEC(PR_DESC, N, NFLOWS, NA, NG)                                                                   \
  static PrANode_t    PR_DESC##_aNodes[(N)];                                                                           \
  static PrFlow_t     PR_DESC##_flows[(NFLOWS)];                                                                       \
  static FwPrAction_t PR_DESC##_actions[(NA)];                                                                         \
  static FwPrGuard_t  PR_DESC##_guards[(NG) + 1];                                                                      \
  static PrBaseDesc_t PR_DESC##_base = {(PR_DESC##_aNodes), NULL, (PR_DESC##_flows), N, 0, NFLOWS};                    \
  static struct FwPrDesc(PR_DESC) = {                                                                                  \
      &(PR_DESC##_base), (PR_DESC##_actions), (PR_DESC##_guards), NA, NG + 1, 1, 0, prSuccess, 0, 0, NULL};

/**
 * Instantiate a descriptor for a derived procedure.
 * A derived procedure is a procedure which is created by extending
 * another procedure.
 * The procedure which is thus extended is called base procedure.
 *
 * A procedure descriptor consists of two parts: the base descriptor and
 * the extension descriptor (see <code>FwPrPrivate.h</code>).
 * A derived procedure and its base procedure share the same base descriptor
 * (which defines the topology of the procedure) but have different extension
 * descriptors.
 * This macro accordingly only instantiates a new extension descriptor.
 * More precisely, this macro generates code that does the following:
 * - It defines an array of NA elements of type <code>PrAction_t</code> to
 *   represent the array holding the procedure actions.
 * - It defines an array of (NG+1) elements of type <code>PrGuard_t</code> to
 *   represent the array holding the procedure guards (the extra guard is
 *   the "dummy guard" <code>::PrDummyGuard"</code> to be attached to control
 *   flows which have no guard).
 * - It defines and initializes a variable with the name PR_DESC of type
 *   <code>struct FwPrDesc</code> to represent the procedure descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The procedure descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwPrInitDer</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 *
 * @param PR_DESC the variable holding the procedure descriptor
 * @param NA a non-negative integer representing the number of actions
 * @param NG a non-negative integer representing the number of guards
 */
#define FW_PR_INST_DER(PR_DESC, NA, NG)                                                                                \
  static FwPrAction_t PR_DESC##_actions[(NA)];                                                                         \
  static FwPrGuard_t  PR_DESC##_guards[(NG) + 1];                                                                      \
  static struct FwPrDesc(PR_DESC) = {NULL, (PR_DESC##_actions), (PR_DESC##_guards), NA, NG + 1, 1, 0, prSuccess, 0, 0, \
                                     NULL};

/**
 * Initialize a procedure descriptor to represent an unconfigured procedure
 * with no control flows, no actions, and no guards.
 * After this function has been executed, the argument procedure descriptor
 * has the same content as a procedure descriptor which has been
 * created by calling <code>::FwPrCreate</code>.
 *
 * This function is primarily intended to be used to initialize a procedure
 * descriptor which has been statically instantiated with macro
 * <code>#FW_PR_INST</code> or <code>#FW_PR_INST_NODEC</code>.
 *
 * If the function is called upon a procedure descriptor that had already been
 * initialized, the previous initialization values are lost.
 * In such a case, a memory leak is possible due to the potential loss of the pointers
 * to the arrays where the procedures nodes and control flows are stored.
 * @param prDesc the procedure descriptor to be initialized.
 */
void FwPrInit(FwPrDesc_t prDesc);

/**
 * Initialize a procedure descriptor to extend another procedure (the
 * base procedure).
 * This function checks that the descriptor to be initialized satisfies
 * the following constraints:
 * - it has the same number of actions as the base procedure, and
 * - it has the same number of guards as the base procedure.
 * .
 * If either constraint is not satisfied, the function reports an error
 * by setting the error code of the descriptor to be initialized and then
 * returns.
 * If the first constraint is not satisfied, the function sets the error
 * code to <code>#prWrongNOfActions</code>.
 * If the second constraint is not satisfied, the function sets the error
 * code to <code>#prWrongNOfGuards</code>.
 *
 * If both constraints are satisfied, this function initializes a descriptor
 * as follows:
 * - It links it to the descriptor of the base procedure.
 * - It initializes its actions to be the same as the actions of the base state
 *   machine.
 * - It initializes its guards to be the same as the guards of the base state
 *   machine.
 * - It initializes its error code to be the same as the error code of the base
 *   procedure.
 * - It initializes its <code>flowCnt</code> field to zero.
 * - It sets its state to STOPPED.
 * .
 * Thus, the descriptor initialized by this function represents
 * exactly the same procedure as the descriptor created by calling
 * function <code>::FwPrCreateDer</code>.
 *
 * This function is primarily intended to be used to initialize a procedure
 * descriptor which has been statically instantiated with macro
 * <code>#FW_PR_INST_DER</code>.
 * If the function is called upon a procedure descriptor that had already been
 * initialized, the previous initialization values are lost.
 * In such a case, a memory leak is possible due to the potential loss of the pointers
 * to the arrays where the procedures states, choice pseudo-states, transitions and
 * embedded procedures are stored.
 * @param prDesc the procedure descriptor to be initialized.
 * @param prDescBase the procedure descriptor of the base procedure.
 */
void FwPrInitDer(FwPrDesc_t prDesc, FwPrDesc_t prDescBase);

#endif /* FWPR_SCREATE_H_ */
