	AREA utils, CODE, READONLY
	
	; Export functions
	EXPORT getPSR
	EXPORT getCONTROL

getPSR
	MRS r0, xPSR
	BX lr
	
getCONTROL
	MRS r0, CONTROL
	BX lr
	
	; ALIGN
	END