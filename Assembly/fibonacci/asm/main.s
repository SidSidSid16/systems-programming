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
	
fibonacci
	CMP r0, r1			; compare start and end address
	BHS end_fibonacci	; end loop if r0 >= r1
	
	STR r2, [r0] #4		; store memory item into r2, increment addr by 4 bytes
	MOV r4, r2
	ADD r2, r3
	MOV r3, r4
	
end_fibonacci
	
	B .			; Loop forever

	ALIGN
	END
