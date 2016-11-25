/**
 * @file
 * @ingroup smGroup
 * Declaration of the dynamical creation interface for a FW State Machine.
 * A FW State Machine is described by a state machine descriptor.
 * This interface declares the functions required to create and release
 * a state machine descriptor dynamically.
 * Dynamic creation and release of memory is done through calls to
 * <code>malloc</code> and <code>free</code>.
 *
 * A state machine can be created in two ways:
 * - It can be created from scratch, or
 * - It can be created by extending an existing state machine.
 * .
 * The functions offered by this interface cover both creation of a new
 * state machine descriptor from scratch (<code>::FwSmCreate</code>) and
 * the extension of an existing state machine descriptor to create the
 * descriptor for a derived state machine (<code>::FwSmCreateDer</code>).
 *
 * Both the creation and the extension functions create a new state machine
 * descriptor and return a pointer to the newly created descriptor instance.
 * The state machine descriptor returned by these functions is initialized
 * (i.e. all its attributes have a well-defined value) but will normally
 * need to be configured.
 * Configuration can be done using the functions offered by the
 * <code>FwSmConfig.h</code> file.
 *
 * The creation and the extension functions in this header file always check the
 * success of calls to <code>malloc</code>.
 * In case of failure, the function aborts and returns a NULL pointer.
 * Memory which had already been allocated at the time the function aborts,
 * is not released.
 *
 * Applications which do not wish to use dynamic memory allocation can
 * create a state machine descriptor statically using the services offered
 * by <code>FwSmSCreate.h</code>.
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

#ifndef FWSM_DCREATE_H_
#define FWSM_DCREATE_H_

#include "FwSmCore.h"

/**
 * Create a new state machine descriptor.
 * This function creates the state machine descriptor and its internal data structures
 * dynamically through calls to <code>malloc</code>.
 * If any of these calls fails, the function aborts and returns NULL.
 * In this case, there may be a memory leak if part of the state machine descriptor memory
 * had been allocated before the function was aborted.
 *
 * It is legal to create a state machine descriptor with no states or with no choice
 * pseudo-states but it is not legal to create a state machine descriptor with no
 * transitions.
 * If the latter is attempted, the function is aborted and returns NULL.
 * @param nOfStates the number of states in the new state machine (a non-negative number).
 * @param nOfChoicePseudoStates the number of choice pseudo-states in the new state machine
 * (a non-negative integer).
 * @param nOfTrans the number of transitions in the new state machine (a
 * positive integer).
 * @param nOfActions the total number of actions (state actions + transition actions) which the
 * user wishes to define for the state machine.
 * The total number of actions must be a non-negative integer.
 * If the same action appears more than once in a state machine, it is counted only once.
 * @param nOfGuards the total number of transition guards which the
 * user wishes to define for the state machine.
 * The total number of guards must be a non-negative integer.
 * If the same guard appears more than once in a state machine, it is counted only once.
 * @return the descriptor of the new state machine (or NULL if creation of the
 * data structures to hold the state machine descriptor failed or one of the
 * function parameters had an illegal value).
 */
FwSmDesc_t FwSmCreate(FwSmCounterS1_t nOfStates, FwSmCounterS1_t nOfChoicePseudoStates, FwSmCounterS1_t nOfTrans,
                      FwSmCounterS1_t nOfActions, FwSmCounterS1_t nOfGuards);

