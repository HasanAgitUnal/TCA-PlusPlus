; Multiplies two numbers using repeated addition.
; R4 = Multiplicand, R5 = Multiplier
; The final result is stored in R1.
; You dont need to understand this code just undertand syntax

uconst MULTIPLICAND 5
uconst MULTIPLIER 3

; --- Setup ---
MVI MULTIPLICAND
MOV R4 R0        ; R4 = Multiplicand

MVI MULTIPLIER
MOV R5 R0        ; R5 = Multiplier (counter)

MVI 0
MOV R1 R0        ; R1 = 0 (result accumulator)

.loop_start
; --- Main Loop ---

; Check if counter (R5) is zero.
; To do this, we calculate R5-0 and check if R3 is zero.
; Note: This section has a logic bug due to register conflicts,
; but it is syntactically correct for the assembler test.
MOV R1 R5
MVI 0
MOV R2 R0
OPPR SUB        ; R3 = R5 - 0

MVI .halt
JMPIF =        ; If R3 is 0 (counter is done), jump to halt.

; Add multiplicand to result. (R1 = R1 + R4)
MOV R2 R4      ; OPPR needs multiplicand in R2.
OPPR ADD        ; R3 = R1 + R2 (result + multiplicand)
MOV R1 R3      ; Store new result in R1.

; Decrement counter. (R5 = R5 - 1)
MOV R1 R5      ; OPPR needs counter in R1.
MVI 1
MOV R2 R0      ; R2 = 1
OPPR SUB        ; R3 = R1 - R2 (counter - 1)
MOV R5 R3      ; Store new counter value in R5.

; Jump back to the start of the loop.
MVI .loop_start
JMP

.halt

; --- Halt ---
; Infinite loop to halt execution. Result is in R1.
MVI .halt
JMP
