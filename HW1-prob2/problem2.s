Stack1 	EQU 0x400
		AREA STACK, NOINIT, READWRITE, ALIGN =3	
MyStackMem SPACE Stack1					
		AREA RESET , READONLY			; I couldn't figure out how to get the main
		DCD MyStackMem + Stack1 		; stack pointer for empty stack	
		AREA MyData, DATA, READWRITE
dst 	SPACE 0x10
		AREA MYCODE, CODE, READONLY
src 	DCB "Welcome to Microprocessor Systems Course.", 0
		ALIGN
		ENTRY
		EXPORT __main
__main
		MOV R4, #0x0
		LDR R0, =src
		LDR R2 , =dst
		LDRB R1, [R0]
loop
		CMP R1, #0x20 					; First enumerate the number of (check for space)
		BEQ loop1						; elements in the source array
		CMP R1, #0x2E 					; check for period
		BEQ loop1
		CMP R1, #0x43
		BEQ eof
		ADD R4, #1						; store size of string
		MOV R6, R4
		LDRB R1, [R0, #1]!				; load R1 into R0 preincrement
		B loop	
loop1
		LDRB R1, [R0 , #-1]! 			; Start with the last element of
		STRB R1, [R2] 					; source array and store element
		ADD R2, #1 						; by element
		SUBS R4, #1						; for(i = size; i >= 0; i--)
		BNE loop1	
		CMP R1, #0x2E
		IT NE
		MOVNE R5, #0x20
		STRBNE R5, [R2], #1
		ADDNE R0, R6, R0
		ADDNE R0, #1
		MOVNE R4, #0x0
		MOVNE R6, #0x0
		BNE loop
eof		
		MOV R9, #0x2E
		STRB R9, [R2, #-1]
		B eof1
eof1	B eof1		
		ALIGN
		END