/**
 * @file
 * @ingroup smGroup
 * Implements the auxiliary functions of the FW State Machine Module.
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

#include "FwSmAux.h"
#include "FwSmConfig.h"
#include "FwSmPrivate.h"

/* ------------------------------------------------------------------------------- */
void FwSmPrintConfig(FwSmDesc_t smDesc, FILE *stream) {
	char prefix[1] = "";
	FwSmCounterS1_t actNOfPStates = 0;
	FwSmCounterS1_t actNOfCStates = 0;
	FwSmCounterS1_t actNOfTrans = 0;
	FwSmCounterS1_t actNOfEsm = 0;
	FwSmCounterS1_t actNOfActions = 0;
	FwSmCounterS1_t actNOfGuards = 0;
	FwSmCounterS1_t i, j, baseLoc;
	SmTrans_t trans;

	if (smDesc == NULL) {
		fprintf(stream, "%sThe argument state machine descriptor is NULL\n", prefix);
		return;
	}

	for (i=0; i<smDesc->smBase->nOfPStates; i++)
		if (smDesc->smBase->pStates[i].outTransIndex != 0) actNOfPStates++;

	for (i=0; i<smDesc->smBase->nOfCStates; i++)
		if (smDesc->smBase->cStates[i].outTransIndex != 0) actNOfCStates++;

	for (i=0; i<smDesc->smBase->nOfTrans; i++)
		if (smDesc->smBase->trans[i].iTrAction != -1) actNOfTrans++;

	for (i=0; i<smDesc->smBase->nOfPStates; i++)
		if (smDesc->esmDesc[i] != NULL) actNOfEsm++;

	for (i=0; i<smDesc->nOfActions; i++)
		if (smDesc->smActions[i] != NULL) actNOfActions++;

	for (i=0; i<smDesc->nOfGuards; i++)
		if (smDesc->smGuards[i] != NULL) actNOfGuards++;

	fprintf(stream, "\n");
	fprintf(stream, "%sSTATE MACHINE SIZE\n", prefix);
	fprintf(stream, "%s------------------\n", prefix);
	fprintf(stream, "%sDeclared number of states              : %d\n", prefix, smDesc->smBase->nOfPStates);
	fprintf(stream, "%sDeclared number of choice pseudo-states: %d\n", prefix, smDesc->smBase->nOfCStates);
	fprintf(stream, "%sDeclared number of transitions         : %d\n", prefix, smDesc->smBase->nOfTrans);
	fprintf(stream, "%sDeclared number of actions             : %d\n", prefix, smDesc->nOfActions-1);
	fprintf(stream, "%sDeclared number of guards              : %d\n", prefix, smDesc->nOfGuards-1);
	fprintf(stream, "\n");

	fprintf(stream, "%sSTATE MACHINE CONFIGURATION\n", prefix);
	fprintf(stream, "%s---------------------------\n", prefix);
	fprintf(stream, "%sThe error code is                        : %s\n", prefix, FwSmPrintErrCode(smDesc->errCode));
	fprintf(stream, "%sThe SM Execution Counter is               : %d\n", prefix, smDesc->smExecCnt);
	fprintf(stream, "%sThe State Execution Counter is            : %d\n", prefix, smDesc->stateExecCnt);
	fprintf(stream, "%sThe outcome of the configuration check is: %s\n", prefix, FwSmPrintErrCode(FwSmCheck(smDesc)));
	fprintf(stream, "%sNumber of configured states              : %d\n", prefix, actNOfPStates);
	fprintf(stream, "%sNumber of configured choice pseudo-states: %d\n", prefix, actNOfCStates);
	fprintf(stream, "%sNumber of configured transitions         : %d\n", prefix, actNOfTrans);
	fprintf(stream, "%sNumber of configured actions             : %d\n", prefix, actNOfActions-1);
	fprintf(stream, "%sNumber of configured guards              : %d\n", prefix, actNOfGuards-1);
	fprintf(stream, "%sNumber of embedded state machines        : %d\n", prefix, actNOfEsm);
	if (FwSmIsStarted(smDesc) == 0)
		fprintf(stream, "%sCurrent state machine state is           : STOPPED\n", prefix);
	else
		fprintf(stream, "%sState machine is STARTED and is in state : %d\n", prefix, FwSmGetCurState(smDesc));
	fprintf(stream, "\n");

	fprintf(stream, "%sCONFIGURATION OF INITIAL PSEUDO STATE\n", prefix);
	fprintf(stream, "%s-------------------------------------\n", prefix);

	if (smDesc->smBase->trans[0].iTrAction == -1)
		fprintf(stream, "%sThe transition from the initial pseudo-state is not defined\n", prefix);
	else {
		if (smDesc->smBase->trans[0].dest > 0)
			fprintf(stream, "%sThe transition from the initial pseudo state is to state n. %d\n",
					prefix, smDesc->smBase->trans[0].dest);
		else if (smDesc->smBase->trans[0].dest < 0)
			fprintf(stream, "%sThe transition from the initial pseudo state is to choice pseudo-state n. %d\n",
					prefix, -smDesc->smBase->trans[0].dest);
		else
			fprintf(stream, "%sThe transition from the initial pseudo state is to the final pseudo-state.\n", prefix);

		if (smDesc->smBase->trans[0].iTrAction != 0)
			fprintf(stream, "\tTransition Action is action n. %d\n", smDesc->smBase->trans[0].iTrAction);
		else
			fprintf(stream, "\tNo Transition Action\n");

		fprintf(stream, "\tNo Transition Guard\n");
	}
	fprintf(stream, "\n");

	if (smDesc->smBase->nOfPStates > 0) {
		fprintf(stream, "%sCONFIGURATION OF STATES\n", prefix);
		fprintf(stream, "%s-----------------------\n", prefix);
	}

	for (i=0; i<smDesc->smBase->nOfPStates; i++) {
		if (smDesc->smBase->pStates[i].outTransIndex == 0)
			fprintf(stream, "%sState %d is not defined\n", prefix, i+1);
		else {
			fprintf(stream, "%sState %d:\n", prefix, i+1);

			if (smDesc->smBase->pStates[i].iEntryAction != 0)
				fprintf(stream, "\tEntry Action: action n. %d\n", smDesc->smBase->pStates[i].iEntryAction);
			else
				fprintf(stream, "\tNo entry action\n");

			if (smDesc->smBase->pStates[i].iDoAction != 0)
				fprintf(stream, "\tDo-Action: action n. %d\n", smDesc->smBase->pStates[i].iDoAction);
			else
				fprintf(stream, "\tNo do-action\n");

			if (smDesc->smBase->pStates[i].iExitAction != 0)
				fprintf(stream, "\tExit Action: action n. %d\n", smDesc->smBase->pStates[i].iExitAction);
			else
				fprintf(stream, "\tNo exit action\n");

			if (smDesc->esmDesc[i] == NULL)
				fprintf(stream, "\tNo state machine is embedded in this state\n");
			else
				fprintf(stream, "\tA state machins is embedded in this state\n");

			baseLoc = smDesc->smBase->pStates[i].outTransIndex;
			for (j=baseLoc; j<(baseLoc+smDesc->smBase->pStates[i].nOfOutTrans); j++) {
				trans = smDesc->smBase->trans[j];
				if (trans.id == FW_TR_EXECUTE) {
					if (trans.dest > 0)
						fprintf(stream, "\t'Execute' transition to state %d\n", trans.dest);
					else if (trans.dest < 0)
						fprintf(stream, "\t'Execute' transition to choice pseudo-state %d\n", -trans.dest);
					else
						fprintf(stream, "\t'Execute' transition to final pseudo-state\n");
				} else {
					if (trans.dest > 0)
						fprintf(stream, "\tTransition %d to state %d\n", trans.id, trans.dest);
					else if (trans.dest < 0)
						fprintf(stream, "\tTransition %d to choice pseudo-state %d\n", trans.id, -trans.dest);
					else
						fprintf(stream, "\tTransition %d to final pseudo-state\n", trans.id);
				}

				if (trans.iTrAction != 0)
					fprintf(stream, "\t\tTransition Action is action n. %d\n", trans.iTrAction);
				else
					fprintf(stream, "\t\tNo Transition Action\n");

				if (trans.iTrGuard != 0)
					fprintf(stream, "\t\tTransition Guard is guard n. %d\n", trans.iTrGuard);
				else
					fprintf(stream, "\t\tNo Transition Guard\n");
			}
		}
	}
	fprintf(stream, "\n");

	if (smDesc->smBase->nOfCStates > 0) {
		fprintf(stream, "%sCONFIGURATION OF CHOICE PSEUDO-STATES\n", prefix);
		fprintf(stream, "%s-------------------------------------\n", prefix);
	}

	for (i=0; i<smDesc->smBase->nOfCStates; i++) {
		if (smDesc->smBase->cStates[i].outTransIndex == 0)
			fprintf(stream, "%sChoice Pseudo-State %d is not defined\n", prefix, i+1);
		else {
			fprintf(stream, "%sChoice Pseudo-State %d:\n", prefix, i+1);

			baseLoc = smDesc->smBase->cStates[i].outTransIndex;
			for (j=baseLoc; j<(baseLoc+smDesc->smBase->cStates[i].nOfOutTrans); j++) {
				if (smDesc->smBase->trans[j].dest > 0)
					fprintf(stream, "\tTransition to state %d\n", smDesc->smBase->trans[j].dest);
				else if (smDesc->smBase->trans[j].dest < 0)
					fprintf(stream, "\tTransition to choice pseudo-state %d (this is an illegal transition)\n",
							-smDesc->smBase->trans[j].dest);
				else
					fprintf(stream, "\tTransition to final pseudo-state\n");

				if (smDesc->smBase->trans[j].iTrAction != 0)
					fprintf(stream, "\t\tTransition Action: action n. %d\n", smDesc->smBase->trans[j].iTrAction);
				else
					fprintf(stream, "\t\tNo Transition Action\n");

				if (smDesc->smBase->trans[j].iTrGuard != 0)
					fprintf(stream, "\t\tTransition Guard: guard n. %d\n", smDesc->smBase->trans[j].iTrGuard);
				else
					fprintf(stream, "\t\tNo Transition Guard\n");
			}
		}
	}
}

