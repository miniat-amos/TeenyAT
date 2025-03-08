# TeenyAT Virtual Architecture 

![Devious looking jellyfish](docs/.leroy.gif)

The **TeenyAT** *( /ˈtē·nē·'āt/ )* is a 16-bit virtual embedded microcontroller delivered as a C library so systems can be simulated around it with ease. System designers create an instance (or more) of the TeenyAT, providing a binary image to load and execute and registering callbacks for any attempted reads or writes to the bus by the running program.

The **TeenyAT** project comes with a custom instruction set and a complete assembler. The *[tnasm](tnasm)* assembler can be compiled via the provided *[Makefile](tnasm/Makefile)*.

## Core Architectural Elements

### Memory
- **Word Size:** 16 bits
- **RAM:** 32K words, addresses `0x0000` - `0x7FFF`
- **Reserved Space:** 1k words, addresses `0x8000` - `0x8FFF`
  - Two bidirectional ports, Port A/B.
  - Positive/Negative Random Number Generators
- **Hardware Access:** addresses, `0x9000` - `0xFFFF`

### Registers
- **PC (Program Counter):** Contains the address of the next instruction; initialized at `0x0000`
- **SP (Stack Pointer):** Contains the address of the top element of the stack; initialized at `0x7FFF`. Stack grows downwards.
- **rZ (Zero Register):** Always contains `0`
- **General-purpose Registers:** rA, rB, rC, rD, rE

### Instruction Encoding
Instructions may be encoded in either one or two 16-bit words:
- **Teeny bit = 1:** Instruction is 16 bits 
- **Teeny bit = 0:** Instruction is 32 bits

### Flags
TeenyAT uses a 4-bit immediate field for flags:
- **Equal:** Set if the last comparison was equal
- **Less:** Set if register1 < register2 in the last comparison
- **Greater:** Set if register1 > register2 in the last comparison
- **Carry:** Reserved for future arithmetic operations

## Assembler Syntax Basics

- **Constants**
  ```
  .const IDENTIFIER VALUE
  ```
- **Variables**
  ```
  .var IDENTIFIER VALUE
  ```
- **Labels**
  ```
  !inf
  jmp !inf
  ```
- **Comments**
  ```
  ; This is a comment
  ```
### Example
```asm
; 
; This is an example from the readme
;
.const thirty_one 31
.const secret_number 4 ; top secret 

.var aero 0xBeeF

!init
     cal !main
     jmp !inf

!main
    set rA, 10
    set rB, thirty_one
    
    add rA, rB
    mod rA, secret_number
    or rA, 0x1

    str [aero], rA
    ret

; ask amos to check his makefile
!inf
    jmp !inf
```

## Instruction Encoding
**First Word**
```
     -------------------------------------------------------------------------------------
    | 15 | 14 | 13 | 12 | 11 |   10  | 9 | 8 | 7 | 6 | 5 | 4 |   3   |   2  |   1  |   0  |
    |-------------------------------------------------------------------------------------|
    |          opcode        | Teeny |   Reg_1   |   Reg_2   | Rsrvd | Eqls | Less | Grtr |
    |-------------------------------------------------------------------------------------|
    |    |    |    |    |    |       |   |   |   |   |   |   |      Immed_4 / Addr_4      |
     -------------------------------------------------------------------------------------
```
**Second Word**
```
     -------------------------------------------------------------------------------------
    | 15 | 14 | 13 | 12 | 11 |   10  | 9 | 8 | 7 | 6 | 5 | 4 |   3   |   2  |   1  |   0  |
    |-------------------------------------------------------------------------------------|
    |                                Immed_16 / Addr_16                                   |
    |-------------------------------------------------------------------------------------|
    |    |    |    |    |    |       |   |   |   |   |   |   |       |      |      |      |
     -------------------------------------------------------------------------------------
```
### See the *[extensive instruction set](./teenyat_instruction_set.md)* for more information.

## Systems Built Around TeenyAT

The **TeenyAT** architecture is a platform suitable for various embedded and educational applications. Writing systems is a key part in developing projects on the TeenyAT. Take a look at the provided **edison** and **[lcd](lcd)** systems.

### Example Implementation in C

 See *[main.c](main.c)* for reference
```c
#include <stdio.h>
#include <stdlib.h>
#include "teenyat.h"

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage:  %s <bin_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *bin_file = fopen(argv[1], "rb");
	teenyat t;
	tny_init_from_file(&t, bin_file, bus_read, bus_write);

	for(int cycle = 0; cycle < 123456; cycle++) {
		tny_clock(&t);
	}

	fclose(bin_file);
	return EXIT_SUCCESS;
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
	data->s = 42; // Example response for all bus reads
	*delay = 9;   // Simulate a 9-cycle delay
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
	// No operation for bus writes
	*delay = 7; // Simulate delay for writes
}
```

---
  
## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
