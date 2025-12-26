; Loop addition code example

; load R1 register
MVI 5 ; MVI command uses R0 as save location
MOV R0 R1

.loop

; load R2 register
MVI 3
MOV R0 R2

; add R1 and R2 register . OPPR command uses R1 and R2 as input and R3 as output
OPPR ADD

; Load R1 with result value
MOV R3 R1

; Jump to loop
MVI .loop ; skip R1 loading for not resetting
JMP ; jmp to address at R0

; this will be run endless