
# TeenyAT Robot Architecture
**/ˈtē∙nē∙ət/**  
**An Architecture to Virtualize**

---

#### Reference Manual


## Contents
### [1. Overview](#ove)
### [2. Core Architectural Elements](#cae)
### [3. Instruction Encoding](#ire)
### [4. Instruction Set](#irs)
### [5. Reference Table](#ref)
### [6. Syntax Help](#syn)
### [7. Sample Programs](#sam)

<a name="ove">

## 1. Overview

The TeenyAT ( /ˈtē·nē·'at/ ) is a 16-bit virtual embedded micro-controller delivered as a C library so systems can be simulated around it with ease.  System designers create an instance (or more) of the TeenyAT, providing a binary image to load and execute and registering callbacks for any attempted reads or writes to the bus by the running program.


</a>

<a name="cae">

## 2. Core Architectural Elements
The TeenyAT core architecture is made up of 16 bit words. Each memory address points to a 16 bit value. If measuring storage in terms of words, there is 32K words of RAM. The hexadecimal addresses range from 0x0000 to 0x7FFF and addresses 0x8000 through 0xFFFF are available for hardware access.
	 
There are 8 registers available for all register-use instructions: PC or reg[1] is the program counter. It contains the address of the next instruction. The PC is initialized to address 0x0000. The next register is the SP or reg[2] is the stack pointer. The SP contains the address of the next top element in the stack and is initialized to address 0x7FFF. This initial address means the SP is empty. It grows down in memory. The "zero" register or rZ is reg[0]. This register always contains the value 0. All attempts to modify the "zero" register are ignored. The rest of the available registers: rA through rE, reg[3] through reg[7], are general purpose registers.  

Each instruction is encoded in either one or two 16 bit words. The fetch instruction must set *PC = PC + 2 - T*, where *T* is the "teeny" bit to account for this. If the teeny bit is equal to one, "teeny == 1," then the instruction is 16 bits. This essentially assumes that the second word is 0x0000. If the teeny bit is equal to zero, "teeny == 0," then the instruction is 32 bits and requires an additional cycle. We can also say that the second instruction is not teeny. Any fetches to RAM referencing addresses greater than 0x7FFF will result in the PC being reset to 0x0000.

A four bit immediate field can be used as flags. In order of highest to lowest order bits, the flags are: carry, equal to, less than, and greater than. The highest bit is currently reserved as the carry bit in some places but it is not fully supported at this moment. The equal flag is set when the last comparison, by virtue of a previous instruction, is found to the equal. The less than flag is set the last comparison resulted in the first register being less than the second register. Likewise, the greater than flag is set when the result of the last comparison resulted in the first register being greater than the second register.

### In a condensed format, the architecture contains:
-   16 bit words (each memory address points to a 16 bit value)
-   32K words of RAM from 0x0000 through 0x7FFF
     -   Address 0x8000-0xFFFF: available for hardware access
-   8 registers available to all register-use instructions
	-   PC (reg[1]) is the program counter, contains address of next instruction, initially 0x0000    
	-   SP (reg[2]) is the stack pointer, contains address of the next top element, initially 0x7FFF (empty)
		-   stack grows down in memory
	-   rZ (reg[0]) is the "Zero" register, which always contains the value 0.
		-   all attempts to modify rZ are ignored
	-   rA through rE (reg[3] through reg[7]) are general purpose registers
-   Instructions encoded in either one or two 16 bit words
	-   Fetch must set PC = PC + 2 - T, where T is the "teeny" bit to account for this
		-   if teeny == 1, the instruction is 16 bits (essentially assumes 2nd word is 0x0000)
		-   if teeny == 0, the instruction is 32 bits and  requires an additional cycle
     -   Fetches to RAM greater than 0x7FFF will result in the PC being set to 0
-   Using 4 bit immedate field as flags
    -   Flags in order from highest to lowest order bits
        -   Reserved currently implemented as carry in some places but not fully supported at the moment
        -   Equals set when last compare is found to be Equal
        -   Less set when the last compare resulted in reg1 being less than reg2
        -   Greater set when the last compare resulted in reg1 being greater than reg2
<a/>

<a name="ire">
## 3. Instruction Encodings

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
<a/>

<a name="irs">

## 4. Instruction Set
***Note:** Notice that all instructions are either three characters or less to maintain a sense of order when looking at the assembly code.*
### 4.1 SET
- Opcode: 0
- Desc: Sets a register to a value
- Flags: N/A
- Usage: 
    - SET register1, register2 + immed
    - SET register1, register2
    - SET register1, immed
### 4.2 LOD
- Opcode: 1
- Desc: Loads a value from an address into register1
     -    If value is greater then 0x7FFF will read from bus
- Flags: N/A
- Usage: 
    - LOD register1, [register2 + immed]
    - LOD register1, [register2]
    - LOD register1, [immed]
### 4.3 STR
- Opcode: 2
- Desc: Stores a value from an address into a register2
     -    If value is greater then 0x7FFF will write to bus
- Flags: N/A
- Usage: 
    - STR [register1 + immed], register2
    - STR [register1], register2
    - STR [immed], register2
### 4.4 PSH
- Opcode: 3
- Desc: Sets SP to a value then Decrements the SP
- Flags: N/A
- Usage: 
    - PSH register2 + immed
    - PSH register2 
    - PSH immed
### 4.5 POP
- Opcode: 4
- Desc: Increments SP and puts the value into register1
- Flags: N/A
- Usage: 
    - POP register1 
### 4.6 BTS
- Opcode: 5
- Desc: Sets a bit in register1 to 1 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to set an invalid bit acts as a nop.
- Usage: 
    - BTS register1, register2 + immed
    - BTS register1, register2
    - BTS register1, immed
### 4.7 BTC
- Opcode: 6
- Desc: Sets a bit in register1 to 0 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to clear an invalid bit acts as a nop.
- Usage: 
    - BTC register1, register2 + immed
    - BTC register1, register2
    - BTC register1, immed
### 4.8 BTF
- Opcode: 7
- Desc: Toggles a bit in register1 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to flip an invalid bit acts as a nop.
- Usage: 
    - BTF register1, register2 + immed
    - BTF register1, register2
    - BTF register1, immed
### 4.9 CAL
- Opcode: 8
- Desc: Sets SP to PC then Decrements SP and sets PC to an address
     - mainly used to set the pc to a label and then return from it later
- Flags: N/A
- Usage: 
    - CAL register2 + immed
    - CAL register2 
    - CAL immed 
### 4.10 ADD
- Opcode: 9
- Desc: Adds a value to register1
- Flags: Eqls,Less,Grtr
- Usage: 
    - ADD register1, register2 + immed
    - ADD register1, register2
    - ADD register1, immed
### 4.11 SUB
- Opcode: 10
- Desc: Subtracts a value from register1
- Flags: Eqls,Less,Grtr
- Usage: 
    - SUB register1, register2 + immed
    - SUB register1, register2
    - SUB register1, immed
### 4.12 MPY
- Opcode: 11
- Desc: Multiplies register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - MPY register1, register2 + immed
    - MPY register1, register2
    - MPY register1, immed
### 4.13 DIV
- Opcode: 12
- Desc: Divides register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - DIV register1, register2 + immed
    - DIV register1, register2
    - DIV register1, immed
### 4.14 MOD
- Opcode: 13
- Desc: Modulo register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - MOD register1, register2 + immed
    - MOD register1, register2
    - MOD register1, immed
### 4.15 AND
- Opcode: 14
- Desc: Bitwise AND register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - AND register1, register2 + immed
    - AND register1, register2
    - AND register1, immed
### 4.16 OR
- Opcode: 15
- Desc: Bitwise OR register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - OR register1, register2 + immed
    - OR register1, register2
    - OR register1, immed
### 4.17 XOR
- Opcode: 16
- Desc: Bitwise EXCLUSIVE OR register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - XOR register1, register2 + immed
    - XOR register1, register2
    - XOR register1, immed
### 4.18 SHF
- Opcode: 17
- Desc: Bitwise SHIFT register1 by a value
     - If value is positive it will shift right (>>)
     - If value is negative it will shift left  (<<)
- Flags: Eqls,Less,Grtr
- Usage: 
    - SHF register1, register2 + immed
    - SHF register1, register2
    - SHF register1, immed
### 4.19 ROT
- Opcode: 18
- Desc: Bitwise ROTATE register1 by a value
     - If value is positive it will rotate right 
     - If value is negative it will rotate left  
- Flags: Eqls,Less,Grtr
- Usage: 
    - ROT register1, register2 + immed
    - ROT register1, register2
    - ROT register1, immed
### 4.20 NEG
- Opcode: 19
- Desc: Negates the value of register1 (0-register1)
- Flags: Eqls,Less,Grtr
- Usage: 
    - NEG register1
### 4.21 CMP
- Opcode: 20
- Desc: Compares register1 againts a value (does not store in register1)  
- Flags: Eqls,Less,Grtr
- Usage: 
    - CMP register1, register2 + immed
    - CMP register1, register2
    - CMP register1, immed
### 4.22 JMP
- Opcode: 21
- Desc: Sets PC to an address
     - The instance flags are set as: 
          - Equal = 0, Less = 0, Greater = 0 
- Flags: N/A
- Usage: 
    - JMP register1 + immed
    - JMP register1
    - JMP immed
### 4.23 DLY
- Opcode: 22
- Desc: Delays for a certain amount of clock cycles
- Flags: N/A
- Usage: 
    - DLY register2 + immed
    - DLY register2 
    - DLY immed
### 4.24 LUP
- Opcode: 23
- Desc: The "Loop" instruction.  Decrements register1 and jumps to the calculated target address if the register is non-zero
- Flags: N/A
- Usage: 
    - LUP register1, register2 + immed
    - LUP register1, register2 
    - LUP register1, immed

##***\*Pseudo instructions:** These instructions can be synthesized with a single different instruction. Reference the opcode for this.*
### 4.25 INC
- Opcode: 9
- Desc: Increments register1 
- Usage: 
    - INC register1
### 4.26 DEC
- Opcode: 10
- Desc: Decrements register1 
- Usage: 
    - DEC register1  
### 4.27 RET
- Opcode: 4
- Desc: Returns from a previous CAL instruction
- Usage: 
    - RET 

### 4.28 JE
- Opcode: 21
- Desc: Sets PC to address if EQUAL
     - The instance flags are set as: 
          - Equal = 1, Less = 0, Greater = 0 
- Flags: N/A
- Usage: 
    - JE register1 + immed
    - JE register1
    - JE immed   
### 4.29 JNE
- Opcode: 21
- Desc: Sets PC to address if NOT EQUAL
     - The instance flags are set as: 
          - Equal = 0, Less = 1, Greater = 1 
- Flags: N/A
- Usage: 
    - JNE register1 + immed
    - JNE register1
    - JNE immed    
### 4.30 JL
- Opcode: 21
- Desc: Sets PC to address if LESS
     - The instance flags are set as: 
          - Equal = 0, Less = 1, Greater = 0 
- Flags: N/A
- Usage: 
    - JL register1 + immed
    - JL register1
    - JL immed     
### 4.31 JLE
- Opcode: 21
- Desc: Sets PC to address if LESS OR EQUAL
     - The instance flags are set as: 
          - Equal = 1, Less = 1, Greater = 0 
- Flags: N/A
- Usage: 
    - JLE register1 + immed
    - JLE register1
    - JLE immed    
### 4.32 JG
- Opcode: 21
- Desc: Sets PC to address if GREATER
     - The instance flags are set as: 
          - Equal = 1, Less = 0, Greater = 1 
- Flags: N/A
- Usage: 
    - JG register1 + immed
    - JG register1
    - JG immed     
### 4.33 JGE
- Opcode: 21
- Desc: Sets PC to address if GREATER OR EQUAL
     - The instance flags are set as: 
          - Equal = 0, Less = 0, Greater = 1 
- Flags: N/A
- Usage: 
    - JGE register1 + immed
    - JGE register1
    - JGE immed
<a/>

<br/>

<a name="ref">

## 5. Instruction Opcodes

***Table:** Quick Referece for all the Opcodes and Flags set by Each Instruction*
|       Instructions             |   Opcode     |
|--------------------------------|:------------:|
|      SET                       |     0        |
|      LOD                       |     1        |
|      STR                       |     2        |
|      PSH                       |     3        |
|      POP                       |     4        |
|      BTS                       |     5        |
|      BTC                       |     6        |
|      BTF                       |     7        |
|      CAL                       |     8        |
|      ADD                       |     9        |
|      SUB                       |     10       |
|      MPY                       |     11       |
|      DIV                       |     12       |
|      MOD                       |     13       |
|      AND                       |     14       |
|      OR                        |     15       |
|      XOR                       |     16       |
|      SHF                       |     17       |
|      ROT                       |     18       |
|      NEG                       |     19       |
|      CMP                       |     20       |
|      JMP                       |     21       |
|      DLY                       |     22       |
|      LUP                       |     23       |

<a/>

***Fun fact:** If all words of memory are initialized to 0x0480, then if-ever a program should try to run code beyond that loaded into memory, the fetched instruction will be 0x0480, which decodes into "set PC, rZ + 0x0"... so the program will start over automatically since the first line of code is at 0x0000.​*

<a name="syn">

## 6. Syntax
-   A line can start with any of the following
    -   .const for a constant line
        -   .const  Identifier  Value
        -   .const EXAMPLE 0xBEEF
    -   .var for a variable line
        -   .var Identifier Value
        -   .var example 0xBEEF
    -   ! for a label
        -   in this case the label includes the starting symbol
        -   !example
### 6.1 Instructions
-   An instruction this can be any of the instructions defined, above following there Syntax
-   Immediate values these insert into memory at the location they are found in code all the immediate values
        found on that line
    -   JMP !test <br/>  
        0xBEEF 0xFEED 10 'r' 0b1111_0000_0000_0_1_0_0
        -   This will insert immedate values in memory following the jump to test instruction 
    -   !test
            0xBeef 0xFeed 10  'r' 0b1111_0000_0000_0_1_0_0
        -   Here the test label points the the address of 0xBeef and can be used as the starting address of an array
            of these values.
### 6.2 Comments
   -   Use a semicolon ';' to start a comment
    -   This will make everything past the ; a comment on that line
### 6.3 Labels
   -   Use an exclamation point '!' for a label
        -   in this case the label includes the starting symbol
        -   !example
### 6.4 Constants
   -   Use the .const identifier for a constant line
        -   .const  Identifier  Value
        -   .const EXAMPLE 0xBEEF
### 6.5 Variables
   -   Use the .var identifier for a variable line
        -   .var Identifier Value
        -   .var example 0xBEEF
### 6.6 Etc.

<a/>

<a name="sam">

## 7. Sample Program

<a/>
