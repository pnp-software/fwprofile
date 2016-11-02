/* 
 * A. Pasetti - P&P Software GmbH - Copyright 2010 - All Rights Reserved
 * 
 * RT Container Model (single start/stop cycle)
 *
 * The following aspects of a RT Container are modelled:
 * 1. The Activation Thread 
 * 2. The Activation Procedure
 * 3. The Notification Procedure
 *
 * Of the actions in the Notification Procedure, only the "Implement Notification
 * Logic" action is modelled.
 * 
 * Of the actions in the Activation Procedure, only the "Execute Functional Behaviour
 * Action" is modelled.
 *
 * In addition to modelling the RT Container itself, the following aspects
 * of its environment are modelled:
 * 1. A process representing a source of a Start command followed by a Stop command
 * 2. A process representing a source of Notification Requests 
 */

mtype = {STARTED, STOPPED};
mtype RTContState = STOPPED;  
mtype activProcState = STOPPED;
mtype notifProcState = STOPPED;
byte notifCounter = 0;
bool notifyActivThread = false;
bool activationThreadReleased = false;

/* A call to this macro corresponds to one execution of the loop in the 
 * Notification Procedure. 
 * The Notification Procedure is started and stopped by setting notifProcState 
 * to, respectively, STARTED and STOPPED.
 */
inline NotifProc() {
   if
   :: (notifProcState==STOPPED) -> skip;
   :: else ->
      if
      :: (activProcState==STOPPED) -> 
         notifProcState = STOPPED;    /* Terminate procedure */
      :: else -> /* Activation procedure is still running */
         if
         :: true ->    /* Activation Thread should be notified */
            notifyActivThread = true; 
         :: true ->    /* Notification of Activ. Thread should be skipped */
            notifyActivThread = false; 
         fi;
         if
         :: notifyActivThread -> notifCounter++; 
         :: else -> skip;
         fi;
      fi;
   fi;
} 

/* A call to this macro corresponds to one execution of the loop in the
 * Activation Procedure.
 * The Activation Procedure is started and stopped by setting activProcState to,
 * respectively, STARTED and STOPPED.
 */
inline ActivProc() {
   if
   :: (activProcState==STOPPED) -> skip;
   :: else ->
      if
      :: (RTContState==STOPPED) -> /* RT Container has been stopped */
         activProcState = STOPPED; /* Terminate procedure */
      :: (RTContState!=STOPPED) -> /* RT Container is still running */
         if 
         :: true ->                   /* Functional behaviour has terminated */
            activProcState = STOPPED; /* Terminate procedure */
         :: true -> skip;             /* Functional behaviour has not yet terminated */
         fi;
      fi;
   fi;
}

/* Process representing source of start/stop requests for RT Container  */
active proctype Environment() {
  /* Start the RT Container (this also releases the Activation Thread
   * and starts the Notification and Activation Procedures) */
  atomic{notifProcState = STARTED; 
         activProcState = STARTED;}
  RTContState = STARTED; 
  /* Stop the RT Container (this also notifies the Activation Thread */
  RTContState = STOPPED;
  notifCounter++; 
}

/* Process representing the thread executing the notification procedure.
 * This model assumes that only up to 2 pending notifications can be 
 * buffered.
 */
active proctype NotificationThread() {
 end1:
  do
  :: (notifCounter<3) -> NotifProc(); 
  :: true -> skip;
  od;
}

/* Process representing the Activation Thread */
active proctype ActivationThread() {

  /* Wait until RT Container is started and released */
 end2:
  (RTContState==STARTED);       
  activationThreadReleased = true;

  do
  :: atomic { (notifCounter>0) -> notifCounter--; }
        ActivProc();   /* Execute Activation Procedure */
        if      /* Check if Activation Procedure has terminated */
        :: (activProcState==STOPPED) -> RTContState = STOPPED; 
                                        NotifProc();      
                                        break;
        :: (activProcState==STARTED) -> skip;
        fi;
        if     /* Check if RT Container is stopped */
        :: (RTContState==STOPPED) -> ActivProc(); 
                                     NotifProc();
                                     break;
        :: (RTContState==STARTED) -> skip;
        fi;
  od;
}

/* Define variables used to formulate never claims */
#define p (RTContState==STOPPED)		/* RT Container is stopped */
#define q (activProcState==STOPPED)		/* Activation Procedure is stopped */
#define r (notifProcState==STOPPED)		/* Notification Procedure is stopped */
#define s (activProcState==STARTED)		/* Activation Procedure is started */
#define t (notifProcState==STARTED)		/* Notification Procedure is started */
#define u (notifyActivThread==false)	/* No notification requested by Notif. Procedure */
#define v (notifCounter==0)				/* All notification requests consumed by Activ. Thread */
#define z (activationThreadReleased==true)  /* Activation thread has been released */

/* The following formulas are used as (positive forms of) never claims */
	
/* If the RT Container is stopped after the activation thread has been released, then 
 * eventually the Activation Procedure terminates. */
ltl P4 {[] ( (p && z) -> <>  q )}

/* If the Activation Procedure is started and then it is terminated, 
 * then eventually the RT Container is stopped. */
ltl P5 {[] (!q  ->  [] (q -> <>  p ))}

/* If the Activation Procedure terminates, then eventually the Notification Procedure terminates. */
ltl P6 {[] (q -> <>  r )}

/* If the Activation Procedure is running, then the Notification Procedure is running too. */
ltl P7 {[] (s -> t )}

/* If notifications cease and RT Container and Activation Procedure remain active, 
 * then eventually all notifications are consumed by Activation Thread. */
ltl P8 {<>[] (u && !p && q) -> <>[] v }
