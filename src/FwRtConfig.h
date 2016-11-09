/**
 * @file
 * @ingroup rtGroup
 * Declaration of the configuration interface for a RT Container.
 * A RT Container is described by a container descriptor.
 * The functions declared in this interface allow a container descriptor
 * to be configured.
 *
 * The user instantiates a RT Container descriptor by creating a variable
 * of type <code>struct FwRtCont</code>.
 * After the container descriptor has been instantiated, it is configured
 * using the functions defined in this header file as follows:
 * -# The container descriptor is reset with function <code>::FwRtReset</code>.
 * -# The attributes of the container's POSIX objects may be set with
 *    function <code>::FwRtSetPosixAttr</code>.
 * -# The pointers to the procedure functions which implement the adaptation
 *    points of the container may be set with function <code>FwRtSetProcActions</code>.
 * -# The pointer to the RT Container data in the container descriptor
 *    may be set with the <code>::FwRtSetData</code> function.
 * -# The container is initialized with function <code>::FwRtInit</code>.
 *    This completes the configuration process.
 * .
 * Configuration can only be done before the container is started with
 * <code>::FwRtStart</code> or after the container has been shut down with
 * <code>::FwRtShutdown</code>.
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

#ifndef FWRT_CONFIG_H_
#define FWRT_CONFIG_H_

#include "FwRtConstants.h"
#include <pthread.h>

/**
 * Reset the RT Container descriptor (RTD).
 * This function should be called at the beginning of the configuration process.
 * The function initializes the fields of the container descriptor with default
 * values as follows:
 * - The actions of the Notification Procedure and of the Activation Procedure are set
 *   equal to a dummy action representing a situation where all
 *   procedure actions do nothing and always return 1.
 * - The attribute objects for the Activation Thread, for the container mutex, and for
 *   the container condition variable are set to NULL. This represents a situation
 *   where the default values for these attributes are used.
 * - The error code is set to zero.
 * - The notification counter is set to zero.
 * - The container state is set to: <code>::rtContUninitialized</code>.
 * - The state of the Activation and Notification Procedures is set to: STOPPED.
 * - The pointer to the container data is set to NULL.
 * .
 * @param rtDesc the descriptor of the RT Container
 */
void FwRtReset(FwRtDesc_t rtDesc);

/**
 * Initialize a RT Container.
 * The following initialization actions are performed:
 * - If the container state is not <code>::rtContUninitialized</code>, the function
 *   sets the container's state to the error state <code>::rtConfigErr</code>
 *   and then returns.
 * - If non-NULL attributes have been loaded for the container's POSIX objects
 *   (i.e. if the user has called function <code>FwRtSetPosixAttr</code> with
 *   non-NULL values), the attributes are initialized using their POSIX initialization
 *   functions (<code>pthread_attr_init</code>, <code>pthread_mutexattr_init</code>,
 *   and <code>pthread_condattr_init</code>) and then the initialized attributes
 *   are used to initialize the mutex and the condition variable.
 * - If no attributes have been loaded for the container's POSIX mutex and
 *   condition variable (i.e. if the user has not called function
 *   <code>::FwRtSetPosixAttr</code> or he has called it with NULL values for
 *   the POSIX attributes), the container POSIX mutex and condition variable are
 *   initialized with their default attribute values.
 * - If any of the POSIX system calls made by this function returns an error,
 *   the container state is set to an error state (see <code>::FwRtState_t</code>)
 *   and the error code is initialized with the error code returned by the failed
 *   POSIX system call.
 * - If all the POSIX system calls made by this function are successful,
 *   the container state is set to <code>::rtContStopped</code>.
 * .
 * A call to this function concludes the configuration process of the RTD.
 * The configuration process has been successful if, after this function is called,
 * the container is in state <code>::rtContStopped</code>.
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtInit(FwRtDesc_t rtDesc);

/**
 * Shutdown the RT Container.
 * The following shutdown actions are performed:
 * - If non-NULL attributes have been loaded for the container's POSIX thread,
 *   or for the container's POSIX mutex, or for the container's POSIX condition
 *   variable, the attributes are destroyed using their POSIX destruction
 *   functions (<code>pthread_attr_destroy</code>, <code>pthread_mutexattr_destroy</code>,
 *   and <code>pthread_condattr_destroy</code>).
 * - The container POSIX mutex and POSIX condition variables are destroyed using
 *   their POSIX destructor functions (<code>pthread_mutex_destroy</code> and
 *   <code>pthread_cond_destroy</code>).
 * - If all the POSIX system calls made by this function are successful, then the
 *   container state is set to <code>::rtContUninitialized</code> to signify
 *   that the container is now uninitialized.
 * - If any of the POSIX system calls made by this function returns an error,
 *   the container state is set to an error state (see <code>::FwRtState_t</code>)
 *   and the error code is set to the error code returned by the failed POSIX
 *   system call.
 * .
 * After this function has been called, the container must be again configured in
 * full before it can again be used.
 *
 * This function should only be called when the RT Container is in state STOPPED
 * and after its Activation Thread has terminated execution.
 * Enforcement of this constraint is under the responsibility of the user.
 *
 * @param rtDesc the descriptor of the RT Container.
 */
