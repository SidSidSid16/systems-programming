	AREA mainarea,CODE
	EXPORT asm_main
	IMPORT fib

asm_main

	; Your call to 'fib' goes here
	MOV r0, #7
	BL fib
	B .			; Loop forever

	ALIGN
	END
