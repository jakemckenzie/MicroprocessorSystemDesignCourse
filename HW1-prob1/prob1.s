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
            MOV r2, #ARRAYSIZE
            MOV r3, #OFFSET
            MOV r4, #SLOPE
            LDR r5, =src_data
            LDR r6, =dst_data
Loop        LDRH r0, [r5], #2
            MLA r0, r0, r4, r3
            STRH r2, [r6], #1
            SUBS r2, r2, #1
            BNE Loop
Loop_Forever
            B Loop_Forever
            END