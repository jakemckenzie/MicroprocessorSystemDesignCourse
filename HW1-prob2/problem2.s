Stack1 	EQU 0x400
		AREA STACK, NOINIT, READWRITE, ALIGN =3	
MyStackMem SPACE Stack1
		AREA RESET , READONLY
		DCD MyStackMem + Stack1 		; stack pointer for empty stack	
		AREA MyData, DATA, READWRITE
dst 	SPACE 0x10
		AREA MYCODE, CODE, READONLY
src 	DCB "Welcome to Microprocessor Systems Course.", 0
		ALIGN
		ENTRY
		EXPORT __main
__main
		LDR R0 , =src
		LDR R2 , =dst
		LDRB R1 , [R0]
loop
		CMP R1, #0 					; First enumerate the number of
		BEQ copy					; elements in the source array
		ADD R4, #1
		LDRB R1, [R0, #1]!
		B loop	
copy
		CMP R4, #0
		BEQ End_Prog
copy1
		LDRB R1, [R0 , #-1]! 		; Start with the last element of
		STRB R1, [R2] 				; source array and store element
		ADD R2, #1 				; by element
		SUBS R4, #1
		BNE copy1
End_Prog
		B End_Prog
		END