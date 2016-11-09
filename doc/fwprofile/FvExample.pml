/* P&P Software GmbH - Copyright 2013 - All Rights Reserved         */
/*                                                                  */
/* Example of Formal Verification of a FW Profile Model             */
/*                                                                  */
/* Model of the actuator example described in section 6.5 of the    */
/* FW Profile Definition Document.                                  */

mtype = {INACTIVE};                      /* State common to all SMs */
mtype = {Start, Stop};                /* Commands common to all SMs */
mtype = {EMPTY, NOT_EMPTY, OVERFLOW};        /* States of Buffer SM */
mtype = {INIT, READY, BUSY};               /* States of Actuator SM */
mtype = {Send, Done, SwitchOff};         /* Commands of Actuator SM */
mtype = {Put, Pop};                        /* Commands of Buffer SM */
mtype = {STOPPED, STARTED, WAITING}; /* Actuator Cont. Proc. States */ 
mtype = {OFF, HW_INIT, HW_BUSY, HW_AVAIL}; /* States of Actuator HW */

mtype bufferState = INACTIVE;    /* State of Buffer SM              */
mtype actState = INACTIVE;       /* State of Actuator SM            */
mtype actContState = STOPPED;    /* State of Actuator Controller PR */ 
mtype actHwState = OFF;          /* State of Hardware Actuator      */
byte nItems = 0;                 /* Number of items in buffer       */   
bool shutdownReqPending = false;  
bool overflowHandled = false;
bool actReqDone = false;     
bool actContExec = false;    

/* ------------------------------------------------------------------ */
/* A call to this macro models the sending of a command to the Buffer */ 
/* State Machine. The buffer size is assumed to be equal to 3.        */
inline BufferSM(cmd) {
   if
   :: (bufferState==INACTIVE) && (cmd==Start) -> 
       bufferState = EMPTY;
       nItems = 0;
   :: (bufferState==EMPTY) && (cmd==Put) -> 
       nItems = 1;
       bufferState = NOT_EMPTY;
   :: (bufferState==NOT_EMPTY) && (cmd==Put) && (nItems<3) -> 
       nItems++;
   :: (bufferState==NOT_EMPTY) && (cmd==Put) && (nItems==3) -> 
       bufferState = OVERFLOW;
   :: (bufferState==NOT_EMPTY) && (cmd==Pop) && (nItems>1) -> 
       nItems--;
   :: (bufferState==NOT_EMPTY) && (cmd==Pop) && (nItems==1) -> 
       nItems = 0;
       bufferState = EMPTY;
   :: (bufferState!=INACTIVE) && (cmd==Stop) -> 
       bufferState = INACTIVE;
   :: else -> skip;
   fi;
} 

/* ------------------------------------------------------------------*/
/* A call to this macro models the sending of a command to the       */
/* Actuator State Machine.                                           */
inline ActuatorSM(cmd) {
   if
   :: (actState==INACTIVE) && (cmd==Start) -> 
       BufferSM(Start);           /* Send command Start to Buffer SM */
       actState = INIT;
       actHwState = HW_INIT;     /* Switch-On command to Actuator HW */
   :: (actState==INIT) && (cmd==Done) -> 
       actState = READY;
   :: (actState==READY) && (cmd==Send) -> 
       actState = BUSY;
       BufferSM(Pop);               /* Send command Pop to Buffer SM */
       actHwState = HW_BUSY;          /* Send command to Actuator HW */
   :: (actState==BUSY) && (cmd==Done) -> 
       actState = READY;
   :: (actState==READY) && (cmd==SwitchOff) -> 
       BufferSM(Stop);             /* Send command Stop to Buffer SM */
       actHwState = OFF;        /* Switch-Off command to Actuator HW */
       actState = INACTIVE;
   :: else -> skip;
   fi;
} 

