	AREA storage,DATA
array
	SPACE 4*20	; Declares a 20-word storage area
array_end

	AREA mainarea,CODE
	EXPORT asm_main
asm_main
	; Your code goes here

	B .			; Loop forever

	ALIGN
	END
