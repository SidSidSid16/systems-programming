	AREA mainarea,CODE
	EXPORT calculate

calculate
	STMFD sp!, {r4, lr}
	
	ADD r4, r0, r0, LSL #2		; r4 = r0 + 4*r0 = 5*r0
	ADD r0, r4, r1				; r0 = r4 + r1 = 5*r0 + y
	
	LDMFD sp!, {r4, pc}

	END
