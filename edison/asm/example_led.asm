.const PORT_A_DIR 0x8000
.const PORT_B_DIR  0x8001
.const PORT_A 0x8002
.const PORT_B 0x8003

SET rA, 0xFFFF ; set port directions to output 
STR [PORT_A_DIR], rA


STR [PORT_B_DIR], rZ

!loop
SET rA, 0xFFFF
STR [PORT_B], rA
DLY 1000
SET rA, 0x0FFF
STR [PORT_B], rA
DLY 1000
JMP !loop


!main
JMP !main
