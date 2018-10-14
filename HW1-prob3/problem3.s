		AREA prog, CODE, READONLY
		EXPORT __main
		ENTRY
__main						;Jake McKenzie	
		LDR R0, =src		;initialize src[]
		LDR R2, =dst		;initialize dst[]
		LDRB R1, [R0], #1	;post increment load register
		
		MOV R3, #0xA		;move 0xA into R3
		MOV R4, #0x41		;move 0x41 into R3
		CMP R3, R1			;R3 == R1
		IT EQ				;if(R3 == R1)
		STRBEQ R4, [R2], #1	;
		LDRB R1, [R0], #1	;load R0 into location of R1(post increment)
		
		MOV R3, #0x5		;move 0x5 into R3
		MOV R4, #0x35		;move 0x35 into R3
		CMP R3, R1			;R3 == R1
		IT EQ				;if(R3 == R1)
		STRBEQ R4, [R2], #1	;Store R4 into R2(post increment)
		LDRB R1, [R0], #1	;load R0 into location of R1(post increment)
		
		MOV R3, #0xF		;move 0xF into R3
		MOV R4, #0x46		;move 0x46 into R3
		CMP R3, R1			;R3 == R1
		IT EQ				;if(R3 == R1)
		STRBEQ R4, [R2], #1	;Store R4 into R2(post increment)
		LDRB R1, [R0], #1	;load R0 into location of R1(post increment)
		
		MOV R3, #0xB		;move 0xB into R3
		MOV R4, #0x42		;move 0x42 into R3
		CMP R3, R1			;R3 == R1
		IT EQ				;if(R3 == R1)
		STRBEQ R4, [R2], #1	;Store R4 into R2(post increment)
		LDRB R1, [R0], #1	;load R0 into location of R1(post increment)
		
		MOV R3, #0x3		;move 0x3 into R3
		MOV R4, #0x33		;move 0x33 into R3
		CMP R3, R1			;R3 == R1
		IT EQ				;if(R3 == R1)
		STRBEQ R4, [R2], #1	;Store R4 into R2(post increment)
		LDRB R1, [R0]		;load R0 into location of R1
		
STOP	B STOP
		ALIGN
		AREA A,DATA,READONLY
src 	DCB 0xA, 0x5, 0xF, 0xB, 0x3
		ALIGN
		AREA Z, DATA, READWRITE
dst 	DCB 0x0, 0x0, 0x0, 0x0, 0x0
		ALIGN
		END