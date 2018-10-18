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
loop									; For the first loop I use it as the train station where I direct traffic
		CMP R1, #0x20 					; of my algorithm. I do branch checks for the special characters I care about
		BEQ loop1						; and have branches for each. In this section I also enumerate over each word 
		CMP R1, #0x2E 					; to find their legnth and use it as a size.
		BEQ loop1
		CMP R1, #0x43
		BEQ eof
		ADD R4, #1						; I use two registers R4 and R6 for the size because I use them both for storing to dst
		MOV R6, R4						; and my index jump.
		LDRB R1, [R0, #1]!				; load R1 into R0 preincrement
		B loop	
loop1
		LDRB R1, [R0 , #-1]! 			; Start with the last element of
		STRB R1, [R2] 					; source array and store element
		ADD R2, #1 						; by element
		SUBS R4, #1						; for(i = size; i >= 0; i--)
		BNE loop1						; I move a space to the end of the string
		CMP R1, #0x2E					; and take care of the period case in this loop.
		IT NE							; I decided for ease of use to change the last element to a space
		MOVNE R5, #0x20					; then to a period so that I could see the change as in KEIL periods are
		STRBNE R5, [R2], #1				; the empty case for a segment of memory.
		ADDNE R0, R6, R0
		ADDNE R0, #1
		MOVNE R4, #0x0
		MOVNE R6, #0x0
		BNE loop
eof		
		MOV R9, #0x2E					; takes care of the period case in eof
		STRB R9, [R2, #-1]
		B eof1
eof1	B eof1		
		ALIGN
		END