		AREA A_SIMPLE_PROGRAM, CODE, READONLY
		ENTRY
		EXPORT __main		; Jake McKenzie Lab 1 Problem 6
__main
		MOV R1, #0x0		; Clear contents of R1
		MOV R2, #0x0 		; Clear contents of R2
		MOV R3, #0x0		; Clear contents of R3
		MOV R3, #0x2		; x <- 2
		MOV R1, #0x6		; Loading constant to multiply
		MUL R2, R3, R1		; 3 * x
		MOV R1, #0x9		; Loading constant to multiply
		SUB R2, R2, R1		; 3 * x - 9
		MUL R2, R3, R2		; x * (3 * x - 9)
		MOV R1, #0x2		; Loading constant to multiply
		ADD R2, R2, R1		; x * (3 * x - 9) + 2
HERE	B	HERE			; Should expect R2 <- 8 at the end. Which we do.
		END