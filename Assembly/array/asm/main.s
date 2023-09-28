	AREA storage,DATA
array
	; Total is 232 for the example list
	DCD 3, 7, 31, 193, 2, -5, 1
array_end

	AREA main,CODE
	EXPORT asm_main
asm_main
	; Your code goes here
	MOV r0, -5
	BL abs
	B .			; Loop forever

abs
	CMP r0, #0
	RSBLE r0, r0, #0
	BX lr
	
	ALIGN
	END
