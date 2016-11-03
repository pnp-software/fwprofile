/**
 * @file
 * @ingroup smGroup
 * Declaration of the static creation interface for a FW State Machine.
 * A FW State Machine is described by a state machine descriptor.
 * This interface allows a state machine descriptor to be created
 * statically (i.e. without using dynamic memory allocation).
 * In this sense, this interface is alternative to the dynamic creation
 * interface defined in <code>FwSmDCreate.h</code>.
 *
 * A state machine can be created in two ways:
 * - It can be created from scratch, or
 * - It can be created by extending an existing state machine.
 * .
 * In both cases, creation of a state machine descriptor is done in two
 * steps:
 * - The state machine descriptor and its internal data structures are
 *   instantiated.
 * - The state machine descriptor and its internal data structures are
 *   initialized.
 * .
 * Instantiation is done by means of the following macros.
 * - <code>#FW_SM_INST</code> should be used to instantiate from scratch
 *   a descriptor for a state machine with one or more choice
 *   pseudo-states.
 * - <code>#FW_SM_INST_NOCPS</code> should be used to instantiate from
 *   scratch a descriptor for a state machine with no choice pseudo-states.
 * - <code>#FW_SM_INST_DER</code> should be used to instantiate a
 *   descriptor for a state machine which is derived by extending another
 *   state machine.
 * .
 * Initialization is done by means of the following functions:
 * - <code>::FwSmInit</code> should be used to initialize a descriptor
 *   which has been created from scratch with either
 *   <code>#FW_SM_INST</code> or <code>#FW_SM_INST_NOCPS</code>.
 * - <code>::FwSmInitDer</code> should be used to initialize a descriptor
 *   of a state machine which is derived by extending another state
 *   machine.
 * .
 * Function <code>::FwSmInitDer</code> only initializes a descriptor
 * in full if the base state machine (i.e. the state machine which is
 * being extended) has no embedded state machines.
 * If the base state machine has embedded state machines, additional
 * initialization actions are required as described in the function
 * documentation.
 *
 * After a state machine descriptor has been instantiated and
 * initialized, it will normally need to be configured.
 * Configuration of a state machine descriptor can be done using the
 * functions described in the <code>FwSmConfig.h</code> file.
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

#ifndef FWSM_SCREATE_H_
#define FWSM_SCREATE_H_

#include "FwSmCore.h"
#include "FwSmPrivate.h"

/**
 * Instantiate a state machine descriptor and its internal data structure.
 * This macro compiles correctly only if the number of states NS, the number
 * of choice pseudo-states NCPS and the number of transitions NTRANS is positive.
 * If there is a need to define a state machine with zero choice pseudo-states,
 * the <code>::FW_SM_INST_NOCPS</code> macro should be used.
 *
 * The macro generates code that does the following:
 * - It defines an array of NS elements of type <code>SmPState_t</code> to
 *   represent the array holding the state machine states.
 * - It defines an array of NS elements of to represent the array holding
 *   the state machines embedded in the NS states.
 * - It defines an array of NCPS elements of type <code>SmCState_t</code> to
 *   represent the array holding the state machine choice pseudo-states.
 * - It defines an array of NTRANS elements of type <code>SmTrans_t</code> to
 *   represent the array holding the state machine transitions.
 * - It defines an array of (NA+1) elements of type <code>SmAction_t</code> to
 *   represent the array holding the state machine actions.
 * - It defines an array of (NG+1) elements of type <code>SmGuard_t</code> to
 *   represent the array holding the state machine guards.
 * - It defines and initializes a variable of type <code>FwSmDescBase_t</code>
 *   to represent the base part of the state machine descriptor.
 * - It defines and initializes a variable with the name SM_DESC of type
 *   <code>struct FwSmDesc</code> to represent the state machine descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The state machine descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwSmInit</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 *
 * @param SM_DESC the variable holding the state machine descriptor
 * @param NS a positive integer representing the number of states
 * @param NCPS a positive integer the number of choice pseudo-states
 * @param NTRANS a positive integer representing the number of transitions
 * @param NA a non-negative integer representing the number of actions (i.e. the
 * number of transition or state actions which are defined on the state machine)
 * @param NG a non-negative integer representing the number of guards (i.e. the
 * number of transition actions which are defined on the state machine)
 */
