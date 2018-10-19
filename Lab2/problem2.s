		AREA prog, CODE, READONLY
		EXPORT __main	; Jake McKenzie Lab 2
		ENTRY
__main
		MOV R0, #1  	; R0 = i
		MOV R1, #0		; R1 = j
		MOV R2, #-1		; R2 = k
		MOV R3, #-5	; R3 = a
		MOV R4, #5	; R4 = b
while	
		CMP R0, R1
		IT LE
		BLE eof
		MOV R9, #2
		MUL R5, R1, R9
		SUB R6, R3, R5
		ADD R1, R1, R6
		CMP R1, R2
		IT GE
		ADDGE R0, R0, R9
		MULGE R7, R1, R9
		ADDGE R8, R4, R7
		SUBGE R2, R2, R8
		ADD R1, R1, #1
		CMP R0, R1
		;IT GT
		B while
eof	B eof
		ALIGN
		END