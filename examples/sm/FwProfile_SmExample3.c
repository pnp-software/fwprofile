/**
 * State Machine Example for C1 Implementation of the FW Profile.
 *
 * This example demonstrates the extension process for state machines.
 * First, a base state machine is created and configured and then
 * a second state machine is derived from the first one.
 * The derived state machine overrides one action of its base state
 * machine but is otherwise a clone of its base.
 *
 * The base state machine has 2 states, no choice pseudo-state,
 * 3 transitions, 2 actions, and 1 guard. Its characteristics are:
 * - State S1 with one entry action Action1
 * - State S2 with one exit action Action2
 * - Transition from the Initial Pseudo-State (IPS) to S1
 * - Transition from S1 to S2 has trigger Trigger1 and guard Guard1
 * - Transition from S2 to S1 has trigger Trigger2
 * .
 * The derived state machine overrides the exit action of S2 with Action2_Der.
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

/* Define the constants used to configure the base state machine */
#define NSTATES 2        /* The number of states						*/
#define NCPS 0           /* The number of choice pseudo-states			*/
#define NTRANS 3         /* The number of transitions					*/
#define NACTIONS 2       /* The number of state and transition actions 	*/
#define NGUARDS 1        /* The number of transition guards				*/
#define STATE_S1 1       /* The identifier of the first state			*/
#define STATE_S2 2       /* The identifier of the second state			*/
#define N_OUT_OF_S1 1    /* The number of transitions out of state S1	*/
#define N_OUT_OF_S2 1    /* The number of transitions out of state S2	*/
#define TRIGGER1 1       /* The identifier of the first transition command  */
#define TRIGGER2 2       /* The identifier of the second transition command */


/* -------------------------------------------------------------------------*/
/** Entry action of state S1 */
void Action1(FwSmDesc_t smDesc) {
	printf("State S1 has been entered\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Exit action of state S2 in base state machine */
void Action2(FwSmDesc_t smDesc) {
	printf("State S2 in base state machine has been exited\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Exit action of state S2 in derived state machine (overrides Action1)    */
void Action2_Der(FwSmDesc_t smDesc) {
	printf("State S2 in derived state machine has been exited\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Guard on transition from S1 to state S2									*/
FwSmBool_t Guard1(FwSmDesc_t smDesc) {
	return 1;
}

/* -------------------------------------------------------------------------*/
/**
 * Main program for the State Machine Example.
 * This program creates and configures the example state machines and then
 * sends a few transition commands to it.
 * @return always returns EXIT_SUCCESS
 */
int main(void) {
	FwSmDesc_t smDesc;		/* The descriptor of the base state machine */
	FwSmDesc_t smDescDer;	/* The descriptor of the derived state machine */

	/* Create and configure the base state machine */
	smDesc = FwSmCreate(NSTATES, NCPS, NTRANS, NACTIONS, NGUARDS);
	FwSmAddState(smDesc, STATE_S1, N_OUT_OF_S1, &Action1, NULL, NULL, NULL);
	FwSmAddState(smDesc, STATE_S2, N_OUT_OF_S2, NULL, &Action2, NULL, NULL);
	FwSmAddTransIpsToSta(smDesc, STATE_S1, NULL);
	FwSmAddTransStaToSta(smDesc, TRIGGER1, STATE_S1, STATE_S2, NULL, &Guard1);
	FwSmAddTransStaToSta(smDesc, TRIGGER2, STATE_S2, STATE_S1, NULL, NULL);

	/* Check that the base SM is properly configured */
	if (FwSmCheck(smDesc) != smSuccess)
		printf("Base state machine is not properly configured...\n");

	/* Create the descriptor of the derived state machine */
	smDescDer = FwSmCreateDer(smDesc);

	/* Override Action2 with Action2_Der in the derived state machine */
	FwSmOverrideAction(smDescDer, &Action2, &Action2_Der);

	/* Check that the derived SM is properly configured */
	if (FwSmCheck(smDescDer) != smSuccess)
		printf("Derived state machine is not properly configured...\n");

	/* Start the base state machines and send a few transition commands to it */
	printf("Activation sequence for base state machine:\n");
	FwSmStart(smDesc);
	FwSmMakeTrans(smDesc, TRIGGER1);	/* Send transition command TRIGGER1 to SM 	*/
	FwSmMakeTrans(smDesc, TRIGGER2);	/* Send transition command TRIGGER2 to SM 	*/

	/* Send the same sequence of commands to the derived state machine */
	printf("\nActivation sequence for derived state machine:\n");
	FwSmStart(smDescDer);
	FwSmMakeTrans(smDescDer, TRIGGER1);	/* Send transition command TRIGGER1 to SM 	*/
	FwSmMakeTrans(smDescDer, TRIGGER2);	/* Send transition command TRIGGER2 to SM 	*/

	return EXIT_SUCCESS;
}


