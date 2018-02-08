/**
 * @file
 * @ingroup tsRtGroup
 * Declaration of functions to create and configure test RT Containers.
 * The test RT Containers are used in the <i>test suite</i> to verify
 * the requirements of the FW Profile RT Container Module.
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

#ifndef FWPR_MAKETESTRT_H_
#define FWPR_MAKETESTRT_H_

#include "FwRtConstants.h"
#include "FwRtConfig.h"
#include "FwRtCore.h"

/** Maximum number of Test RT Container instances of type RT1 or RT2 */
#define MAX_RT_INDEX 10

/**
 * Type for the data structure passed to all RT Containers made by the functions declared
 * by this header file.
 * The counters are incremented by the actions in the container procedures and the flags
 * are used to determine the outcome of the guards of the container procedures.
 */
struct TestRtData {
	/** Counter incremented by the Initialization Action of the Notification Procedure. */
	int npInitCounter;
	/** Counter incremented by the Finalization Action of the Notification Procedure. */
	int npFinalCounter;
	/** Counter incremented by the Implement Notification Logic Action. */
	int npImplNotifLogicCounter;
	/** Flag determining outcome of Implement Notification Logic Action. */
	FwRtOutcome_t npImplNotifLogicFlag;
	/** Counter incremented by the Initialization Action of the Activation Procedure. */
	int apInitCounter;
	/** Counter incremented by the Finalization Action of the Activation Procedure. */
	int apFinalCounter;
	/** Counter incremented by the Setup Notification Action. */
	int apSetupNotifCounter;
	/** Counter increment by the Implement Activation Logic Action. */
	int apImplActivLogicCounter;
	/** Flag determining outcome of Implement Activation Logic Action. */
	FwRtOutcome_t apImplActivLogicFlag;
	/** Counter incremented by the Execute Functional Behaviour Action. */
	int apExecFuncBehaviourCounter;
	/** Flag determining the outcome of the Execute Functional Behaviour Action. */
	FwRtOutcome_t apExecFuncBehaviourFlag;
};

/**
 * This function resets the i-th instance of the Test RT Container RT1 and returns
 * a pointer to its descriptor.
 * The Test RT Container RT1 has the following characteristics:
 * - Each action in one of the container procedures increments the associated
 *   counter in the <code>::TestRtData</code> data structure.
 * - The outcome of the actions in the container procedures is determined by
 *   the flags in the <code>::TestRtData</code> data structure.
 * - Default values are used for the Activation Thread, mutex and condition
 *   variable attributes.
 * .
 * The function returns the RT Container with its counters set to zero and its
 * flags set to false.
 * @param i the index of the RT Container instance (an integer in the range 1 to
 * MAX_RT_INDEX).
 * @return the descriptor of the created RT Container or NULL if the argument is
 * out-of-range.
 */
FwRtDesc_t FwRtMakeTestRT1(unsigned int i);

/**
 * This function resets the i-th instance of the Test RT Container RT2 and returns
 * a pointer to its descriptor.
 * The Test RT Container RT2 has the following characteristics:
 * - Each action in one of the container procedures increments the associated
 *   counter in the <code>::TestRtData</code> data structure.
 * - The Execute Functional Behaviour action, in addition to incrementing a
 *   counter, also waits 1 millisecond before returning.
 * - The outcome of the actions in the container procedures is determined by
 *   the flags in the <code>::TestRtData</code> data structure.
 * - Default values are used for the Activation Thread, mutex and condition
 *   variable attributes.
 * .
 * The wait in the functional behaviour action is useful to simulate the case
 * of a container with a "long" execution time potentially giving rise to
 * an accumulation of unhandled notification requests.
 *
 * This function returns the RT Container with its counters set to zero and its
 * flags set to false.
 * @param i the index of the RT Container instance (an integer in the range 1 to
 * MAX_RT_INDEX).
 * @return the descriptor of the created RT Container or NULL if the argument is
 * out-of-range.
 */
FwRtDesc_t FwRtMakeTestRT2(unsigned int i);

/**
 * This function resets the i-th instance of the Test RT Container RT3 and returns
 * a pointer to its descriptor.
 * The Test RT Container RT3 has the following characteristics:
 * - The initialization and finalization actions increment the associated
 *   counter in the <code>::TestRtData</code> data structure.
 * - The Implement Notification Logic action either (with probability 50%) returns
 *   zero or (with probability 50%) it returns 1 and increments its associated
 *   counter.
 * - The Implement Activation Logic action increments its counter and then
 *   either returns zero (with probability 50%) or it returns one (with probability
 *   50%).
 * - The Implement Functional Behaviour action increments its counter, then it
 *   may wait 1 ms (with probability 50%), and then it returns zero.
 * - Default values are used for the Activation Thread, mutex and condition
 *   variable attributes.
 * .
 * This function returns the RT Container with its counters set to zero and its
 * flags set to false.
 * @param i the index of the RT Container instance (an integer in the range 1 to
 * MAX_RT_INDEX).
 * @return the descriptor of the created RT Container or NULL if the argument is
 * out-of-range.
 */
FwRtDesc_t FwRtMakeTestRT3(unsigned int i);

/**
 * This function resets the i-th instance of the Test RT Container RT4 and returns
 * a pointer to its descriptor.
 * The Test RT Container RT4 has the following characteristics:
 * - The initialization and finalization actions increment the associated
 *   counter in the <code>::TestRtData</code> data structure.
 * - The Implement Notification Logic action either (with probability 50%) returns
 *   zero or (with probability 50%) it returns 1 and increments its associated
 *   counter.
 * - The Implement Activation Logic action increments its counter and then
 *   either returns zero (with probability 50%) or it returns one (with probability
 *   50%).
 * - The Implement Functional Behaviour action increments its counter, then it
 *   may wait 1 ms (with probability 50%), and then it either returns zero
 *   (with probability 99%) or it returns one (with probability 1%).
 * - Default values are used for the Activation Thread, mutex and condition
 *   variable attributes.
 * .
 * This function returns the RT Container with its counters set to zero and its
 * flags set to false.
 * @param i the index of the RT Container instance (an integer in the range 1 to
 * MAX_RT_INDEX).
 * @return the descriptor of the created RT Container or NULL if the argument is
 * out-of-range.
 */
FwRtDesc_t FwRtMakeTestRT4(unsigned int i);

/**
 * This function resets the i-th instance of the Test RT Container RT5 and returns
 * a pointer to its descriptor.
 * The Test RT Container RT2 has the following characteristics:
 * - The initialization and finalization actions increment the associated
 *   counter in the <code>::TestRtData</code> data structure.
 * - The Implement Notification Logic increments its associated counter and
 *   returns the value of its associated flag.
 * - The Implement Notification Logic increments its associated counter and
 *   returns the value of its associated flag.
 * - The Implement Functional Behaviour action increments its counter, then it
 *   may wait 1 ms (with probability 50%), and then it either returns zero
 *   (with probability 99%) or it returns one (with probability 1%).
 * - Default values are used for the Activation Thread, mutex and condition
 *   variable attributes.
 * .
 * This function returns the RT Container with its counters set to zero and its
 * flags set to false.
 * @param i the index of the RT Container instance (an integer in the range 1 to
 * MAX_RT_INDEX).
 * @return the descriptor of the created RT Container or NULL if the argument is
 * out-of-range.
 */
FwRtDesc_t FwRtMakeTestRT5(unsigned int i);

#endif /* FWPR_MAKETESTRT_H_ */