void FwRtShutdown(FwRtDesc_t rtDesc);

/**
 * Set the pointers to the attribute objects of the POSIX thread, mutex and condition
 * variable used by the container.
 * A RT Container uses a POSIX thread, a POSIX mutex and a POSIX condition
 * variable.
 * Their configuration is defined by attribute objects.
 * This function allows the attribute objects to be loaded into the container.
 * A value of NULL for an attribute object means that the POSIX-defined default
 * value for that attribute will be used.
 * The attribute objects will be initialized at the end of the container
 * configuration process when <code>::FwRtInit</code> is called.
 *
 * The thread attributes are used when the container's thread (the Activation Thread)
 * is started by calling <code>::FwRtStart</code>.
 * The mutex and condition variable attributes are used when the container mutex and
 * its condition variable are initialized by calling <code>::FwRtInit</code>.
 *
 * If this function is not called during the container configuration process,
 * the container will be configured with NULL values for all POSIX attribute
 * objects (i.e. the POSIX thread, mutex and condition variable will be
 * initialized with their POSIX-defined default attribute values).
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the descriptor of the RT Container
 * @param pThreadAttr the attributes of the Activation Thread (or NULL if it is desired
 * to use default attribute for the thread)
 * @param pMutexAttr the attributes of the container's mutex (or NULL if it is desired
 * to use default attribute for the mutex)
 * @param pCondAttr the attributes of the container's mutex (or NULL if it is desired
 * to use default attribute for the condition variable)
 */
void FwRtSetPosixAttr(FwRtDesc_t rtDesc, pthread_attr_t* pThreadAttr, pthread_mutexattr_t* pMutexAttr,
                      pthread_condattr_t* pCondAttr);

/**
 * Get the value of the attributes of the Activation Thread.
 * The attribute values returned by this call are those set with
 * <code>::FwRtSetPosixAttr</code> or NULL if the container
 * was initialized without calling this function.
 * @param rtDesc the descriptor of the RT Container
 * @return the attributes of the Activation Thread
 */
pthread_attr_t* FwRtGetActivThreadAttr(FwRtDesc_t rtDesc);

/**
 * Get the value of the attributes of the mutex associated to the RT Container.
 * The attribute values returned by this call are those set with
 * <code>::FwRtSetPosixAttr</code> or NULL if the container
 * was initialized without calling this function.
 * @param rtDesc the descriptor of the RT Container
 * @return the attributes of the mutex
 */
pthread_mutexattr_t* FwRtGetMutexAttr(FwRtDesc_t rtDesc);

/**
 * Get the value of the attributes of the condition variable associated to
 * the RT Container.
 * The attribute values returned by this call are those set with
 * <code>::FwRtSetPosixAttr</code> or NULL if the container
 * was initialized without calling this function.
 * @param rtDesc the descriptor of the RT Container
 * @return the attributes of the condition variable
 */
pthread_condattr_t* FwRtGetCondAttr(FwRtDesc_t rtDesc);

/**
 * Set the pointer to the RT Container data in the container descriptor.
 * The container data are data which are manipulated by the container's
 * procedures.
 * The container descriptor holds a pointer to the procedure data.
 * This function allows this pointer to be defined.
 * The exact type of the container data is defined by applications for each
 * RT Container.
 * It is therefore unknown at the level of the RT Container module.
 * This function accordingly treats the pointer to the procedure data as a
 * pointer to <code>void</code>.
 * In most cases, the container data will take the form of a <code>struct</code>
 * whose fields represent the inputs and outputs for the procedure actions
 * and guards.
 *
 * The functions which implement the container's procedures access the
 * container data as follows:
 * - The procedure functions take the container descriptor as an argument
 *   (because they must conform to the <code>::FwRtAction_t</code> prototype).
 * - The procedure functions use the <code>::FwRtGetData</code> function to
 *   retrieve the pointer to the container data.
 * - The pointer returned by <code>::FwRtGetData</code> is defined as a pointer
 *   to <code>void</code> and will normally have to be cast to its true type.
 * .
 * If this function is not called, the value of the container data is NULL.
 *
 * This function may be called at any time but should normally be called only during
 * the container configuration process.
 * @param rtDesc the RT Container descriptor.
 * @param rtData the pointer to the container data (a value of NULL is legal
 * if no data need be attached to the container).
 */
