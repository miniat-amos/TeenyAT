; TeenyAT Constants
.const PORT_A_DIR                0x8000
.const PORT_B_DIR                0x8001
.const PORT_A                    0x8002
.const PORT_B                    0x8003
.const RAND                      0x8010
.const RAND_BITS                 0x8011
.const INTERRUPT_VECTOR_TABLE    0x8E00
.const INTERRUPT_ENABLE_REGISTER 0x8E10
.const CONTROL_STATUS_REGISTER   0x8EFF

; LCD Peripherals
.const LIVESCREEN 0x9000
.const UPDATESCREEN 0xA000
.const X1 0xD000
.const Y1 0xD001
.const X2 0xD002
.const Y2 0xD003
.const STROKE 0xD010
.const FILL 0xD011
.const DRAWFILL 0xD012
.const DRAWSTROKE 0xD013
.const UPDATE 0xE000
.const RECT 0xE010
.const LINE 0xE011
.const POINT 0xE012
.const MOUSEX 0xFFFC
.const MOUSEY 0xFFFD
.const MOUSEB 0xFFFB
.const TERM 0xFFFF
.const KEY 0xFFFE

.const CHANGE_AMT 10

; Setup interrupt callback by mapping it to external interrupt 8
set rA, !key_pressed
set rB, 8
str [ INTERRUPT_VECTOR_TABLE + rB ], rA

; Enable external interrupt 8
set rA, 0b00000001_00000000
str [ INTERRUPT_ENABLE_REGISTER ], rA

; Enable interrupts globally
set rA, 0b000000000000000_1
str [ CONTROL_STATUS_REGISTER ], rA

; Reset registers
set rA, rZ
set rB, rZ
!main
    str [ LIVESCREEN + rB ], rA
    add rB, 3
    add rA, 3
    mod rB, 4096
    jmp !main

;---------  CALLBACK  ----------------
!key_pressed
    add rA, CHANGE_AMT
    lod rC, [ KEY ]
    str [ TERM ], rC
    rti
