; syntax.asm
;
; TeenyAT example of all allowed assembly syntax as supported by tnasm.

!label  ; Labels are equivalent to the address of whatever code or data might
        ; follow.  Labels start with a "!" and are then followed by non-
        ; whitespace and non-"!" characters.

;;;;;;;;; ALU Instructions ;;;;;;;;;
add rA, rB        ; same as... rA += rB
add rA, 72        ; same as... rA += 72
add rA, rC + 13   ; same as... rA += rC + 13

sub rA, rB        ; same as... rA -= rB
sub rA, 72        ; same as... rA -= 72
sub rA, rC + 13   ; same as... rA -= rC + 13

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
and rA, rC + 13   ; same as... rA &= rC + 13

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

rol rA, rB        ; same as... rA += rB
rol rA, 72        ; same as... rA += 72
rol rA, rC + 13   ; same as... rA += rC + 13
