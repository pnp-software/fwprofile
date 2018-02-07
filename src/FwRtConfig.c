/**
 * @file
 * @ingroup rtGroup
 * Implements Configuration Functions of the RT Container.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2013, All Rights Reserved
 *
 * This file is part of the FW Profile.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#include "FwRtConfig.h"
#include "FwRtConstants.h"
#include <pthread.h>
#include <stdlib.h>

/**
 * Dummy function which always returns 1.
 * This function is used as the default for the procedure actions of the RT Container.
 * @param rtDesc the descriptor of the RT Container
 * @return this function always returns 1
 */
FwRtOutcome_t DummyAction(FwRtDesc_t rtDesc);

void FwRtReset(FwRtDesc_t rtDesc) {
  rtDesc->state               = rtContUninitialized;
  rtDesc->activPrStarted      = 0;
  rtDesc->notifPrStarted      = 0;
  rtDesc->errCode             = 0;
  rtDesc->execFuncBehaviour   = &DummyAction;
  rtDesc->finalizeActivPr     = &DummyAction;
  rtDesc->finalizeNotifPr     = &DummyAction;
  rtDesc->implementActivLogic = &DummyAction;
  rtDesc->implementNotifLogic = &DummyAction;
  rtDesc->initializeActivPr   = &DummyAction;
  rtDesc->initializeNotifPr   = &DummyAction;
  rtDesc->setUpNotification   = &DummyAction;
  rtDesc->notifCounter        = 0;
  rtDesc->pThreadAttr         = NULL;
  rtDesc->pCondAttr           = NULL;
  rtDesc->pMutexAttr          = NULL;
  rtDesc->rtData              = NULL;
}

/*--------------------------------------------------------------------------------------*/
void FwRtInit(FwRtDesc_t rtDesc) {
  int errCode;

  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }

  /* Initialize mutex attributes */
  if (rtDesc->pMutexAttr != NULL) {
    if ((errCode = pthread_mutexattr_init(rtDesc->pMutexAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexAttrInitErr;
      return;
    }
  }

  /* Initialize condition variable attributes */
  if (rtDesc->pCondAttr != NULL) {
    if ((errCode = pthread_condattr_init(rtDesc->pCondAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtCondAttrInitErr;
      return;
    }
  }

  /* Initialize thread attributes */
  if (rtDesc->pThreadAttr != NULL) {
    if ((errCode = pthread_attr_init(rtDesc->pThreadAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtThreadAttrInitErr;
      return;
    }
  }

  /* Initialize mutex */
  if ((errCode = pthread_mutex_init(&(rtDesc->mutex), rtDesc->pMutexAttr)) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexInitErr;
    return;
  }

  /* Initialize condition variable */
  if ((errCode = pthread_cond_init(&(rtDesc->cond), rtDesc->pCondAttr)) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtCondInitErr;
    return;
  }

  rtDesc->state = rtContStopped;
}

/*--------------------------------------------------------------------------------------*/
void FwRtShutdown(FwRtDesc_t rtDesc) {
  int errCode;

  /* Destroy mutex attributes */
  if (rtDesc->pMutexAttr != NULL) {
    if ((errCode = pthread_mutexattr_destroy(rtDesc->pMutexAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtMutexAttrDestroyErr;
      return;
    }
  }

  /* Destroy condition variable attributes */
  if (rtDesc->pCondAttr != NULL) {
    if ((errCode = pthread_condattr_destroy(rtDesc->pCondAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtCondAttrDestroyErr;
      return;
    }
  }

  /* Destroy thread attributes */
  if (rtDesc->pThreadAttr != NULL) {
    if ((errCode = pthread_attr_destroy(rtDesc->pThreadAttr)) != 0) {
      rtDesc->errCode = errCode;
      rtDesc->state   = rtThreadAttrDestroyErr;
      return;
    }
  }

  if ((errCode = pthread_cond_destroy(&(rtDesc->cond))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtCondDestroyErr;
    return;
  }

  if ((errCode = pthread_mutex_destroy(&(rtDesc->mutex))) != 0) {
    rtDesc->errCode = errCode;
    rtDesc->state   = rtMutexDestroyErr;
    return;
  }

  rtDesc->state = rtContUninitialized;
}

/*--------------------------------------------------------------------------------------*/
void FwRtSetPosixAttr(FwRtDesc_t rtDesc, pthread_attr_t* pThreadAttr, pthread_mutexattr_t* pMutexAttr,
                      pthread_condattr_t* pCondAttr) {

  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }

  rtDesc->pThreadAttr = pThreadAttr;
  rtDesc->pMutexAttr  = pMutexAttr;
  rtDesc->pCondAttr   = pCondAttr;
}

/*--------------------------------------------------------------------------------------*/
pthread_attr_t* FwRtGetActivThreadAttr(FwRtDesc_t rtDesc) {
  return rtDesc->pThreadAttr;
}

/*--------------------------------------------------------------------------------------*/
pthread_mutexattr_t* FwRtGetMutexAttr(FwRtDesc_t rtDesc) {
  return rtDesc->pMutexAttr;
}

/*--------------------------------------------------------------------------------------*/
pthread_condattr_t* FwRtGetCondAttr(FwRtDesc_t rtDesc) {
  return rtDesc->pCondAttr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetData(FwRtDesc_t rtDesc, void* rtData) {
  rtDesc->rtData = rtData;
}

/* -------------------------------------------------------------------------------------*/
void* FwRtGetData(FwRtDesc_t rtDesc) {
  return rtDesc->rtData;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetInitializeNotifPr(FwRtDesc_t rtDesc, FwRtAction_t initializeNotifPr) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->initializeNotifPr = initializeNotifPr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetFinalizeNotifPr(FwRtDesc_t rtDesc, FwRtAction_t finalizeNotifPr) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->finalizeNotifPr = finalizeNotifPr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetImplementNotifLogic(FwRtDesc_t rtDesc, FwRtAction_t implementNotifLogicPr) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->implementNotifLogic = implementNotifLogicPr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetInitializeActivPr(FwRtDesc_t rtDesc, FwRtAction_t initializeActivPr) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->initializeActivPr = initializeActivPr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetFinalizeActivPr(FwRtDesc_t rtDesc, FwRtAction_t finalizeActivPr) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->finalizeActivPr = finalizeActivPr;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetSetUpNotif(FwRtDesc_t rtDesc, FwRtAction_t setUpNotification) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->setUpNotification = setUpNotification;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetImplementActivLogic(FwRtDesc_t rtDesc, FwRtAction_t implementActivLogic) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->implementActivLogic = implementActivLogic;
}

/* -------------------------------------------------------------------------------------*/
void FwRtSetExecFuncBehaviour(FwRtDesc_t rtDesc, FwRtAction_t execFuncBehaviour) {
  if (rtDesc->state != rtContUninitialized) {
    rtDesc->state = rtConfigErr;
    return;
  }
  rtDesc->execFuncBehaviour = execFuncBehaviour;
}

/* -------------------------------------------------------------------------------------*/
FwRtOutcome_t DummyAction(FwRtDesc_t rtDesc) {
  (void)(rtDesc);
  return 1;
}