/* ------------------------------------------------------------------------------- */
void FwSmPrintConfigRec(FwSmDesc_t smDesc, FILE *stream)
{
	FwSmCounterS1_t i;
	FwSmDesc_t embDesc;

	FwSmPrintConfig(smDesc, stream);

	for (i = 1; i <= smDesc->smBase->nOfPStates; ++i) {
		embDesc = FwSmGetEmbSm(smDesc, i);
		if (NULL != embDesc)
			FwSmPrintConfigRec(embDesc, stream);
	}
}

/* ------------------------------------------------------------------------------- */
char* FwSmPrintErrCode(FwSmErrCode_t errCode) {
	switch (errCode) {
		case smSuccess: return "success";
		case smOutOfMemory: return "smOutOfMemory";
		case smNullPState: return "smNullPState";
		case smNullCState: return "smNullCState";
		case smNullTrans: return "smNullTrans";
		case smConfigErr: return "smConfigErr";
		case smIllStateId: return "smIllStateId";
		case smIllChoiceId: return "smIllChoiceId";
		case smStateIdInUse: return "smStateIdInUse";
		case smChoiceIdInUse: return "smChoiceIdInUse";
		case smUndefinedTransSrc: return "smUndefinedTransSrc";
		case smIllegalPDest: return "smIllegalPDest";
		case smIllegalCDest: return "smIllegalCDest";
		case smIllNOfOutTrans: return "smIllNOfOutTrans";
		case smIllTransSrc: return "smIllTransSrc";
		case smTransErr: return "smTransErr";
		case smTooManyTrans: return "smTooManyTrans";
		case smTooManyOutTrans: return "smTooManyOutTrans";
		case smTooManyActions: return "smTooManyActions";
		case smTooManyGuards: return "smTooManyGuards";
		case smTooFewActions: return "smTooFewActions";
		case smTooFewGuards: return "smTooFewGuards";
		case smNegOutTrans: return "smNegOutTrans";
		case smUndefAction: return "smUndefAction";
		case smUndefGuard: return "smUndefGuard";
		case smNotDerivedSM: return "smNotDerivedSM";
		case smEsmDefined: return "smEsmDefined";
		case smWrongNOfActions: return "smWrongNOfActions";
		case smWrongNOfGuards: return "smWrongNOfGuards";
		default: return "invalid error code";
	}
}
