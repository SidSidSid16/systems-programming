	AREA mainarea,CODE
	EXPORT asm_main

asm_main
	MOV	r0, #0
	MOV r1, #0
loop
	ADD	r0, #1
	ADD	r1, r0
	CMP	r0, #10
	BLT loop

	B .			; Loop forever

	END
