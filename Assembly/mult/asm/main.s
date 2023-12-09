	AREA mainarea,CODE
	EXPORT asm_main
asm_main

	; Your instructions go here...
	MOV r0, #2
	ADD r1, r0, r0, LSL #3		; r1 = r0 + 8*r0 = 9*r0
	ADD r1, r1, r0, LSL #2		; r1 = r1 + 4*r0 = 9*r0 + 4*r0 = 13*r0
	
	B .			; Loop forever

	END
