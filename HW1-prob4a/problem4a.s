		AREA prog, CODE, READONLY
		ALIGN	
		EXPORT __main		;Jake McKenzie
		ENTRY
__main
		LDR R0, =src		;initialize src[]
		LDRB R2, [R0]		;post increment load register
		MOV R1, #0x0
		MOV R3, #0x10
loop	
		CMP R2, #0
		BEQ endloop
		CMP R3, R2
		IT EQ
		ADDEQ R1, #1
		LDRB R2, [R0], #1
		B loop
endloop
		
STOP	B STOP
		ALIGN
		AREA A, DATA, READONLY
src		DCB "This is a paragraph.", 0x10 ,0
		ALIGN
		END