/**
 * Create the descriptor of a derived state machine.
 * A derived state machine is a state machine which is created by extending
 * another state machine.
 * The state machine which is thus extended is called base state machine.
 *
 * This function takes a state machine as an argument and creates a derived
 * state machine from it.
 * The function returns the descriptor of the newly created derived state machine.
 *
 * The base state machine should be fully and correctly configured (i.e. it should
 * pass the configuration check implemented by <code>::FwSmCheck</code>).
 * Compliance with this constraint is not checked by this function.
 * If the constraint is not satisfied, the behaviour of the derived state machine
 * is undefined.
 *
 * After being created, the derived state machine has the following characteristics:
 * - It has the same number of states, choice pseudo-states and transitions as
 *   the base state machine.
 * - Its states and choice pseudo-states are connected by the same transitions as
 *   the base state machine.
 * - Its states have the same actions as the homologous states of the base state
 *   machine.
 * - Its transitions have the same actions and guards and are triggered by the same
 *   transition commands as the homologous transitions of the base state machine.
 * - States whose homologous states in the base state machine have an embedded
 *   state machine also have an embedded state machine: the derivation process
 *   is applied recursively to the embedded state machines (i.e. if state S1 in
 *   the base state machine has an embedded state machine SM1, then the homologous
 *   state in the derived state machine has an embedded state machine which is
 *   derived by extending SM1).
 * .
 * Thus, the derived state machine is a structural clone of its base state machine.
 *
 * The attributes of the derived state machine are initialized as
 * follows:
 * - The error code is the same as the error code of the base state machine.
 * - No state machine data are associated to the derived state machine.
 * - The state machine state is STOPPED.
 * - The execution counters are equal to zero.
 * - The <code>transCnt</code> field in the state machine descriptor is initialized
 *   to zero.
 * .
 * After being created, the derived state machine is fully configured because it
 * inherits the configuration of its base state machine.
 * The configuration of a derived state machine can be modified by:
 * - loading its state machine data;
 * - overriding some or all of its actions and guards; and
 * - embedding state machines in some or all of its states which do not
 *   already hold embedded state machines.
 * .
 * The functions to perform these reconfiguration operations are defined in
 * <code>FwSmConfig.h</code>.
 *
 * A state machine descriptor consists of two parts: the base descriptor and
 * the extension descriptor (see <code>FwSmPrivate.h</code>).
 * A derived state machine and its base state machine share the same base descriptor
 * (which defines the topology of the state machine) but have different extension
 * descriptors.
 * The extension descriptor is linked to the base descriptor through a pointer.
 * This function accordingly creates a new extension descriptor and links it to the
 * base descriptor of the base state machine.
 * @param smDesc the descriptor of the base state machine. The base state machine
 * should be a fully and correctly configured state machine (i.e. it should pass
 * the <code>::FwSmCheck</code> configuration check).
 * @return the descriptor of the derived state machine (or NULL if creation of the
 * data structures to hold the extended state machine descriptor failed).
 */
FwSmDesc_t FwSmCreateDer(FwSmDesc_t smDesc);

/**
 * Release the memory which was allocated when the state machine descriptor.
 * After this operation is called, the state machine descriptor can no longer be used.
 *
 * This function releases the memory of both the base and the extension parts of the
 * state machine descriptor.
 * Hence, if the argument state machine descriptor acted as base for other state
 * machine descriptors, the derived state machine descriptors are no longer usable
 * after the function has been called.
 *
 * This function only releases the memory of the argument state machine.
 * The memory allocated to embedded state machines is not affected.
 *
 * Use of this function is subject to the following constraints:
 * - It should only be called on a state machine descriptor which was created using
 *   function <code>FwSmCreate</code>.
 * - It should only be called once on the same state machine descriptor.
 * - It should only be called on a state machine descriptor which is correctly
 *   configured.
 * .
 * Violation of any of the above constraints may result in memory corruption.
 *
 * @param smDesc the descriptor of the state machine.
 */
void FwSmRelease(FwSmDesc_t smDesc);

/**
 * Release the memory allocated to a derived state machine descriptor.
 * After this operation is called, the argument state machine descriptor can no longer
 * be used.
 * The state machine descriptor of the base state machine is unaffected by this
 * function.
 *
 * This function only releases the memory of the argument state machine.
 * The memory allocated to embedded state machines is not affected.
 *
 * Use of this function is subject to the following constraints:
 * - It should only be called on a state machine descriptor which was created using
 *   function <code>FwSmCreateDer</code>.
 * - It should only be called once on the same state machine descriptor.
 * - It should only be called on a state machine descriptor which is correctly
 *   configured.
 * .
 * Violation of any of the above constraints may result in memory corruption.
 * @param smDesc the descriptor of the state machine.
 */
void FwSmReleaseDer(FwSmDesc_t smDesc);

/**
 * Recursively release the memory which was allocated when the state machine descriptor
 * was created.
 * After this operation is called, the state machine descriptor can no longer be used.
 * This operation also (recursively) releases the memory of the state machines embedded
 * in the argument state machine.
 *
 * The release of the memory for the state machine and its embedded state machines
 * is done by means of function <code>FwSmRelease</code>.
 * The same constraints defined for <code>FwSmRelease</code> therefore also apply
 * to this function.
 *
 * As in the case of the <code>FwSmRelease</code>, both the the base and the extension
 * parts of the argument state machine descriptor are released.
 * Hence, if the argument state machine or any of its embedded state machines acted as
 * bases for other state machines, the derived state machine descriptors are no longer
 * usable after the function has been called.
 * @param smDesc the descriptor of the state machine.
 */
void FwSmReleaseRec(FwSmDesc_t smDesc);

#endif /* FWSM_DCREATE_H_ */
