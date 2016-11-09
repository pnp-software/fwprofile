/**
 * @file
 * @ingroup prGroup
 * Declaration of the dynamical creation interface for a FW Procedure.
 * A FW Procedure is described by a procedure descriptor.
 * This interface declares the functions required to create and release
 * a procedure descriptor dynamically.
 * Dynamic creation and release of memory is done through calls to
 * <code>malloc</code> and <code>free</code>.
 *
 * A procedure can be created in two ways:
 * - It can be created from scratch, or
 * - It can be created by extending an existing procedure.
 * .
 * The functions offered by this interface cover both creation of a new
 * procedure descriptor from scratch (<code>::FwPrCreate</code>) and
 * the extension of an existing procedure descriptor to create the
 * descriptor for a derived procedure (<code>::FwPrCreateDer</code>).
 *
 * Both the creation and the extension functions create a new procedure
 * descriptor and return a pointer to the newly created descriptor instance.
 * The procedure descriptor returned by these functions is initialized
 * (i.e. all its attributes have a well-defined value) but will normally
 * need to be configured.
 * Configuration can be done using the functions offered by
 * <code>FwPrConfig.h</code>.
 *
 * The creation and the extension functions in this header file always check the
 * success of calls to <code>malloc</code>.
 * In case of failure, the caller aborts and returns a NULL pointer.
 *
 * Applications which do not wish to use dynamic memory allocation can
 * create a procedure descriptor statically using the services offered
 * by <code>FwPrSCreate.h</code>.
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

#ifndef FWPR_DCREATE_H_
#define FWPR_DCREATE_H_

#include "FwPrConstants.h"

/**
 * Create a new procedure descriptor.
 * This function creates the procedure descriptor and its internal data structures
 * dynamically through calls to <code>malloc</code>.
 * If any of these calls fails, the function aborts and returns NULL.
 * In this case, there may be a memory leak if part of the procedure descriptor memory
 * had been allocated before the function was aborted.
 *
 * It is legal to create a procedure descriptor with no decision
 * nodes but it is not legal to create a procedure descriptor with no
 * control flows or with no action nodes.
 * @param nOfANodes the number of action nodes in the new procedure (a positive integer).
 * @param nOfDNodes the number of decision nodes in the new procedure
 * (a non-negative integer).
 * @param nOfFlows the number of control flows in the new procedure (an
 * integer greater than 1).
 * @param nOfActions the total number of actions which the
 * user wishes to define for the procedure.
 * The total number of actions must be a positive integer not greater than the number
 * of action nodes.
 * If the same action appears more than once in a procedure, it is counted only once.
 * @param nOfGuards the total number of control flow guards which the
 * user wishes to define for the procedure.
 * The total number of guards must be a non-negative integer not greater than the number
 * control flows.
 * If the same guard appears more than once in a procedure, it is counted only once.
 * @return the descriptor of the new procedure (or NULL if creation of the
 * data structures to hold the procedure descriptor failed or one of the
 * function parameters had an illegal value).
 */
FwPrDesc_t FwPrCreate(FwPrCounterS1_t nOfANodes, FwPrCounterS1_t nOfDNodes, FwPrCounterS1_t nOfFlows,
                      FwPrCounterS1_t nOfActions, FwPrCounterS1_t nOfGuards);

/**
 * Create the descriptor of a derived procedure.
 * A derived procedure is a procedure which is created by extending
 * another procedure.
 * The procedure which is thus extended is called base procedure.
 *
 * This function takes a procedure as an argument and creates a derived
 * procedure from it.
 * The function returns the descriptor of the newly created derived procedure.
 *
 * The base procedure should be fully and correctly configured (i.e. it should
 * pass the configuration check implemented by <code>::FwPrCheck</code>).
 * Compliance with this constraint is not checked by this function.
 * If the constraint is not satisfied, the behaviour of the derived procedure
 * is undefined.
 *
 * After being created, the derived procedure has the following characteristics:
 * - It has the same number of action and decision nodes as the base procedure.
 * - Its action and decision nodes are connected by the same control flows as
 *   the base procedure.
 * - Its action nodes have the same actions as the homologous nodes of the base node
 *   machine.
 * - Its control flows have the same guards as the homologous control flows of
 *   the base procedure.
 * .
 * Thus, the derived procedure is a structural clone of its base procedure.
 *
 * The attributes of the derived procedure are initialized as follows:
 * - The error code is the same as the error code of the base procedure.
 * - No procedure data are associated to the derived procedure.
 * - The execution counters are equal to zero
 * - The procedure state is STOPPED.
 * - The <code>flowCnt</code> field in the procedure descriptor is initialized
 *   to zero.
 * .
 * After being created, the derived procedure is fully configured because it
 * inherits the configuration of its base procedure.
 * The configuration of a derived procedure can be modified by:
 * - loading its procedure data, and
 * - overriding some or all of its actions and guards.
 * .
 * The functions to perform these reconfiguration operations are defined in
 * <code>FwPrConfig.h</code>.
 *
 * A procedure descriptor consists of two parts: the base descriptor and
 * the extension descriptor (see <code>FwPrPrivate.h</code>).
 * A derived procedure and its base procedure share the same base descriptor
 * (which defines the topology of the procedure) but have different extension
 * descriptors.
 * The extension descriptor is linked to the base descriptor through a pointer.
 * This function accordingly creates a new extension descriptor and links it to the
 * base descriptor of the base procedure.
 * @param prDesc the descriptor of the base procedure. The base procedure
 * should be a fully and correctly configured procedure (i.e. it should pass
 * the <code>::FwPrCheck</code> configuration check).
 * @return the descriptor of the derived procedure (or NULL if creation of the
 * data structures to hold the extended procedure descriptor failed).
 */
FwPrDesc_t FwPrCreateDer(FwPrDesc_t prDesc);

/**
 * Release the memory which was allocated when the procedure descriptor was created.
 * After this operation is called, the procedure descriptor can no longer be used.
 *
 * This function releases the memory of both the base and the extension parts of the
 * procedure descriptor.
 * Hence, if the argument procedure descriptor acted as base for other procedure
 * descriptors, the derived procedure descriptors are no longer usable
 * after the function has been called.
 *
 * Use of this function is subject to the following constraints:
 * - It should only be called on a procedure descriptor which was created using
 *   function <code>FwPrCreate</code>.
 * - It should only be called once on the same procedure descriptor.
 * - It should only be called on a procedure descriptor which is correctly configured.
 * .
 * Violation of any of the above constraints may result in memory corruption.
 * @param prDesc the descriptor of the procedure.
 */
void FwPrRelease(FwPrDesc_t prDesc);

/**
 * Release the memory allocated to a derived procedure descriptor.
 * After this operation is called, the argument procedure descriptor can no longer
 * be used.
 * The procedure descriptor of the base procedure is unaffected by this
 * function.
 *
 * Use of this function is subject to the following constraints:
 * - It should only be called on a procedure descriptor which was created using
 *   function <code>FwPrCreateDer</code>.
 * - It should only be called once on the same procedure descriptor.
 * - It should only be called on a procedure descriptor which is correctly configured.
 * .
 * Violation of any of the above constraints may result in memory corruption.
 * @param prDesc the descriptor of the procedure.
 */
void FwPrReleaseDer(FwPrDesc_t prDesc);

#endif /* FWSM_DCREATE_H_ */
