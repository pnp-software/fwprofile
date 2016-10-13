/**
 * State Machine Example for C1 Implementation of the FW Profile.
 *
 * This example demonstrates the creation, configuration and use of
 * a state machine with another state machine embedded in one of its
 * states.
 *
 * The outer state machine has 2 states, 1 choice pseudo-state,
 * 5 transitions, 4 actions, and 2 guards. Its characteristics are:
 * - State S1 with one exit action Action2
 * - State S2 with one entry action Action4 and one do-Action Action3
 *   and a state machine embedded within it
 * - Transition from the Initial Pseudo-State (IPS) to S1 has a
 *   transition action Action1
 * - Transition from S1 to CPS1 has trigger Trigger1
 * - Transition from CPS1 to S1 has guard Guard1
 * - Transition from CPS2 to S2 has guard Guard2
 * - Transition from S2 to the Final Pseudo-State has trigger Trigger2,
 *   guard Guard2 and transition action Action2
 * .
 *
 * The embedded state machine is embedded in state S2. It has 2
 * states, 2 transitions, 2 actions and 1 guard. Its characteristics are:
 * - State ES1 with one entry action Action5
 * - Transition from the Initial Pseudo-State (IPS) to ES1 has no
 *   transition action
 * - Transition from ES1 to ES2 has trigger Trigger3
 * - State ES2 with one entry action Action6.
 * - There is no final pseudo-state
 * .
 *
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

#include <stdio.h>
#include <stdlib.h>

/* Include FW Profile functions */
#include "FwSmConstants.h"
#include "FwSmDCreate.h"
#include "FwSmConfig.h"
#include "FwSmCore.h"

/* Define the constants used to configure the outer state machine */
#define NSTATES 2        /* The number of states						*/
#define NCPS 1           /* The number of choice pseudo-states			*/
#define NTRANS 5         /* The number of transitions					*/
#define NACTIONS 4       /* The number of state and transition actions 	*/
#define NGUARDS 2        /* The number of transition guards				*/
#define STATE_S1 1       /* The identifier of the first state			*/
#define STATE_S2 2       /* The identifier of the second state			*/
#define N_OUT_OF_S1 1    /* The number of transitions out of state S1	*/
#define N_OUT_OF_S2 1    /* The number of transitions out of state S2	*/
#define CPS1 1           /* The identifier of the choice pseudo-state                 */
#define N_OUT_OF_CPS1 2  /* The number of transitions out of the choice-pseudo state  */
#define TRIGGER1 1       /* The identifier of the first transition command  */
#define TRIGGER2 2       /* The identifier of the second transition command */

/* Define the constants used to configure the embedded state machine */
#define E_NSTATES 2      /* The number of states						*/
#define E_NCPS 0         /* The number of choice pseudo-states			*/
#define E_NTRANS 2       /* The number of transitions					*/
#define E_NACTIONS 2     /* The number of state and transition actions 	*/
#define E_NGUARDS 0      /* The number of transition guards				*/
#define STATE_ES1 1      /* The identifier of the first state			*/
#define STATE_ES2 2      /* The identifier of the second state			*/
#define N_OUT_OF_ES1 1   /* The number of transitions out of state S1	*/
#define N_OUT_OF_ES2 0   /* The number of transitions out of state S2	*/
#define TRIGGER3 3       /* The identifier of the transition command    */

/* -------------------------------------------------------------------------*/
/** Action on transition from Initial Pseudo-State (IPS) to state S1 */
void Action1(FwSmDesc_t smDesc) {
	printf("Transition from IPS to state S1 has been executed\n");
	return;
}

/* -------------------------------------------------------------------------*/
/**
 * Exit action of state S1 and transition action on transition from state
 * S2 to Final Pseudo-State (FPS)
 */
