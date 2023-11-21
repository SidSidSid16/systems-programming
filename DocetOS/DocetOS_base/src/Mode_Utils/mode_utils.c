#include "Mode_Utils/mode_utils.h"
#include <stdio.h>
#include <inttypes.h> 

/* Mask to extract the last 9 bits */
#define PSR_HANDLER_MASK 0x1FF
/* Mask to extract the last 1 bit */
#define CONTROL_NPRIV_MASK 0x1
/* Mask to extract the last 2 bits */
#define CONTROL_SPSEL_MASK 0x2

void _reportState_delegate(_OS_SVC_StackFrame_t * const stack);

void reportState() {
	uint32_t PSR = getPSR();
	uint32_t CONTROL = getCONTROL();
	
	// create mask to get 9 bits of PSR
	uint32_t mode = PSR & PSR_HANDLER_MASK;
	// create mask to check nPRIV
	uint32_t nPRIV = CONTROL & CONTROL_NPRIV_MASK;
	// create mask to check nPRIV
	uint32_t SPSEL = CONTROL & CONTROL_SPSEL_MASK;

	
	// all zeros in PSR indicates thread mode, anything else indicates handler mode
	char* modeOutput = (!mode) ? "Thread mode" : "Handler mode";
	// Check bit 0 (nPRIV) to find out whether thread mode code is privileged (0 mean yes, 1 means no)
	char* nPRIVOutput = (!nPRIV || mode) ? "priviledged" : "not priviledged";
	// Check bit 1 (SPSEL) to find out whether the MSP (0) or the PSP (1) is the active stack pointer.
	char* SPSELOutput = (!SPSEL) ? "MSP in use" : "PSP in use";
	
	printf("\n\n%s, %s, %s\n\n\r", modeOutput, nPRIVOutput, SPSELOutput);
}

void _reportState_delegate(_OS_SVC_StackFrame_t * const stack) {
	reportState();
	// print the value of the stacked r0
	printf("\n\n Stacked r0: %" PRId32 "\n\n\r", stack->r0);
	// increment the stacked r0
	stack->r0++;
}