void FwRtSetData(FwRtDesc_t rtDesc, void* rtData);

/**
 * Get the pointer to the container data in the container descriptor.
 * This function returns the pointer which was set with the
 * <code>::FwRtSetData</code> function.
 * @param rtDesc the RT Container descriptor.
 * @return the pointer to the container data.
 */
void* FwRtGetData(FwRtDesc_t rtDesc);

/**
 * Define the function implementing the Initialization Action for the Notification
 * Procedure.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Initialization Action
 * is used which is a dummy action which corresponds to a situation where no
 * initialization action is performed by the Notification Procedure.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param initializeNotifPr the pointer to the function implementing the
 * Initialization Action for the Notification Procedure.
 */
void FwRtSetInitializeNotifPr(FwRtDesc_t rtDesc, FwRtAction_t initializeNotifPr);

/**
 * Define the function implementing the Finalization Action for the Notification
 * Procedure.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Finalization Action
 * is used which is a dummy action which corresponds to a situation where no
 * finalization action is performed by the Notification Procedure.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param finalizeNotifPr the pointer to the function implementing the
 * Finalization Action for the Notification Procedure.
 */
void FwRtSetFinalizeNotifPr(FwRtDesc_t rtDesc, FwRtAction_t finalizeNotifPr);

/**
 * Define the function implementing the Notification Logic for the Notification
 * Procedure.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Notification Logic
 * is used which is a dummy action which never skips a notification.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param implementNotifLogicPr the pointer to the function implementing the
 * Notification Logic for the Notification Procedure.
 */
void FwRtSetImplementNotifLogic(FwRtDesc_t rtDesc, FwRtAction_t implementNotifLogicPr);

/**
 * Define the function implementing the Initialization Action for the Activation
 * Procedure.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Initialization Action
 * is used which is a dummy action which corresponds to a situation where no
 * initialization action is performed by the Activation Procedure.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param initializeActivPr the pointer to the function implementing the
 * Initialization Action for the Activation Procedure.
 */
void FwRtSetInitializeActivPr(FwRtDesc_t rtDesc, FwRtAction_t initializeActivPr);

/**
 * Define the function implementing the Finalization Action for the Activation
 * Procedure.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Finalization Action
 * is used which is a dummy action which corresponds to a situation where no
 * finalization action is performed by the Activation Procedure.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param finalizeActivPr the pointer to the function implementing the
 * Finalization Action for the Activation Procedure.
 */
void FwRtSetFinalizeActivPr(FwRtDesc_t rtDesc, FwRtAction_t finalizeActivPr);

/**
 * Define the function implementing the logic to set up a notification for the
 * RT Container.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Set Up Notification
 * Function is used which is a dummy action which corresponds to a situation
 * where no notification set-up mechanism is implemented.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param setUpNotification the pointer to the function implementing the
 * Set Up Notification logic.
 */
void FwRtSetSetUpNotif(FwRtDesc_t rtDesc, FwRtAction_t setUpNotification);

/**
 * Define the function implementing the activation logic for the RT Container.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the Implementation Activation
 * Logic Function is used which is a dummy action which corresponds to a situation
 * where the container's functional behaviour is executed at every activation cycle.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param implementActivLogic the pointer to the function implementing the
 * container's activation logic.
 */
void FwRtSetImplementActivLogic(FwRtDesc_t rtDesc, FwRtAction_t implementActivLogic);

/**
 * Define the function which executes the functional behaviour associated to the
 * RT Container.
 * The argument function must conform to the <code>::FwRtAction_t</code> prototype.
 *
 * If this function is not called, the default value for the container's functional
 * behaviour is used.
 * This is a dummy action which corresponds to a situation where the
 * container's functional behaviour is terminated after the first execution.
 *
 * This function may only be called during the container's configuration process
 * (i.e. before <code>::FwRtInit</code> is called or after <code>::FwRtShutdown</code>
 * is called).
 * If it is called at any other time, it causes the container's state to be
 * put in error state <code>::rtConfigErr</code>.
 * @param rtDesc the RT Container descriptor.
 * @param execFuncBehaviour the pointer to the function implementing the
 * container's functional behaviour.
 */
void FwRtSetExecFuncBehaviour(FwRtDesc_t rtDesc, FwRtAction_t execFuncBehaviour);

#endif /* FWRT_CONFIG_H_ */
