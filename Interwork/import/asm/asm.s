	AREA mainarea,CODE
	EXPORT fib
	IMPORT report
fib
	STMFD sp!, {r4-r8, lr}
	
	MOV r4, r0			
	MOV r5, #1			; first value
	MOV r6, #1			; second value
	
fib_loop
	CMP r4, #0			; check if counter is at the end
	BLS fib_loop_end	; end loop if counter is at the end
	
	SUB r4, r4, #1		; decrement counter
	
	MOV r0, r5			; cache the fib number to r0
	BL report			; print the fib number
	
	ADD r7, r5, r6		; add the two fib numbers store in r8
	MOV r5, r6			; store the second number into r5
	MOV r6, r7			; store the bigger number into r6
	
	B fib_loop			; branch back to the start of the loop
fib_loop_end
	
	LDMFD sp!, {r4-r8, pc}
	
	END