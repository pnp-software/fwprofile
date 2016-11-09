/**
 * @file
 * @ingroup rtGroup
 * Declaration of the API for a RT Container.
 * A RT Container consists of:
 * - A POSIX thread (the container's Activation Thread)
 * - A POSIX mutex
 * - A POSIX condition variable
 * - Two procedures (the Notification Procedure and the Activation Procedure)
 * - A pointer to a user-defined data structure to hold the data associated to the
 *   RT Container
 * - An error code field to hold the code of the last error encountered by the
 *   container procedures
 * .
 * The Notification Procedure and the Activation Procedure implement the logic
 * defined in the figure below.
 *
 * This interface offers functions to:
 * - start and stop the RT Container;
 * - execute the container's Notification Procedure;
 * - query the state of the container or of its procedures.
 * .
 * The functions declared in this header file, take a RT Container Descriptor
 * as their argument.
 * This represents the RT Container upon which the functions operate.
 * The functions assume that the container has been fully and correctly configured
 * (this is done through the functions defined in <code>FwRtConfig.h</code>).
 *
 * The basic mode of use of the functions defined in this header file is as follows:
 * -# The container is configured through the functions offered by
 *    <code>FwRtConfig.h</code>.
 * -# The RT Container is started with function <code>::FwRtStart</code>.
 * -# The RT Container is notified by calling function
 *    <code>::FwRtNotify</code>.
 * -# The RT Container is stopped with function <code>::FwRtStop</code>.
 * .
 * After the container has been stopped, it can be re-started.
 * @image html RTContainer.png
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

#ifndef FWRT_CORE_H_
#define FWRT_CORE_H_

#include "FwRtConstants.h"

/**
 * Start a RT Container.
 * A RT Container can be in two states: STARTED or STOPPED.
 * This function proceeds as follows:
 * - It locks the container's mutex
 * - If the container is not in state STOPPED, the function releases
 *   the mutex and returns without doing anything.
 * - If instead the container is in state STOPPED, the function:
 *   1. puts the container in state STARTED;
 *   2. resets the Notification Counter
 *   3. starts the Notification Procedure
 *   4. starts the Activation Procedure
 *   5. executes the Notification Procedure (this causes its initialization action
 *      to be performed)
 *   6. executes the Activation Procedure (this causes its initialization action
 *      to be performed)
 *   7. creates the container's Activation Thread (this also releases the thread);
 *   8. release the mutex and returns
 * .
 * The attributes of the Activation Thread are NULL by default or are those
 * set with function <code>::FwRtSetPosixAttr</code>.
 *
 * If any of the system calls made by this function returns an error, the
 * container is put in an error state (see <code>::FwRtState_t</code>) and
 * the error code is stored in the <code>errCode</code> field of the container
 * descriptor.
 *
 * The Activation Thread is released upon creation and executes the following actions:
 * <pre>
 * while true do {
 *   lock container's mutex;
 *   	wait until Notification Counter is greater than zero;
 *   	decrement Notification Counter;
 *   release container's mutex;
 *   execute Activation Procedure;
 *
 *   if (Activation Procedure has terminated) then {
 *     put RT Container in STOPPED state;
 *     execute Notification Procedure;
 *     break;
 *   }
 *
 *   if (RT Container is in state STOPPED) then {
 *     execute Activation Procedure;
 *     execute Notification Procedure;
 *     break;
 *   }
 * }
 * </pre>
 * Use of this function is subject to the following constraint:
 * - The function may only be called before the Activation Thread has been created for
 *   the first time or after the Activation Thread has terminated execution (function
 *   <code>::FwRtWaitForTermination</code> can be used to wait for the Activation Thread
 *   to have terminated).
 * .
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtStart(FwRtDesc_t rtDesc);

/**
 * Stop a RT Container.
 * A RT Container can be in two states: STARTED or STOPPED.
 * This function proceeds as follows:
 * - It locks the container's mutex.
 * - If the container is not in state STARTED, the function releases
 *   the mutex and returns.
 * - If instead the container is in state STARTED, the function: (a) puts
 *   the container in the STOPPED state; (b) sends a notification to
 *   the Activation Thread; and (c) releases the mutex.
 * .
 * If any of the system calls made by this function returns an error, the
 * container is put in an error state (see <code>::FwRtState_t</code>) and
 * the error code is stored in the <code>errCode</code> field of the container
 * descriptor.
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtStop(FwRtDesc_t rtDesc);

/**
 * Blocking function which returns when the Activation Thread has terminated.
 * This function uses POSIX's <code>pthread_join</code> to wait until the
 * Activation Thread has terminated.
 *
 * Use of this function is subject to the following constraints:
 * - While this function is waiting on a given container, no other call to the
 *   function can be made on the same container.
 * - Use of this function assumes that the thread in the argument container
 *   is joinable (this will normally be the case if the container's thread
 *   has been created with default attributes).
 * .
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtWaitForTermination(FwRtDesc_t rtDesc);

/**
 * Execute the Notification Procedure of a RT Container.
 * This function proceeds as follows:
 * - It locks the container mutex.
 * - It executes the Notification Procedure
 * - It releases the mutex
 * .
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtNotify(FwRtDesc_t rtDesc);

/**
 * Check whether the Notification Procedure is started.
 * This function is not thread-safe (but note that it only returns the value of
 * a field of the RT Container descriptor).
 * @param rtDesc the descriptor of the RT Container.
 * @return 1 if the Notification Procedure is started, 0 otherwise.
 */
FwRtBool_t FwRtIsNotifPrStarted(FwRtDesc_t rtDesc);

/**
 * Check whether the Activation Procedure is started.
 * This function is not thread-safe (but note that it only returns the value of
 * a field of the RT Container descriptor).
 * @param rtDesc the descriptor of the RT Container.
 * @return 1 if the Activation Procedure is started, 0 otherwise.
 */
FwRtBool_t FwRtIsActivPrStarted(FwRtDesc_t rtDesc);

/**
 * Return the RT Container state.
 * This function is not thread-safe (but note that it only returns the value of
 * a field of the RT Container descriptor).
 * @param rtDesc the descriptor of the RT Container.
 * @return the state of the RT Container.
 */
FwRtState_t FwRtGetContState(FwRtDesc_t rtDesc);

/**
 * Return the error code of the RT Container.
 * This function is not thread-safe (but note that it only returns the value of
 * a field of the RT Container descriptor).
 * @param rtDesc the descriptor of the RT Container.
 * @return the state of the RT Container.
 */
int FwRtGetErrCode(FwRtDesc_t rtDesc);

/**
 * Return the value of the notification counter.
 * The value of the notification counter represents the number of pending notifications
 * in the RT Container.
 * This function is not thread-safe (but note that it only returns the value of
 * a field of the RT Container descriptor).
 * @param rtDesc the descriptor of the RT Container.
 * @return the value of the notification counter.
 */
FwRtCounterU2_t FwRtGetNotifCounter(FwRtDesc_t rtDesc);

#endif /* FWRT_CORE_H_ */
