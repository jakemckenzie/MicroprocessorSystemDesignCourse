		AREA A_SIMPLE_PROGRAM, CODE, READONLY
		ENTRY
		EXPORT __main		; Jake McKenzie Lab 1 Problem 8
__main
		MOV R7, #0x0		; Clear contents of R7
		MOV R6, #0x0 		; Clear contents of R6
		MOV R5, #0x1
		MOV R6, #-149		; Load -149 into R6
		MOV R7, #-4321		; Load -4321 into R7
		ADD R7, R6, R7		; -149 + (-4321) = -4470
HERE	B	HERE			; 
		END