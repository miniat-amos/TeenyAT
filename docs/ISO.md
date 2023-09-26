# TeenyAT Instruction Set Overview


### Registers
##### All 8 registers are available to register-use instructions
- PC - is the program counter, contains address of _next_ instruction, initially 0x0000
- SP - is the stack pointer, contains address of the _next_ top element, initially 0x7FFF (empty)
	- _stack grows down in memory_
- rZ - is the "Zero" register, which always contains the value 0
	- _all attempts to modify rZ are ignored_
- rA through rE are general purpose registers

### Flags

##### All flags are set by both ALU & CMP instructions
- C- carry flag
- E - equals or zero flag
- L - less than flag
- G - greater than flag

### Instructions by Type

#####  Logical Instructions
- AND - bitwise ANDs a register with a value
- OR - bitwise ORs a register with a value
- XOR - bitwise XORs a register with a value
- INV - flips all bits in a register
- SHF - shift bits (left or right based on sign)
- ROT - rotates bits (left or right based on sign)

#####  Arithmetic Instructions
- ADD - adds a value to a register
- SUB - subtracts a value to a register
- MPY - multiplies a value to a register
- DIV - divides a value to a register
- MOD - divides and then stores remainder
- NEG - negates a register
- INC - increments a register by 1 (+1)
- DEC - decrements a register by 1 (-1)

#####  Data Instructions
- SET - sets a register to a value
- LOD - loads a value from memory into a register
- STR - Stores a value into a given memory address
- PSH - loades a register into SP and decrements SP by 1
- POP - loads value from top of SP into register

#####  Bit Instructions
- BTS - sets a specific bit in a register to 1
- BTC - clears a specific bit in a register (sets to 0)
- BTF - flips a specific bit in a register

#####  Control Instructions
- CAL - sets the PC to an address and decrements SP  
- RET - returns out of a CAL instruction and increments SP
- CMP - compares two values to eachother and sets flags (C,E,L,G)
- JMP - jumps to given address
- JE - jumps to given address if Equals flag true
- JNE - jumps to given address if equals flag false
- JL - jumps to given address if less than flag true
- JLE - jumps to given address if less than or equal flag are true
- JG - jumps to given address if greater than flag true
- JGE - jumps to given address if greater than or equals flag true
- DJZ - decrements a register then jumps to  address if register is zero (zero flag true)