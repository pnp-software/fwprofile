/**
 * Procedure Example 1 for C1 Implementation of the FW Profile.
 *
 * This example shows how to create, configure and execute a simple
 * procedure. Procedures are best represented as UML2 activity diagrams.
 *
 * The procedure has: 4 action nodes, 1 decision node, 7 control flows,
 * 2 actions (each of which is used in two action nodes), and 3 guards
 * (one of which one is used in two control flows). The topology of the
 * procedure is as follows:
 * - The entry node is N1 and its action is Action1.
 * - Node N1 is connected to node N2 through a control flow with
 *   guard Guard1.
 * - The action of node N2 is Action2.
 * - Node N2 is connected to decision node D1 through a control flow
 *   with a guard which becomes true after three execution cycles.
 * - Decision node D1 is connected to action node A3 through guard
 *   Guard1 and to action node A4 through guard Guard2.
 * - The action of node N3 is Action1.
 * - The action of node N4 is Action2.
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
#include "FwProfile/FwPrConstants.h"
#include "FwProfile/FwPrDCreate.h"
#include "FwProfile/FwPrConfig.h"
#include "FwProfile/FwPrCore.h"

/* Define the constants used to configure the base state machine */
#define N_ANODES 4       /* The number of action nodes					*/
#define N_DNODES 1       /* The number of decision nodes				*/
#define N_FLOWS 7        /* The number of control flows					*/
#define N_ACTIONS 2      /* The number of actions 						*/
#define N_GUARDS 3       /* The number of control flow guards			*/
#define N1 1       		 /* The identifier of the first action node		*/
#define N2 2       		 /* The identifier of the second action node	*/
#define N3 3       		 /* The identifier of the third action node		*/
#define N4 4       		 /* The identifier of the fourth action node	*/
#define D1 1       		 /* The identifier of the decision node			*/
#define N_OUT_OF_D1 2    /* The number of control flows out of D1		*/

/* -------------------------------------------------------------------------*/
/** Implementation of Action1 */
void Action1(FwPrDesc_t prDesc) {
	printf("Action1 has been executed\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Implementation of Action1 */
void Action2(FwPrDesc_t prDesc) {
	printf("Action2 has been executed\n");
	return;
}

/* -------------------------------------------------------------------------*/
/** Implementation of Guard1 */
FwPrBool_t Guard1(FwPrDesc_t prDesc) {
	return 1;
}

/* -------------------------------------------------------------------------*/
/** Implementation of Guard2 */
FwPrBool_t Guard2(FwPrDesc_t prDesc) {
	return 0;
}

/* -------------------------------------------------------------------------*/
/**
 * Implementation of Guard on control flow from N2 to D1. This guard becomes
 * true when the procedure has been executed three times in the current node
 * (i.e. the procedure is held at the current node for three execution cycles)
 */
FwPrBool_t Guard3(FwPrDesc_t prDesc) {
	return (FwPrGetNodeExecCnt(prDesc)==3);
}

/* -------------------------------------------------------------------------*/
/**
 * Main program for the Procedure Example.
 * This program creates and configures the example procedure and then
 * executes it a few times.
 * @return always returns EXIT_SUCCESS
 */
int main(void) {
	FwPrDesc_t prDesc;		/* The descriptor of the procedure */

	/* Create the procedure descriptor */
	prDesc = FwPrCreate(N_ANODES, N_DNODES, N_FLOWS, N_ACTIONS, N_GUARDS);

	/* Configure the action nodes */
	FwPrAddActionNode(prDesc, N1, &Action1);
	FwPrAddActionNode(prDesc, N2, &Action2);
	FwPrAddActionNode(prDesc, N3, &Action1);
	FwPrAddActionNode(prDesc, N4, &Action2);

	/* Configure the decision node */
	FwPrAddDecisionNode(prDesc, D1, N_OUT_OF_D1);

	/* Configure the control flows */
	 FwPrAddFlowIniToAct(prDesc, N1, NULL);
	 FwPrAddFlowActToAct(prDesc, N1, N2, &Guard1);
	 FwPrAddFlowActToDec(prDesc, N2, D1, &Guard3);
	 FwPrAddFlowDecToAct(prDesc, D1, N3, &Guard1);
	 FwPrAddFlowDecToAct(prDesc, D1, N4, &Guard2);
	 FwPrAddFlowActToFin(prDesc, N3, NULL);
	 FwPrAddFlowActToFin(prDesc, N4, NULL);

	/* Check that the procedure is properly configured */
	if (FwPrCheck(prDesc) != prSuccess)
		printf("Procedure is not properly configured...\n");

	/* Start the procedure and execute it a few times */
	FwPrStart(prDesc);
	printf("Procedure is executed first time...\n");
	FwPrExecute(prDesc);
	printf("Procedure is executed second time...\n");
	FwPrExecute(prDesc);
	printf("Procedure is executed third time...\n");
	FwPrExecute(prDesc);
	printf("Procedure is executed fourth time...\n");
	FwPrExecute(prDesc);

	return EXIT_SUCCESS;
}