/* ----------------------------------------------------------------- */
/* A call to this macro models the execution of the Actuator         */
/* Controller Procedure.                                             */
inline ActuatorControllerPR() {
   if
   :: (actContState==STOPPED) -> 
       skip;    
   :: (actContState==STARTED) -> 
       ActuatorSM(Start);                       /* Start Actuator SM */  
       actContState = WAITING;
   :: (actContState==WAITING) -> 
       if
       :: (bufferState==EMPTY) -> skip;
       :: (bufferState==NOT_EMPTY) ->
           ActuatorSM(Send);             /* Send Send to Actuator SM */
       :: (bufferState==OVERFLOW) ->
           overflowHandled = true;
           ActuatorSM(SwitchOff);   /* Send SwitchOff to Actuator SM */
           actContState = STOPPED;            /* Terminate procedure */
       fi;
       if
       :: (shutdownReqPending==true) && (bufferState==EMPTY) ->
          ActuatorSM(SwitchOff);  /* Send SwitchOff to Actuator SM */
          actContState = STOPPED;             /* Terminate procedure */
       :: else -> skip;
       fi;
   fi;
} 

/* ------------------------------------------------------------------*/
/* Process representing the Hardware Actuator.                       */
/* The actuator hardware responds to Switch-On and Send requests by  */
/* sending a Done command to the Actuator SM.                        */
active proctype HardwareActuator() {
 end1:
  do
  :: (actHwState == HW_INIT) -> 
      actHwState = HW_AVAIL; 
      ActuatorSM(Done);
  :: (actHwState == HW_BUSY) -> 
      actHwState = HW_AVAIL; 
      ActuatorSM(Done);
  od;
}

/* ------------------------------------------------------------------*/
/* Process representing the application software which starts the    */
/* Actuator Controller Procedure, then periodically executes it and  */
/* puts commands in the Actuator Buffer until it eventually stops the*/
/* Actuator Controller Procedure.                                    */
active proctype ApplicationSoftware() {
  actContState = STARTED;      /* Start Actuator Controller Procedure*/
 end2:
  do
  :: true -> 
     actReqDone = true; 
     BufferSM(Put); 
     actReqDone = false; 
  :: true -> 
     actContExec = true;
     ActuatorControllerPR();
     actContExec = false;
  :: (!shutdownReqPending) -> 
     shutdownReqPending = true;
  od;
}

/* -------------------------------------------------------------------- */
/* Define variables used to formulate never claims:                     */
#define p (actReqDone==false)   /* No Put request to Buffer is done     */
#define q (nItems==0)           /* There are no pending items in Buffer */
#define r (actState==INACTIVE)     /* Actuator SM is inactive           */
#define s (bufferState==INACTIVE)  /* Buffer SM is inactive             */
#define t (shutdownReqPending==true) /* A shutdown request has been made*/
#define u (actHwState==OFF)    /* The Hardware Actuator is switched off */
#define v (overflowHandled==true)    /* Overflow error has been handled */
#define w (actContExec==true)        /* The Act. Cont. PR is executed   */
#define x (actState==READY)          /* Actuator SM is READY            */
#define y (actState==BUSY)           /* Actuator SM is BUSY             */
#define z (bufferState==OVERFLOW)    /* Buffer SM is in OVERFLOW        */
#define a (actContState==WAITING)    /* Act. Cont. PR is active         */

/* The following formulas are used as (positive forms of) never claims  */
	
/* If no new commands are placed in the buffer, then, eventually,       */
/* all pending commands are sent to the actuator.                       */
ltl P1 {(<>[]p) -> (<>[]q)}

/* If the Actuator SM is inactive, then the Buffer SM is inactive too  */
ltl P2 {[](r->s)}

/* If a shutdown request is made, then, eventually, the Actuator SM    */
/* and the Buffer SM terminate and the Actuator HW is switched off     */
ltl P3 {[](t -> <>[](r && s && u))}

/* If, during normal operation (i.e. after the Actuator has completed */
/* its initialization and while the Controller Procedure is active),  */
/* the Buffer overflows and then the Actuator Controller Procedure    */
/* executes, then the overflow error is handled.                      */
ltl P4 {[]((z && a && (x || y) && <>w) -> <>v)}
