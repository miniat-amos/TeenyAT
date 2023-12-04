; syntax.asm
;
; TeenyAT example of all allowed assembly syntax as supported by tnasm.

!label  ; Labels are equivalent to the address of whatever code or data might
        ; follow.  Labels start with a "!" and are then followed by non-
        ; whitespace and non-"!" characters.

;--- Registers ---
; The eight registers supported by tnasm are:
; rA, rB, rC, rD, rE: five general purpose registers
; PC: the Program Counter register
; SP: the Stack Pointer register
; rZ: the Zero Register (always contains the value zero)
; r0--r7: Numeric reference to registers is also supported

;--- Variables and Constants ---
; Declare variables using the .var or .variable directives.  If no initial value
; is provided at the time of declaration, the value is undefined.
; Define constants using the .const or .constant directives.  These require an
; initial value.

.const TERM 0xFFFE
.constant FOUR 4
.var age 27
.variable count

;--- Common Instruction formats 
; The great majority of instructions have three main forms supported by tnasm.
; These forms are differentiated by whether a particular operand is a register,
; an immediate, or a register added to an immediate.  Not all instructions can
; be written in each of the three forms, but for the sake of clarity, the
; examples in the remainder of this file show ALL acceptable forms for every
; instruction.

;--- ALU Instructions ---
add rA, PC        ; same as... rA += rB
add rA, 72        ; same as... rA += 72
add rA, rC + 13   ; same as... rA += rC + 13

sub rD, rE        ; same as... rD -= rE
sub SP, 4        ; same as... SP -= 4
sub PC, rB - 5   ; same as... PC -= rB - 5

mpy rA, rB        ; same as... rA *= rB
mpy rA, 72        ; same as... rA *= 72
mpy rA, rC + 13   ; same as... rA *= rC + 13

div rA, rB        ; same as... rA /= rB
div rA, 72        ; same as... rA /= 72
div rA, rC + 13   ; same as... rA /= rC + 13

mod rA, rB        ; same as... rA %= rB
mod rA, 72        ; same as... rA %= 72
mod rA, rC + 13   ; same as... rA %= rC + 13

and rA, rB        ; same as... rA &= rB
and rA, 72        ; same as... rA &= 72
and rA, rZ + 13   ; same as... rA &= rC + 13

or rA, rB        ; same as... rA |= rB
or rA, 72        ; same as... rA |= 72
or rA, rC + 13   ; same as... rA |= rC + 13

xor rA, rB        ; same as... rA ^= rB
xor rA, 72        ; same as... rA ^= 72
xor rA, rC + 13   ; same as... rA ^= rC + 13

shl rA, rB        ; same as... rA <<= rB
shl rA, 72        ; same as... rA <<= 72
shl rA, rC + 13   ; same as... rA <<= rC + 13

shr rA, rB        ; same as... rA >>= rB
shr rA, 72        ; same as... rA >>= 72
shr rA, rC + 13   ; same as... rA >>= rC + 13

rol rA, rB        ; same as... rA = (rA << rB) | (rA >> (16 - rB)
rol rA, 4        ; same as... rA = (rA << 4) | (rA >> (16 - 4)
rol rA, rC + 13   ; same as... rA = (rA << (rC + 13)) | (rA >> (16 - (rA + 13)))

ror rA, rB        ; same as... rA = (rA >> rB) | (rA << (16 - rB)
ror rA, 4        ; same as... rA = (rA >> 4) | (rA << (16 - 4)
ror rA, rC + 13   ; same as... rA = (rA >> (rC + 13)) | (rA << (16 - (rA + 13)))
