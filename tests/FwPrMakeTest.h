/**
 * @file
 * @ingroup tsPrGroup
 * Declaration of functions to create and configure test procedures.
 * The procedures are used in the <i>test suite</i> to verify
 * the requirements of the FW Procedure Module.
 * 
 * The data for the procedures defined in this file are instances of
 * a data structure of type <code>::TestPrData</code>.
 *
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

#ifndef FWPR_MAKETESTPR_H_
#define FWPR_MAKETESTPR_H_

#include "FwPrConstants.h"
#include "FwPrConfig.h"
#include "FwPrDCreate.h"
#include "FwPrSCreate.h"
#include "FwPrCore.h"

/** An action node identifier */
#define N1 1
/** An action node identifier */
#define N2 2
/** An action node identifier */
#define N3 3

/** A decision node identifier */
#define D1 1
/** A decision node identifier */
#define D2 2

/** Size of the log arrays */
#define LOG_ARRAY_SIZE 50

/**
 * Type for the data structure passed to all procedures made by the functions declared
 * by this header file.
 * The counters are incremented by the procedure actions and the flags are used to
 * determine the outcome of the procedure guards.
 * 
 * The actions attached to the test procedures write a "marker" and the current node of
 * their procedure to two log arrays (<code>logMarker</code> and <code>logNode</code>, these
 * arrays are defined as global variables in the c-file). The value of the marker is stored
 * in field <code>marker</code>.
 * Thus, when a procedure action is executed it does the following:
 * -# it writes the marker at <code>logMarker[logIndex]</code>;
 * -# it writes the current node identifier at <code>logNode[logIndex]</code>; and
 * -# it increments <code>logIndex</code> by 1.
 */
struct TestPrData {
	/** Counter incremented by the procedure actions. */
	int counter_1;
	/** Procedure's marker. */
	int marker;
	/** Flag used in the procedure control flows. */
	int flag_1;
	/** Flag used in the procedure control flows. */
	int flag_2;
	/** Flag used in the procedure control flows. */
	int flag_3;
	/** Flag used in the procedure control flows. */
	int flag_4;
	/** Flag used in the procedure control flows. */
	int flag_5;
	/** Flag used in the procedure control flows. */
	int flag_6;
};

