	AREA mainarea,CODE
	EXPORT fib
		
fib
	PUSH {r1, LR}
	; check for the base case if r0 is 1 or 2
	CMP r0, #1
	BEQ base_case
	CMP r0, #2
	BEQ base_case
	
	; recursive fib(n-1)
	PUSH {r0}			; save r0 so that when n-1 completes, we can do n-2.
	SUB r0, r0, #1		; calculate the n-1 index.
	BL fib				; branch back to fib, recursively calc fib(n-1).
						; we want to return to this line after we recursively
						; calculated fib(n-1).
	MOV r1, r0			; save output of fib(n-1) in r1.
	
	; recursive fib(n-2)
	POP {r0}			; restore r0 to calculate fib(n-2) for original index.
	SUB r0, r0, #2		; calculate the n-2 index.
	BL fib				; branch back to fib, recursively calc fib(n-1) for n-2 value.
						; we want to return to this line after we recurively
						; calculated fib(n-1) for n = requested index - 2.
						; at this point, we'd have calculated both fib(n-1) and fib(n-2).
	
	ADD r0, r0, r1		; add the two fib calcs for n-1 and n-2 - this is the result.
	
	POP {r1, LR}
	BX LR
	
	
base_case
	MOV r0, #1
	POP {r1, LR}
	BX LR
		
	END