		AREA A_SIMPLE_PROGRAM, CODE, READONLY
		ENTRY
		EXPORT __main						  	; Jake Mckenzie Lab 1 problem 5.8.5
TABLE 	DCD 0xFEBBAAAA, 0x12340000, 0x88881111	; Loading values into the table
		DCD 0x00000013, 0x80808080, 0xFFFF0000	;
__main
		LDR R1, =TABLE						 	; Load array
		
		LDR R2, [R1, #4]						; R2 <- 0xFEBBAAAA
		MOV R3, R2								; R3 <- R2
		
		LDR R2, [R1, #4]						; R2 <- 0x12340000 
		ADD R3, R3, R2							; R3 <- R2

		LDR R2, [R1, #4]						; R2 <- 0x88881111
		ADD R3, R3, R2							; R3 <- R2
		
		LDR R2, [R1, #4]						; R2 <- 0x00000013
		ADD R3, R3, R2							; R3 <- R2
		
		LDR R2, [R1, #4]						; R2 <- 0x80808080
		ADD R3, R3, R2							; R3 <- R2
		
		LDR R2, [R1, #4]						; R2 <- 0xFFFF0000
		ADD R3, R3, R2							; R3 <- R2
HERE	B	HERE		
		END