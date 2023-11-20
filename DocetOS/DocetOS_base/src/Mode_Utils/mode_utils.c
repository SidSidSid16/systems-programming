#include "Mode_Utils/mode_utils.h"
#include <stdio.h>
#include <inttypes.h>

#define PSR_HANDLER_MASK 0x1FF
#define CONTROL_NPRIV_MASK 0x1
#define CONTROL_SPSEL_MASK 0x2

void reportState() {
	// all zeros in PSR indicates thread mode,
	// anything else indicates handler mode
	uint32_t PSR = getPSR();
	// create mask to extract last 9 bits
	uint32_t mode = PSR & PSR_HANDLER_MASK;
	if (mode == 0) {
		printf("\r\n\nThread mode, ");
	} else {
		printf("\r\n\nHandler mode, ");
	}
	
	// Check bit 0 (nPRIV) to find out whether
	// thread mode code is privileged (0 mean
	// yes, 1 means no).
	uint32_t CONTROL = getCONTROL();
	// create mask to check nPRIV
	uint32_t nPRIV = CONTROL & CONTROL_NPRIV_MASK;
	if (nPRIV == 0) {
		printf("priviledged, ");
	} else {
		printf("not priviledged, ");
	}
	
	// Check bit 1 (SPSEL) to find out whether
	// the MSP (0) or the PSP (1) is the active
	// stack pointer.
	// create mask to check nPRIV
	uint32_t SPSEL = CONTROL & CONTROL_SPSEL_MASK;
	if (SPSEL == 0) {
		printf("MSP in use\n");
	} else {
		printf("PSP in use\n");
	}
}
