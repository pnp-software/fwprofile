/**
 * @file
 * @ingroup rtGroup
 * Implements Core Functions of the RT Container.
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

#include "FwRtCore.h"
#include "FwRtConstants.h"
#include <pthread.h>
#include <stdlib.h>

/**
 * The Activation Thread of the RT Container.
 * This function is called by the Activation Thread when it is created.
 * @param ptr this parameter is not used
 * @return this function always returns NULL
 */
void* ExecActivThread(void* ptr);

/**
 * Execute the loop in the Notification Procedure.
 * @param rtDesc the descriptor of the RT Container
 */
void ExecNotifProcedure(FwRtDesc_t rtDesc);

/**
 * Execute the loop in the Activation Procedure.
 * No check is performed on whether the activation procedure is started
 * because, by design, the Activation Procedure is only ever executed
 * if it is started.
 * @param rtDesc the descriptor of the RT Container
 */
void ExecActivProcedure(FwRtDesc_t rtDesc);

/*--------------------------------------------------------------------------------------*/
void FwRtStart(FwRtDesc_t rtDesc) {
  int errCode;

  if ((errCode = pthread_mutex_lock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexLockErr;
    return;
  }

  if (rtDesc->state != rtContStopped) {
    if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexUnlockErr;
      return;
    }
    return;
  }

  /* Start Notification Procedure */
  rtDesc->notifPrStarted = 1;
  /* Start Activation Procedure */
  rtDesc->activPrStarted = 1;

  /* Execute Notification and Activation Procedures. Since the procedures have just been
   * started, this is equivalent to executing their initialization actions and (for the
   * Activation Procedure) its Set Up Notification action. */
  rtDesc->initializeNotifPr(rtDesc);
  rtDesc->initializeActivPr(rtDesc);
  rtDesc->setUpNotification(rtDesc);

  rtDesc->state        = rtContStarted;
  rtDesc->notifCounter = 0;

  /* Create thread */
  if ((errCode = pthread_create(&(rtDesc->activationThread), rtDesc->pThreadAttr, ExecActivThread, rtDesc)) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtThreadCreateErr;
    return;
  }

  if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexUnlockErr;
    return;
  }

  return;
}

/*--------------------------------------------------------------------------------------*/
void FwRtStop(FwRtDesc_t rtDesc) {
  int errCode;

  if ((errCode = pthread_mutex_lock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexLockErr;
    return;
  }

  if (rtDesc->state != rtContStarted) {
    if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexUnlockErr;
      return;
    }
    return;
  }

  /* Stop the RT Container */
  rtDesc->state = rtContStopped;

  /* Notify the Activation Thread */
  rtDesc->notifCounter++;

  if ((errCode = pthread_cond_signal(&(rtDesc->cond))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtCondSignalErr;
    return;
  }

  if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexUnlockErr;
    return;
  }

  return;
}

/*--------------------------------------------------------------------------------------*/
void FwRtNotify(FwRtDesc_t rtDesc) {
  int errCode;

  if ((errCode = pthread_mutex_lock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexLockErr;
    return;
  }
  ExecNotifProcedure(rtDesc);
  if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexUnlockErr;
    return;
  }
}

/*--------------------------------------------------------------------------------------*/
void FwRtWaitForTermination(FwRtDesc_t rtDesc) {
  int   errCode;
  void* status = 0;

  if ((errCode = pthread_join(rtDesc->activationThread, &status)) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtJoinErr;
    return;
  }
}

/*--------------------------------------------------------------------------------------*/
FwRtBool_t FwRtIsNotifPrStarted(FwRtDesc_t rtDesc) {
  return rtDesc->notifPrStarted;
}

/*--------------------------------------------------------------------------------------*/
FwRtBool_t FwRtIsActivPrStarted(FwRtDesc_t rtDesc) {
  return rtDesc->activPrStarted;
}

/*--------------------------------------------------------------------------------------*/
FwRtState_t FwRtGetContState(FwRtDesc_t rtDesc) {
  return rtDesc->state;
}

/*--------------------------------------------------------------------------------------*/
int FwRtGetErrCode(FwRtDesc_t rtDesc) {
  return rtDesc->errCode;
}

/*--------------------------------------------------------------------------------------*/
FwRtCounterU2_t FwRtGetNotifCounter(FwRtDesc_t rtDesc) {
  return rtDesc->notifCounter;
}

/*--------------------------------------------------------------------------------------*/
void ExecNotifProcedure(FwRtDesc_t rtDesc) {
  int errCode;

  if (rtDesc->notifPrStarted == 0) {
    return;
  }

  if (rtDesc->activPrStarted == 0) {
    rtDesc->finalizeNotifPr(rtDesc);
    rtDesc->notifPrStarted = 0;
    return;
  }

  if (rtDesc->implementNotifLogic(rtDesc) == 1) {
    rtDesc->notifCounter++;
    if ((errCode = pthread_cond_signal(&(rtDesc->cond))) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtCondSignalErr;
      return;
    }
  }

  return;
}

/*--------------------------------------------------------------------------------------*/
void ExecActivProcedure(FwRtDesc_t rtDesc) {

  if (rtDesc->state == rtContStopped) {
    rtDesc->finalizeActivPr(rtDesc);
    rtDesc->activPrStarted = 0;
    return;
  }

  if (rtDesc->implementActivLogic(rtDesc) == 1) { /* Execute functional behaviour */
    if (rtDesc->execFuncBehaviour(rtDesc) == 1) { /* Functional behaviour is terminated */
      rtDesc->finalizeActivPr(rtDesc);
      rtDesc->activPrStarted = 0;
      return;
    }
  }
  rtDesc->setUpNotification(rtDesc);
  return;
}

/*--------------------------------------------------------------------------------------*/
void* ExecActivThread(void* ptr) {
  FwRtDesc_t rtDesc = (FwRtDesc_t)ptr;
  int        errCode;

  while (1) {
    if ((errCode = pthread_mutex_lock(&(rtDesc->mutex))) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexLockErr;
      return NULL;
    }
    while (rtDesc->notifCounter == 0) {
      if ((errCode = pthread_cond_wait(&(rtDesc->cond), &(rtDesc->mutex))) != 0) {
        rtDesc->errCode = errCode;
        rtDesc->state   = rtCondWaitErr;
        return NULL;
      }
    }
    rtDesc->notifCounter--;
    if ((errCode = pthread_mutex_unlock(&(rtDesc->mutex))) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexUnlockErr;
      return NULL;
    }

    ExecActivProcedure(rtDesc);

    if (rtDesc->activPrStarted == 0) {
      rtDesc->state = rtContStopped; /* Put RT Container in state STOPPED */
      FwRtNotify(rtDesc);            /* Execute Notification Procedure in mutual exclusion */
      break;
    }

    if (rtDesc->state == rtContStopped) {
      ExecActivProcedure(rtDesc);
      FwRtNotify(rtDesc); /* Execute Notification Procedure in mutual exclusion */
      break;
    }
  }

  return NULL;
}
