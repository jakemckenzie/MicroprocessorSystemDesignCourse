		AREA prog, CODE, READONLY
		ALIGN	
		EXPORT __main		; Jake McKenzie numP
		ENTRY
__main
		LDR R0, =src		; initialize src[]
		LDRB R2, [R0]		; post increment load register
		MOV R1, #0x0		; Making sure R1 is 0 at the start, mostly for debugging purposes
		MOV R3, #0x10		; R3 <- "newline character" for counting
loop	
		CMP R2, #0			; while R2 != 0 do loop
		BEQ endloop
		CMP R3, R2			; checks to see if the current character
		IT EQ				; is a newline character.
		ADDEQ R1, #1		; count ++ where count is held
		LDRB R2, [R0], #1	; loads next char to check
		B loop
endloop
		
STOP	B STOP
		ALIGN
		AREA A, DATA, READONLY
src		DCB "This is a paragraph.", 0x10 ,0 ; for some reason this stupid 
		ALIGN								; stupid assembler can't parse \n
		END									; so I had to do this.