/**
 * Operation to create the test procedure PR1.
 * This procedure has the following characteristics:
 * - Three action nodes N1, N2 and N3.
 * - No decision nodes.
 * - Four control flows.
 * - Node actions increment counter Counter_1 by 1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to N1: the guard returns the value of flag_1.
 * - Control flow from N1 to N2: the guard returns the value of flag_2.
 * - Control flow from N2 to N3: the guard returns the value of flag_3.
 * - Control flow from N3 to final node: the guard returns the value of flag_4.
 * .
 * @image html PR1.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR1(struct TestPrData* prData);

/**
 * Operation to create and configure the procedure PR1 (see <code>::FwPrMakeTestPR1</code>
 * and figure below) statically using the interface defined in <code>FwPrSCreate.h</code>.
 * @image html PR1.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR1Static(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR2.
 * This procedure has the following characteristics (see also figure below):
 * - Three action nodes N1, N2 and N3.
 * - Two decision nodes D1 and D2.
 * - Nine control flows.
 * - Node actions increment counter Counter_1 by 1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to N1: the guard returns the value of flag_1.
 * - Control flow from N1 to N2: the guard returns the value of flag_2.
 * - Control flow from N2 to D1: no guard is attached to this control flow.
 * - Control flow from D1 to final node: the guard returns the value of flag_3.
 * - Control flow from D1 to D2: the guard returns the value of flag_4.
 * - Control flow from D1 to N3: the guard returns the value of flag_5.
 * - Control flow from N3 to N2: the guard returns the value of flag_6.
 * - Control flow from D2 to N3: the guard returns true if Counter_1 is greater than 5.
 * - Control flow from D2 to final node: the guard returns true if Counter_1 is smaller than 6.
 * .
 * @image html PR2.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR2(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR2 (see <code>::FwPrMakeTestPR2</code>
 * and figure below) directly by instantiating and configuring its data
 * structures.
 * This function does not use any of the functions provided by the C1 Implementation
 * to create and configure a procedure.
 * Instead, it instantiated and directly configures the data structures
 * which define the procedure PR2.
 * This approach to creating a procedure is useful in applications which are
 * severely memory-constrained because it allowed them to do without the
 * <code>FwPrSCreate.h</code>, <code>FwPrDCreate.h</code>, and
 * <code>FwPrConfig.h</code>.
 * @image html PR2.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR2Dir(struct TestPrData* prData);

/**
 * Operation to create and configure the procedure PR2 (see <code>::FwPrMakeTestPR2</code>
 * and figure below) statically using the interface defined in <code>FwPrSCreate.h</code>.
 * @image html PR2.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR2Static(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR3.
 * This procedure has the following characteristics (see also figure below):
 * - One action nodes N1.
 * - no decision nodes.
 * - Two control flows.
 * - Node actions increment counter Counter_1 by 1.
 * - No guards on any control flow.
 * .
 * @image html PR3.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR3(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR4.
 * This procedure has the following characteristics (see also figure below):
 * - One action node N1.
 * - One decision node D1.
 * - Four control flows.
 * - Node action increments counter Counter_1 by 1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to D1: no guard is attached to this control flow.
 * - Control flow from D1 to N1: the guard returns the value of flag_1.
 * - Control flow from D1 to final node: the guard returns the value of flag_2.
 * - Control flow from D2 to final node: no guard is attached to this control flow.
 * .
 * @image html PR4.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR4(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR5.
 * This procedure has the following characteristics (see also figure below):
 * - Three action nodes N1, N2 and N3.
 * - No decision nodes.
 * - Four control flows.
 * - Node action assigns the value of the Procedure Execution Counter to Counter_1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to N1: no guard is attached to this control flow.
 * - Control flow from N1 to N2: the guard returns true after waiting for three cycles.
 * - Control flow from N2 to N3: the guard returns true after waiting for three cycles.
 * - Control flow from D3 to final node: no guard is attached to this control flow.
 * .
 * @image html PR5.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR5(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR6.
 * This procedure has the following characteristics (see also figure below):
 * - Three action nodes N1, N2 and N3.
 * - One decision node.
 * - Seven control flows.
 * - Node action increments the value of Counter_1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to D1: no guard is attached to this control flow.
 * - Control flow from D1 to N1: the guard returns the value of Flag_1.
 * - Control flow from D1 to N2: the guard returns the value of Flag_2.
 * - Control flow from D1 to N3: the guard returns the value of Flag_3.
 * - Control flow from N1 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N2 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N3 to Final Node: the guard returns the value of Flag_4.
 * .
 * The control flows are added to the procedure in the order in which they are listed
 * above.
 * @image html PR6.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR6_1(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR6.
 * This procedure has the following characteristics (see also figure below):
 * - Three action nodes N1, N2 and N3.
 * - One decision node.
 * - Seven control flows.
 * - Node action increments the value of Counter_1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to D1: no guard is attached to this control flow.
 * - Control flow from D1 to N3: the guard returns the value of Flag_3.
 * - Control flow from D1 to N2: the guard returns the value of Flag_2.
 * - Control flow from D1 to N1: the guard returns the value of Flag_1.
 * - Control flow from N1 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N2 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N3 to Final Node: the guard returns the value of Flag_4.
 * .
 * The control flows are added to the procedure in the order in which they are listed
 * above.
 * @image html PR6.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR6_2(struct TestPrData* prData);

/**
 * Operation to create the test procedure PR6.
 * This procedure has the following characteristics (see also figure below):
 * - Three action nodes N1, N2 and N3.
 * - One decision node.
 * - Seven control flows.
 * - Node action increments the value of Counter_1.
 * The control flows and their guards are defined as follows:
 * - Control flow from initial node to D1: no guard is attached to this control flow.
 * - Control flow from D1 to N2: the guard returns the value of Flag_2.
 * - Control flow from D1 to N3: the guard returns the value of Flag_3.
 * - Control flow from D1 to N1: the guard returns the value of Flag_1.
 * - Control flow from N1 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N2 to Final Node: the guard returns the value of Flag_4.
 * - Control flow from N3 to Final Node: the guard returns the value of Flag_4.
 * .
 * The control flows are added to the procedure in the order in which they are listed
 * above.
 * @image html PR6.png
 * @param prData the data structure upon which the procedure operates
 * @return the descriptor of the created procedure or NULL if the creation
 * or configuration of the procedure failed.
 */
FwPrDesc_t FwPrMakeTestPR6_3(struct TestPrData* prData);

/**
 * Create a derived procedure and override one of its actions and
 * one of its guards.
 * The base procedure is passed as an argument to the function.
 * The function extends the base procedure and then it performs the
 * following overrides on the derived procedure:
 * - action <code>incrCnt1By1</code> (increment Counter_1 by 1) is
 *   overridden with action <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The derived state machine is created using the dynamic creation
 * interface of <code>FwPrDCreate.h</code>.
 * @param prDescBase the base state machine
 * @param prData the data structure for the newly created state machine
 * @return the derived state machine
 */
FwPrDesc_t FwPrMakeTestPRDer1(FwPrDesc_t prDescBase, struct TestPrData* prData);

/**
 * Create a derived procedure and override one of its actions and
 * one of its guards.
 * The base procedure is passed as an argument to the function.
 * This function assumes that the base state machine has 1 action,
 * and 8 guards.
 * The function extends the base procedure and then it performs the
 * following overrides on the derived procedure:
 * - action <code>incrCnt1By1</code> (increment Counter_1 by 1) is
 *   overridden with action <code>incrCnt1By8</code> (increment Counter_1 by 8).
 * - guard <code>retFlag1</code> (return Flag_1) is overridden with guard
 *   <code>retFlag2</code> (return Flag_2).
 * .
 * The derived state machine is created using the static creation
 * interface of <code>FwPrSCreate.h</code>.
 * @param prDescBase the base state machine
 * @param prData the data structure for the newly created state machine
 * @return the derived state machine
 */
FwPrDesc_t FwPrMakeTestPRDer1Static(FwPrDesc_t prDescBase, struct TestPrData* prData);

#endif /* FWPR_MAKETESTPR_H_ */
