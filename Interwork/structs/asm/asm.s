	AREA mainarea,CODE
	EXPORT process

process
	STMFD sp!, {r4-r8, lr}

	LDMIA r0, {r4-r7}			; load struct fields into scratch regs
	ADD r5, r4, r5				; r5 = x + y
	STR r5, [r0, #4]			; store r5 into struct y field
	
	MOV r0, r6					; move ptr field (r6) into r0 before callback BLX
	BLX r7						; branch to callback function

	LDMFD sp!, {r4-r8, pc}
	END