#define FW_SM_INST(SM_DESC, NS, NCPS, NTRANS, NA, NG)                                                                  \
  static SmPState_t   SM_DESC##_pState[(NS)];                                                                          \
  static SmCState_t   SM_DESC##_cState[(NCPS)];                                                                        \
  static SmTrans_t    SM_DESC##_trans[(NTRANS)];                                                                       \
  static FwSmAction_t SM_DESC##_actions[(NA) + 1];                                                                     \
  static FwSmGuard_t  SM_DESC##_guards[(NG) + 1];                                                                      \
  static FwSmDesc_t   SM_DESC##_esm[(NS)];                                                                             \
  static SmBaseDesc_t SM_DESC##_base = {(SM_DESC##_pState), (SM_DESC##_cState), (SM_DESC##_trans), NS, NCPS, NTRANS};  \
  static struct FwSmDesc(SM_DESC) = {&(SM_DESC##_base),                                                                \
                                     (SM_DESC##_actions),                                                              \
                                     (SM_DESC##_guards),                                                               \
                                     (SM_DESC##_esm),                                                                  \
                                     NA + 1,                                                                           \
                                     NG + 1,                                                                           \
                                     1,                                                                                \
                                     0,                                                                                \
                                     0,                                                                                \
                                     0,                                                                                \
                                     smSuccess,                                                                        \
                                     NULL};

/**
 * Instantiate a state machine descriptor and its internal data structure.
 * This macro compiles correctly only if the number of states NS and the number
 * of transitions NTRANS is positive.
 * This macro instantiates a descriptor for a state machine without choice
 * pseudo-states.
 * If there is a need to define a state machine with one or more choice
 * pseudo-states, the <code>::FW_SM_INST</code> macro should be used.
 *
 * The macro generates code that does the following:
 * - It defines an array of NS elements of type <code>SmPState_t</code> to
 *   represent the array holding the state machine states.
 * - It defines an array of NS elements of to represent the array holding
 *   the state machines embedded in the NS states.
 * - It defines an array of NTRANS elements of type <code>SmTrans_t</code> to
 *   represent the array holding the state machine transitions.
 * - It defines an array of (NA+1) elements of type <code>SmAction_t</code> to
 *   represent the array holding the state machine actions.
 * - It defines an array of (NG+1) elements of type <code>SmGuard_t</code> to
 *   represent the array holding the state machine guards.
 * - It defines and initializes a variable of type <code>FwSmDescBase_t</code>
 *   to represent the base part of the state machine descriptor.
 * - It defines and initializes a variable with the name SM_DESC of type
 *   <code>struct FwSmDesc</code> to represent the state machine descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The state machine descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwSmInit</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 *
 * @param SM_DESC the variable holding the state machine descriptor
 * @param NS a positive integer representing the number of states
 * @param NTRANS a positive integer representing the number of transitions
 * @param NA a non-negative integer representing the number of actions (i.e. the
 * number of transition or state actions which are defined on the state machine)
 * @param NG a non-negative integer representing the number of guards (i.e. the
 * number of transition actions which are defined on the state machine)
 */
#define FW_SM_INST_NOCPS(SM_DESC, NS, NTRANS, NA, NG)                                                                  \
  static SmPState_t   SM_DESC##_pState[(NS)];                                                                          \
  static SmTrans_t    SM_DESC##_trans[(NTRANS)];                                                                       \
  static FwSmAction_t SM_DESC##_actions[(NA) + 1];                                                                     \
  static FwSmGuard_t  SM_DESC##_guards[(NG) + 1];                                                                      \
  static FwSmDesc_t   SM_DESC##_esm[(NS)];                                                                             \
  static SmBaseDesc_t SM_DESC##_base = {(SM_DESC##_pState), NULL, (SM_DESC##_trans), NS, 0, NTRANS};                   \
  static struct FwSmDesc(SM_DESC) = {&(SM_DESC##_base),                                                                \
                                     (SM_DESC##_actions),                                                              \
                                     (SM_DESC##_guards),                                                               \
                                     (SM_DESC##_esm),                                                                  \
                                     NA + 1,                                                                           \
                                     NG + 1,                                                                           \
                                     1,                                                                                \
                                     0,                                                                                \
                                     0,                                                                                \
                                     0,                                                                                \
                                     smSuccess,                                                                        \
                                     NULL};

/**
 * Instantiate a descriptor for a derived state machine.
 * A derived state machine is a state machine which is created by extending
 * another state machine.
 * The state machine which is thus extended is called base state machine.
 *
 * A state machine descriptor consists of two parts: the base descriptor and
 * the extension descriptor (see <code>FwSmPrivate.h</code>).
 * A derived state machine and its base state machine share the same base descriptor
 * (which defines the topology of the state machine) but have different extension
 * descriptors.
 * This macro accordingly only instantiates a new extension descriptor.
 * More precisely, this macro generates code that does the following:
 * - It defines an array of (NA+1) elements of type <code>SmAction_t</code> to
 *   represent the array holding the state machine actions.
 * - It defines an array of (NG+1) elements of type <code>SmGuard_t</code> to
 *   represent the array holding the state machine guards.
 * - It defines and initializes a variable with the name SM_DESC of type
 *   <code>struct FwSmDesc</code> to represent the state machine descriptor.
 * .
 * All variables defined by this macro are <code>static</code>.
 *
 * The state machine descriptor instantiated by the macro is only partially
 * initialized.
 * Full initialization is performed using function <code>::FwSmInitDer</code>.
 *
 * Since the macro includes the declaration of several variables, it should be located
 * in the section of a c-file where variable declaration is legal.
 *
 * @param SM_DESC the variable holding the state machine descriptor
 * @param NS a positive integer representing the number of states
 * @param NA a non-negative integer representing the number of actions (i.e. the
 * number of transition or state actions which are defined on the state machine)
 * @param NG a non-negative integer representing the number of guards
 */
#define FW_SM_INST_DER(SM_DESC, NS, NA, NG)                                                                            \
  static FwSmAction_t SM_DESC##_actions[(NA) + 1];                                                                     \
  static FwSmGuard_t  SM_DESC##_guards[(NG) + 1];                                                                      \
  static FwSmDesc_t   SM_DESC##_esm[(NS)];                                                                             \
  static struct FwSmDesc(SM_DESC) = {                                                                                  \
      NULL, (SM_DESC##_actions), (SM_DESC##_guards), (SM_DESC##_esm), NA + 1, NG + 1, 1, 0, 0, 0, smSuccess, NULL};

/**
 * Initialize a state machine descriptor to represent an unconfigured state
 * machine with no transitions, no actions, no guards and no embedded state
 * machines.
 * After this function has been executed, the argument state machine descriptor
 * has the same content as a state machine descriptor which has been
 * created by calling <code>::FwSmCreate</code>.
 *
 * This function is primarily intended to be used to initialize a state machine
 * descriptor which has been statically instantiated with macro
 * <code>#FW_SM_INST</code> or <code>#FW_SM_INST_NOCPS</code>.
 *
 * If the function is called upon a state machine descriptor that had already been
 * initialized, the previous initialization values are lost.
 * In such a case, a memory leak is possible due to the potential loss of the pointers
 * to the arrays where the state machines states, choice pseudo-states, transitions and
 * embedded state machines are stored.
 * @param smDesc the state machine descriptor to be initialized.
 */
void FwSmInit(FwSmDesc_t smDesc);

/**
 * Initialize a state machine descriptor to extend another state machine (the
 * base state machine).
 * This function checks that the descriptor to be initialized satisfies
 * the following constraints:
 * - it has the same number of actions as the base state machine, and
 * - it has the same number of guards as the base state machine.
 * .
 * If either constraint is not satisfied, the function reports an error
 * by setting the error code of the descriptor to be initialized and then
 * returns.
 * If the first constraint is not satisfied, the function sets the error
 * code to <code>#smWrongNOfActions</code>.
 * If the second constraint is not satisfied, the function sets the error
 * code to <code>#smWrongNOfGuards</code>.
 *
 * If both constraints are satisfied, this function initializes a descriptor
 * as follows:
 * - It links it to the descriptor of the base state machine.
 * - It initializes its actions to be the same as the actions of the base state
 *   machine.
 * - It initializes its guards to be the same as the guards of the base state
 *   machine.
 * - It initializes its states to have no embedded state machines.
 * - It initializes its error code to be the same as the error code of the base
 *   state machine.
 * - It initializes its <code>transCnt</code> field to zero.
 * - It sets its state to STOPPED.
 * .
 * Thus, if the base state machine has no embedded state machines, then this
 * function initializes a descriptor to represent its derived state machine.
 * In this case, the descriptor initialized by this function represents
 * exactly the same state machine as the descriptor created by calling
 * function <code>::FwSmCreateDer</code>.
 * If, instead, the base state machine has one or more embedded state machines,
 * then, in order to achieve the same effect as the <code>::FwSmCreateDer</code>
 * function, the descriptor initialized by this function must be further
 * configured as follows:
 * - The state machines embedded in the base state machine are extended.
 *   The extension is done using, recursively, the <code>#FW_SM_INST_DER</code>
 *   macro and the <code>::FwSmInitDer</code> function.
 *   Let ESM(i) be the extension of the state machine embedded in the
 *   i-th state of the base state machine.
 * - The ESM(i) state machine is embedded within the i-th state of the
 *   descriptor initialized by this function using the
 *   <code>::FwSmEmbed</code> function.
 * .
 *
 * This function is primarily intended to be used to initialize a state machine
 * descriptor which has been statically instantiated with macro
 * <code>#FW_SM_INST_DER</code>.
 * If the function is called upon a state machine descriptor that had already been
 * initialized, the previous initialization values are lost.
 * In such a case, a memory leak is possible due to the potential loss of the pointers
 * to the arrays where the state machines states, choice pseudo-states, transitions and
 * embedded state machines are stored.
 * @param smDesc the state machine descriptor to be initialized.
 * @param smDescBase the state machine descriptor of the base state machine.
 */
void FwSmInitDer(FwSmDesc_t smDesc, FwSmDesc_t smDescBase);

#endif /* FWSM_SCREATE_H_ */
