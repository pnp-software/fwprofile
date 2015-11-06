/**
 * State Machine Example for C1 Implementation of FW Profile.
 * This file contains a complete program to create, configure
 * and use a simple state machine with 2 states, 1 choice pseudo-state,
 * 5 transitions, 4 actions, and 2 guards.
 *
 * The state machine has the following characteristics:
 * - State S1 with one exit action Action2
 * - State S2 with one entry action Action4 and one do-Action Action3
 * - Transition from the Initial Pseudo-State (IPS) to S1 has a
 *   transition action Action1
 * - Transition from S1 to CPS1 has trigger Trigger1
 * - Transition from CPS1 to S1 has guard Guard1
 * - Transition from CPS2 to S2 has guard Guard2
 * - Transition from S2 to the Final Pseudo-State has trigger Trigger2,
 *   guard Guard2 and transition action Action2
 * .
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
#include "FwProfile/FwSmConstants.h"
#include "FwProfile/FwSmDCreate.h"
#include "FwProfile/FwSmConfig.h"
#include "FwProfile/FwSmCore.h"

/* Define the constants used to configure the state machine */
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
 * This program creates and configures a simple state machine and then sends a few
 * transition commands to it.
 * @return always returns EXIT_SUCCESS
 */
int main(void) {
	FwSmDesc_t smDesc;	/* The state machine descriptor */

	/* Create the state machine */
	smDesc = FwSmCreate(NSTATES, NCPS, NTRANS, NACTIONS, NGUARDS);

	/* Add the states and the choice pseudo-state to the state machine */
	FwSmAddState(smDesc, STATE_S1, N_OUT_OF_S1, NULL, &Action2, NULL, NULL);
	FwSmAddState(smDesc, STATE_S2, N_OUT_OF_S2, &Action4, NULL, &Action3, NULL);
	FwSmAddChoicePseudoState(smDesc, CPS1, N_OUT_OF_CPS1);

	/* Add the transitions to the state machine */
	FwSmAddTransIpsToSta(smDesc, STATE_S1, &Action1);
	FwSmAddTransStaToCps(smDesc, TRIGGER1, STATE_S1, CPS1, NULL, NULL);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S1, NULL, &Guard1);
	FwSmAddTransCpsToSta(smDesc, CPS1, STATE_S2, NULL, &Guard2);
	FwSmAddTransStaToFps(smDesc, TRIGGER2, STATE_S2, &Action2, &Guard2);

	/* Check that the SM is properly configured */
	if (FwSmCheck(smDesc) != smSuccess)
		printf("Example state machine is not properly configured...\n");

	/* Start the SM, send a few transition commands to it, and execute it */
	FwSmStart(smDesc);
	FwSmMakeTrans(smDesc, TRIGGER1);	/* Send transition command TRIGGER1 to SM 	*/
	FwSmExecute(smDesc);
	FwSmExecute(smDesc);
	FwSmExecute(smDesc);
	FwSmMakeTrans(smDesc, TRIGGER2);	/* Send transition command TRIGGER2 to SM 	*/

	return EXIT_SUCCESS;
}


