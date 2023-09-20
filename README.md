This project is an early work in progress not ready for much, but I wanted to make it available for my student research team via GitHub as we work to finalize design and work toward an initial release.

# TeenyAT

The TeenyAT ( /ˈtē·nē·'at/ ) is a 16-bit virtual embedded micro-controller delivered as a C library so systems can be simulated around it with ease.  System designers create an instance (or more) of the TeenyAT, providing a binary image to load and execute and registering callbacks for any attempted reads or writes to the bus by the running program.

The following is not necessarily accurate, but is nonetheless constructed from different bits of documentation written long ago as the initial concepts of the architecture were being fleshed out.  Some details are contradictory or plain wrong, but we'll hopefully get these corrected over time.

## Core Architectural Elements

-   16 bit words (each memory address points to a 16 bit value)
-   32K words of RAM from 0x0000 through 0x7FFF
	-   Address 0x8000-0xFFFF: available for hardware access
-   8 registers available to all register-use instructions
	-   PC (reg[0]) is the program counter, contains address of next instruction, initially 0x0000
	-   SP (reg[1]) is the stack pointer, contains address of the next top element, initially 0x7FFF (empty)
		-   stack grows down in memory
	-   rZ (reg[2]) is the "Zero" register, which always contains the value 0.
		-   all attempts to modify rZ are ignored
	-   rA through rE (reg[3] through reg[7]) are general purpose registers
-   Instructions encoded in either one or two 16 bit words
	-   Fetch must set PC = PC + 1 - T, where T is the "teeny" bit to account for this
		-   if teeny == 1, the instruction is 16 bits (essentially assumes 2nd word is 0x0000)
		-   if teeny == 0, the instruction is 32 bits and requires an additional cycle
