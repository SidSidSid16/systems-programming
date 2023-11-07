	AREA mainarea,CODE
	EXPORT process

process
	STMFD sp!, {r4-r8, lr}

	LDMIA r0, {r4-r7}			; load struct fields into scratch regs
	ADD r4, r5, r4				; r4 (y) = x + y
	STR r4, [r0, #0]			; store r4 into struct y field
	
	MOV r0, r7					; move ptr field (r7) into r0 before callback BLX
	BLX r6						; branch to callback function

	LDMFD sp!, {r4-r8, pc}
	END
