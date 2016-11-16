/**
 * @file
 * @ingroup rtGroup
 * Header file to define all constants and types for the RT Container Module
 * of the FW Profile.
 * This header file should be included by all applications which use the
 * RT Container Module of the FW Profile.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2013, All Rights Reserved
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

#ifndef FWRT_CONSTANTS_H_
#define FWRT_CONSTANTS_H_

#include <pthread.h>

/**
 * Forward declaration for the pointer to a RT Container Descriptor.
 * A RT Container Descriptor is a data structure which holds all the information
 * describing a RT Container instance.
 */
typedef struct FwRtDesc* FwRtDesc_t;

/** Type used for booleans (0 is "false" and 1 is "true"). */
typedef int FwRtBool_t;

/** Type used for the outcome of a container action. */
typedef int FwRtOutcome_t;

/** Type used for unsigned integers with a "medium" range. */
typedef short int FwRtCounterU2_t;

/**
 * Type for a pointer to a container action.
 * A container action is a function which encapsulates an action executed by
 * either the Notification Procedure or the Activation Procedure of a
 * RT Container.
 * Container actions return an outcome.
 * The semantics of the outcome depends on the kind of action.
 */
typedef FwRtOutcome_t (*FwRtAction_t)(FwRtDesc_t);

/**
 * Enumerated type for the state of the RT Container.
 * The only nominal values are <code>rtContStopped</code> and <code>rtContStarted</code>.
 * If the container is not in one of these two states, its behaviour is undefined.
 * All other values indicate that an error has been reported by a system call.
 * The error code reported by the system call is stored in the error code field of
 * the RT Container Descriptor.
 */
typedef enum {
  /** The RT Container has not yet been initialized */
  rtContUninitialized = 0,
  /** The RT Container is in state STOPPED */
  rtContStopped = 3,
  /** The RT Container is in state STARTED */
  rtContStarted = 4,
  /** The function to create the Activation Thread has reported an error */
  rtThreadCreateErr = 5,
  /** The function to initialize the container mutex has reported an error */
  rtMutexInitErr = 6,
  /** The function to destroy the container mutex has reported an error */
  rtMutexDestroyErr = 7,
  /** The function to initialize the container condition has reported an error */
  rtCondInitErr = 8,
  /** The function to destroy the container condition has reported an error */
  rtCondDestroyErr = 9,
  /** The function to lock the container mutex has reported an error */
  rtMutexLockErr = 10,
  /** The function to unlock the container mutex has reported an error */
  rtMutexUnlockErr = 11,
  /** The function to signal a condition has reported an error */
  rtCondSignalErr = 12,
  /** The function to wait on a condition has reported an error */
  rtCondWaitErr = 13,
  /** The function to initialize a mutex attribute has reported an error */
  rtMutexAttrInitErr = 14,
  /** The function to initialize a mutex attribute has reported an error */
  rtCondAttrInitErr = 15,
  /** The function to initialize a thread attribute has reported an error */
  rtThreadAttrInitErr = 16,
  /** The function to destroy a mutex attribute has reported an error */
  rtMutexAttrDestroyErr = 17,
  /** The function to destroy a mutex attribute has reported an error */
  rtCondAttrDestroyErr = 18,
  /** The function to destroy a thread attribute has reported an error */
  rtThreadAttrDestroyErr = 19,
  /** The function to wait on a thread join has reported an error */
  rtJoinErr = 20,
  /** A configuration function has been called during the container's normal operation
   * (i.e. after <code>::FwRtInit</code> has been called but before <code>::FwRtShutdown</code>
   * is called) */
  rtConfigErr = 21
} FwRtState_t;

typedef enum { rtSampleEnumItem } FwRtSampleEnum;

/**
 * Structure representing a RT Container Descriptor.
 * The RT Container Descriptor holds all the information which describes a RT
 * Container.
 * This consists of:
 * - The STARTED/STOPPED state of the RT Container;
 * - The activation thread;
 * - The lock (a mutex) associated to the RT Container;
 * - The state of the Notification and Activation Procedures associated to the
 *   container;
 * - The pointers to the functions which implement the adaptation points of
 *   the container;
 * - The outcome of the last action executed by the Notification Procedure;
 * - The outcome of the last action executed by the Activation Procedure;
 * - The error code for the RT Container;
 * - A pointer to a user-defined data structure to hold the container's data.
 * .
 * The STARTED/STOPPED state of the RT Container is not stored explicitly
 * because a RT Container is started if and only if its pthread object is
 * non-NULL.
 */
