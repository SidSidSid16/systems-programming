; new fibonacci code with fewer move instr.
	AREA storage,DATA
array
	SPACE 4*20	; Declares a 20-word storage area
array_end

	AREA mainarea,CODE
	EXPORT asm_main
asm_main
	; Your code goes here
	LDR r0, =array
	LDR r1, =array_end
	MOV r2, #1
	MOV r3, #1
	BL fibonacci
fibonacci
	STR r2, [r1], #4
	CMP r0, r1
	BXHS lr
	
	STR r3, [r1], #4
	CMP r0, r1
	BXHS lr
	
	ADD r2, r2, r3
	ADD r3, r3, r2

	B fibonacci			; branch back to the start of the loop
end_fibonacci
	
	B .			; Loop forever

	ALIGN
	END






; Old fibonacci code
;	AREA storage,DATA
;array
;	SPACE 4*20	; Declares a 20-word storage area
;array_end

;	AREA mainarea,CODE
;	EXPORT asm_main
;asm_main
;	; Your code goes here
;	LDR r0, =array
;	LDR r1, =array_end
;	MOV r2, #1
;	MOV r3, #1
;	
;fibonacci
;	CMP r0, r1			; compare start and end address
;	BHS end_fibonacci	; end loop if r0 >= r1
;	
;	STR r2, [r0], #4	; store r0 into r2, increment r0 addr by a word
;	MOV r4, r2			; store copy of r2 in r4
;	ADD r2, r3			; add r2 and r3, store result in r2
;	MOV r3, r4			; move r4 into r3
;	
;	B fibonacci			; branch back to the start of the loop
;end_fibonacci
;	
;	B .			; Loop forever

;	ALIGN
;	END
