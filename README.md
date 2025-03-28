# TeenyAT Virtual Architecture 

![Devious looking jellyfish](docs/leroy.gif)

The **TeenyAT** *( /ˈtē·nē·'āt/ )* is a 16-bit virtual embedded microcontroller delivered as a C library so systems can be simulated around it with ease. System designers create an instance (or more) of the TeenyAT, providing a binary image to load and execute and registering callbacks for any attempted reads or writes to the bus by the running program.

The **TeenyAT** project comes with a custom instruction set and a complete assembler. The *[tnasm](tnasm)* assembler can be compiled via the provided *[Makefile](tnasm/Makefile)*.

## Core Architectural Elements

### Memory
- **Word Size:** 16 bits
- **RAM:** 32K words, addresses `0x0000` - `0x7FFF`
- **Reserved Space:** 1k words, addresses `0x8000` - `0x8FFF`
  - Two bidirectional ports, Port A/B.
  - Random Positive/Negative Number Generator
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

## Assembler Syntax Basics

**Variables & Constants** 
  ```asm
  .const IDENTIFIER VALUE
  .var IDENTIFIER VALUE
  ```
**Labels**
  ```asm
  !inf
  jmp !inf
  ```
**Comments**
  ```asm
  ; This is a comment
  ```

## Instruction Encoding

### First Word

### Second Word


### See the *[instruction set](docs/teenyat_instruction_set.md)* for more information about particular instructions.

## Systems Built Around TeenyAT

The **TeenyAT** architecture is a platform suitable for various embedded and educational applications. Writing systems is a key part in developing projects on the TeenyAT. Take a look at the provided **edison** and **[lcd](lcd)** systems.

### Example System in C

```c
#include <stdio.h>
#include <stdlib.h>
#include "teenyat.h"

int main(int argc, char *argv[]) {
	FILE *bin_file = fopen("tbone.bin", "rb");
	teenyat t;
	tny_init_from_file(&t, bin_file, NULL, NULL);

	tny_word port_a;
	for ( int i=0; i <= 76; i++ ) {
		tny_clock(&t);
		tny_get_ports(&t,&port_a, NULL);
		
		if(port_a.bits.bit0 == 0) {
			printf("@"); // LED On
		} else {
			printf("."); // LED Off
		}
	}
	printf("\n");
	return EXIT_SUCCESS;
}
```

### Assembly
```asm
.const PORT_A 0x8002
!main
    str [PORT_A], rA
    inv rA
    jmp !main
```

### Results
```
@@@@@@@.......@@@@@@@.......@@@@@@@.......@@@@@@@.......@@@@@@@.......@@@@@@@
```
---
  
## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