void Action2(FwSmDesc_t smDesc) {
	printf("State S1 has been exited or transition to FPS has been executed\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Do-action of state S2    												*/
void Action3(FwSmDesc_t smDesc) {
	printf("State S2 has been executed\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Entry action of state S2    											*/
void Action4(FwSmDesc_t smDesc) {
	printf("State S2 has been entered\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Entry action of state ES1 in embedded state machine						*/
void Action5(FwSmDesc_t smDesc) {
	printf("State ES1 in embedded SM has been entered\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Entry action of state ES1 in embedded state machine						*/
void Action6(FwSmDesc_t smDesc) {
	printf("State ES2 in embedded SM has been entered\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Guard on transition from Choice Pseudo-State (CPS) to state S1			*/
FwSmBool_t Guard1(FwSmDesc_t smDesc) {
	return 0;
}

/* -------------------------------------------------------------------------*/
/**
 * Guard on transition from Choice Pseudo-State (CPS) to state S2 and from
 * state S2 to the Final Pseudo-State (FPS)
 */
FwSmBool_t Guard2(FwSmDesc_t smDesc) {
	return 1;
}

/* -------------------------------------------------------------------------*/
/**
 * Main program for the State Machine Example.
 * This program creates and configures the example state machine and then
 * sends a few transition commands to it.
 * @return always returns EXIT_SUCCESS
 */
int main(void) {
	FwSmDesc_t smDesc;	/* The descriptor of the outer state machine */
	FwSmDesc_t esmDesc;	/* The descriptor of the embedded state machine */

	/* Create and configure the embedded state machine */
	esmDesc = FwSmCreate(E_NSTATES, E_NCPS, E_NTRANS, E_NACTIONS, E_NGUARDS);
	FwSmAddState(esmDesc, STATE_ES1, N_OUT_OF_ES1, &Action5, NULL, NULL, NULL);
	FwSmAddState(esmDesc, STATE_ES2, N_OUT_OF_ES2, &Action6, NULL, NULL, NULL);
	FwSmAddTransIpsToSta(esmDesc, STATE_ES1, NULL);
	FwSmAddTransStaToSta(esmDesc, TRIGGER3, STATE_ES1, STATE_ES2, NULL, NULL);

	/* Check that the Embedded SM is properly configured */
	if (FwSmCheck(esmDesc) != smSuccess)
		printf("Embedded state machine is not properly configured...\n");

	/* Create and configure the outer state machine */
	smDesc = FwSmCreate(NSTATES, NCPS, NTRANS, NACTIONS, NGUARDS);
	FwSmAddState(smDesc, STATE_S1, N_OUT_OF_S1, NULL, &Action2, NULL, NULL);
	FwSmAddState(smDesc, STATE_S2, N_OUT_OF_S2, &Action4, NULL, &Action3, esmDesc);
	FwSmAddChoicePseudoState(smDesc, CPS1, N_OUT_OF_CPS1);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, &Action1);
	FwSmAddTransStaToCps(smDesc, TRIGGER1, STATE_S1, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, &Guard1);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S2, NULL, &Guard2);
	FwSmAddTransStaToFps(smDesc, TRIGGER2, STATE_S2, &Action2, &Guard2);

	/* Check that the outer SM is properly configured */
	if (FwSmCheck(smDesc) != smSuccess)
		printf("Outer state machine is not properly configured...\n");

	/* Start the SM, send a few transition commands to it, and execute it */
	FwSmStart(smDesc);
	FwSmMakeTrans(smDesc, TRIGGER1);	/* Send transition command TRIGGER1 to SM 	*/
	FwSmExecute(smDesc);
	FwSmExecute(smDesc);
	FwSmExecute(smDesc);
	FwSmMakeTrans(smDesc, TRIGGER3);	/* Send transition command TRIGGER3 to SM 	*/
	FwSmMakeTrans(smDesc, TRIGGER2);	/* Send transition command TRIGGER2 to SM 	*/

	return EXIT_SUCCESS;
}


