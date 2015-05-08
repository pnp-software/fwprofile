/**
 * @file
 * @ingroup prGroup
 * Declaration of the execution interface for a FW Procedure.
 * The execution interface offers the functions to start and stop
 * a procedure and to execute a procedure.
 * The functions declared in this header file, take a procedure descriptor
 * as their first argument.
 * This represents the procedure upon which the functions operate.
 * 
 * The functions declared in this header file can only be used after a
 * procedure has been fully configured.
 * This is normally done using the configuration functions declared
 * in <code>FwPrConfig.h</code>.
 * The basic mode of use of the functions declared in this file is as follows:
 * -# The procedure is started with function <code>::FwPrStart</code>.
 * -# The procedure is executed with function
 *    <code>::FwPrExecute</code>.
 * -# The procedure is stopped with function <code>::FwPrStop</code>.
 * .
 * 
 * The functions declared in this header file assume that they are passed
 * a valid procedure descriptor representing a correctly configured
 * procedure.
 * Failure to comply with this assumption will result in undefined
 * behaviour.
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2011, All Rights Reserved
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

#ifndef FWPR_CORE_H_
#define FWPR_CORE_H_

#include "FwPrConstants.h"

/**
 * Start a procedure. The semantics of this operation is defined by the activity
 * diagram of the following figure
 * (this is figure 3.3-1 in the "FW Profile Definition Document"):
 * @image html PR_StartStop.png
 * @param prDesc the descriptor of the procedure to be started.
 */
void FwPrStart(FwPrDesc_t prDesc);

/**
 * Stop a procedure. The semantics of this operation is defined by the activity
 * diagram of the following figure
 * (this is figure 3.3-1 in the "FW Profile Definition Document"):
 * @image html SM_StartStop.png
 * @param prDesc the descriptor of the procedure to be started.
 */
void FwPrStop(FwPrDesc_t prDesc);

/**
 * Execute a procedure. The semantics of this function is defined
 * by the following activity diagram (this is taken from figure 3.3-2 in the "FW Profile
 * Definition Document"):
 * @image html PR_Execution.png
 * 
 * If the execution of the procedure passes through a decision node and there are two
 * or more control flows out of that node which have guards which evaluate to true,
 * the control flow to be taken is the one which was added first to the procedure.
 * 
 * The FW Profile stipulates that at least one of the control flows out of a
 * decision node must have a guard evaluating to true. This constraint is not enforced
 * by this function.
 * However, if the constraint is violated, the error code is set to
 * <code>#prFlowErr</code>.
 * The procedure, however, remains in a consistent state.
 * @param prDesc the descriptor of the procedure where the transition is triggered.
 */
void FwPrExecute(FwPrDesc_t prDesc);

/**
 * Run a procedure.
 * When a procedure is run, the procedure is first started, then it is executed
 * once, and then it is stopped.
 * @param prDesc the descriptor of the procedure where the transition is triggered.
 */
void FwPrRun(FwPrDesc_t prDesc);

/**
 * Return the identifier of the current action node in a procedure.
 * The following convention is used:
 * - If the procedure is in the STOPPED state, this function returns 0.
 * - If the procedure is in the STARTED state but has not yet executed
 *   any action (because the guard from the initial node is still false),
 *   this function returns -1.
 * - If the procedure is in the STARTED state and has already left the
 *   initial node, the procedure returns the identifier of the current node
 *   (a positive integer).
 * .
 * @param prDesc the descriptor of the procedure.
 * @return the identifier of the current action node of the procedure (or 0
 * if the procedure is at the initial node, or -1 if the procedure is in the
 * STOPPED state).
 */
FwPrCounterS1_t FwPrGetCurNode(FwPrDesc_t prDesc);

/**
 * Check whether the procedure is started.
 * @param prDesc the descriptor of the procedure.
 * @return 1 if the procedure is STARTED or 0 if it is STOPPED.
 */
FwPrBool_t FwPrIsStarted(FwPrDesc_t prDesc);

/**
 * Return the error code of the argument procedure.
 * The error code of a procedure holds either <code>#prSuccess</code> if the
 * procedure never encountered an error or else it holds the code of the last error
 * encountered by the procedure.
 * If the error code is different from <code>#prSuccess</code>, the behaviour of
 * the procedure is undefined.
 * @param prDesc the descriptor of the procedure.
 * @return either <code>#prSuccess</code> or the code of the last error encountered
 * by the procedure.
 */
FwPrErrCode_t FwPrGetErrCode(FwPrDesc_t prDesc);

/**
 * Return the Procedure Execution Counter.
 * The Procedure Execution Counter holds the number of execution cycles since
 * the procedure was started.
 * Note that the Procedure Execution Counter is not reset when the procedure
 * is stopped  (it is only reset when the procedure is started).
 * @param prDesc the descriptor of the procedure.
 * @return the value of the Procedure Execution Counter.
 */
FwPrCounterU3_t FwPrGetExecCnt(FwPrDesc_t prDesc);

/**
 * Return the Node Execution Counter.
 * The Node Execution Counter holds the number of execution cycles since
 * the current node was entered.
 * Note that the Node Execution Counter is not reset when the procedure
 * is stopped  (it is only reset when the procedure is started).
 * @param prDesc the descriptor of the procedure.
 * @return the value of the Node Execution Counter.
 */
FwPrCounterU3_t FwPrGetNodeExecCnt(FwPrDesc_t prDesc);

#endif /* FWPR_CORE_H_ */
