STACKSIZE   EQU 0x100
SLOPE       EQU 5
OFFSET      EQU 10
ARRAYSIZE   EQU 4



; Stack area initialization in RAM memory
            AREA STACK, NOINIT, READWRITE, ALIGN = 3
StackMem    SPACE STACKSIZE

; Vector table initialization in ROM memory
            AREA RESET, DATA, READONLY
            EXPORT Vectors

Vectors
            DCD StackMem + STACKSIZE ; stack pointer for empty stack
            DCD Reset_Handler        ; reset vector
            ALIGN

; An array of variables in RAM
            AREA MyDSTdata, DATA, READWRITE
dst_data    DCW 0, 0, 0, 0

; An array of constants in ROM

            AREA MySRCdata, DATA, READWRITE
src_data    DCW 1, 2, 3, 4
; User program
            AREA | .text|, CODE, READONLY, ALIGN = 2
            ENTRY
            EXPORT Reset_Handler
Reset_Handler
            MOV r2, #ARRAYSIZE  ; R2 = 4
            MOV r3, #OFFSET     ; R3 = 10
            MOV r4, #SLOPE      ; R5 = 5
            LDR r5, =src_data   ; new r5[src_data]
            LDR r6, =dst_data   ; new r6[dst_data]
Loop        LDRH r0, [r5], #2   ; R0 <- 1 and R5[i + 1]; R0 <- 2 & R5[i + 1]
            MLA r0, r0, r4, r3  ; R0 <- 1*5+10=15 R0 <- 2*5+10=20; 
            STRH r0, [r6], #1   ; {15, 20, 25, 30} 
            SUBS r2, r2, #1     ; R2 -> 3; R2 -> 2
            BNE Loop
Loop_Forever
            B Loop_Forever
            END