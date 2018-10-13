Stack1		EQU 0x400
			AREA STACK, NOINIT, READWRITE, ALIGN =3
MyStackMem	SPACE Stack1
			AREA RESET, READONLY
			DCD MyStackMem + Stack1
			AREA MYCODE, CODE, READONLY
src			DCB 0xA, 0x5, 0xF, 0xB, 0x3
			AREA MyData, DATA, READWRITE
dst			DCB 0x0, 0x0, 0x0, 0x0, 0x0
			ALIGN
			ENTRY
			EXPORT __main
__main
			LDR R0, =src
			LDR R2, =dst
			LDR R1, [R0]
End_Prog
			B End_Prog
			END