struct FwRtDesc {
  /** The state of the RT Container. */
  FwRtState_t state;
  /**
   * The pointer to the Activation Thread attributes.
   * The default value of NULL indicates the default values of a pthread
   * attributes should be used.
   */
  pthread_attr_t* pThreadAttr;
  /**
   * The pointer to the mutex attributes.
   * The default value of NULL indicates the default values of a pthread
   * mutex attributes should be used.
   */
  pthread_mutexattr_t* pMutexAttr;
  /**
   * The pointer to the condition variable attributes.
   * The default value of NULL indicates the default values of a pthread
   * condition variable attributes should be used.
   */
  pthread_condattr_t* pCondAttr;
  /** The thread associated to the RT Container. */
  pthread_t activationThread;
  /** The mutex associated to the RT Container. */
  pthread_mutex_t mutex;
  /** The condition variable associated to the RT Container */
  pthread_cond_t cond;
  /** The notification counter */
  FwRtCounterU2_t notifCounter;
  /**
   * Pointer to the function encapsulating the initialization action for the
   * Notification Procedure.
   * This is a user-defined function.
   * This function is called once when the Notification Procedure is executed
   * for the first time.
   * It should perform any initialization action which is required by the
   * Notification Procedure.
   * The function always returns a value of 1.
   */
  FwRtAction_t initializeNotifPr;
  /**
   * Pointer to the function encapsulating the finalization action for the
   * Notification Procedure.
   * This is a user-defined function.
   * This function is called just before the Notification Procedure terminates.
   * It should perform any finalization action which is required by the
   * Notification Procedure.
   * The function always returns a value of 1.
   */
  FwRtAction_t finalizeNotifPr;
  /**
   * Pointer to the function encapsulating the implementation of the notification
   * logic.
   * This is a user-defined function.
   * After the Notification Procedure has been executed for the first time,
   * this function is called every time the procedure is executed as long as
   * the Notification Procedure has not been stopped.
   * This procedure should implement the logic which decides whether the Activation
   * Thread should be notified.
   * The function returns 1 if the Activation Thread is to be notified or it returns
   * 0 if the notification of the Activation Thread should be skipped.
   */
  FwRtAction_t implementNotifLogic;
  /**
   * Pointer to the function encapsulating the initialization action for the
   * Activation Procedure.
   * This is a user-defined function.
   * This function is called once when the Activation Procedure is executed
   * for the first time.
   * It should perform any initialization action which is required by the
   * Activation Procedure.
   * The function always returns a value of 1.
   */
  FwRtAction_t initializeActivPr;
  /**
   * Pointer to the function encapsulating the finalization action for the
   * Activation Procedure.
   * This is a user-defined function.
   * This function is called just before the Activation Procedure terminates.
   * It should perform any finalization action which is required by the
   * Activation Procedure.
   * The function always returns a value of 1.
   */
  FwRtAction_t finalizeActivPr;
  /**
   * Pointer to the function encapsulating the logic to set up the notification
   * for the RT Container.
   * This is a user-defined function.
   * This function is called each time the Activation Procedure is executed.
   * This function may be used to implement the logic which determines when the
   * next notification is sent to the RT Container.
   * The function always returns a value of 1.
   */
  FwRtAction_t setUpNotification;
  /**
   * Pointer to the function encapsulating the implementation of the activation
   * logic.
   * This is a user-defined function.
   * After the Activation Procedure has been executed for the first time,
   * this function is called every time the procedure is executed as long as
   * the RT Container has not been stopped.
   * This procedure should implement the logic which decides whether the Functional
   * Behaviour associated to the RT Container is executed.
   * The function returns 1 if the container's functional behaviour should be executed
   * and it returns 0 if the container's functional behaviour should not be executed.
   */
  FwRtAction_t implementActivLogic;
  /**
   * Pointer to the function encapsulating the execution of the functional behaviour
   * associated to the RT Container.
   * This is a user-defined function.
   * After the Activation Procedure has been executed for the first time,
   * this function is called every time the procedure is executed as long as
   * the RT Container has not been stopped.
   * This procedure should implement the functional behaviour associated to the
   * RT Container.
   * The function returns 1 if the execution of the functional behaviour has terminated
   * and it returns 0 if the execution of the functional behaviour is not yet terminated.
   */
  FwRtAction_t execFuncBehaviour;
  /** The flag indicating whether the Notification Procedure is STÂRTED. */
  FwRtBool_t notifPrStarted;
  /** The flag indicating whether the Activation Procedure is STÂRTED. */
  FwRtBool_t activPrStarted;
  /** The return value of the last system call which failed. */
  int errCode;
  /** The pointer to the RT Container data. */
  void* rtData;
};

#endif /* FWRT_CONSTANTS_H_ */
