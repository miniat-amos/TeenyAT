%token_prefix T_

lang ::= loc EOL.

loc ::= variable_line.
loc ::= constant_line.
loc ::= raw_line.
loc ::= label_line.
loc ::= code_1_line.
loc ::= code_2_line.
/*
loc ::= code_3_line.
loc ::= code_4_line.
loc ::= code_5_line.
loc ::= code_6_line.
loc ::= code_7_line.
loc ::= code_8_line.
loc ::= code_9_line.
loc ::= code_10_line.
loc ::= code_11_line.
loc ::= code_12_line.
loc ::= code_13_line.
loc ::= code_14_line.
*/
variable_line ::= VARIABLE IDENTIFIER immediate.

constant_line ::= CONSTANT IDENTIFIER immediate.

raw_line ::= raw_line number.
raw_line ::= number.

label_line ::= LABEL.

immediate ::= number.
immediate ::= LABEL.

number ::= PLUS NUMBER.
number ::= MINUS NUMBER.
number ::= NUMBER.

code_1_line ::= code_1_inst REGISTER COMMA REGISTER.
code_2_line ::= code_2_inst REGISTER COMMA REGISTER add_or_subtract immediate.
/*
code_3_line ::= code_3_inst REGISTER COMMA immediate.
code_4_line ::= code_4_inst REGISTER.
code_5_line ::= code_5_inst REGISTER.
code_6_line ::= code_6_inst REGISTER add_or_subtract immediate COMMA REGISTER.
code_7_line ::= code_7_inst immediate COMMA REGISTER.
code_8_line ::= code_8_inst REGISTER.
code_9_line ::= code_9_inst REGISTER add_or_subtract immediate.
code_10_line ::= code_10_inst immediate.
code_11_line ::= code_11_inst.
code_12_line ::= code_12_inst REGISTER.
code_13_line ::= code_13_inst REGISTER add_or_subtract immediate.
code_14_line ::= code_14_inst immediate.
*/

add_or_subtract ::= PLUS.
add_or_subtract ::= MINUS.

code_1_inst ::= ADD.
code_1_inst ::= SUB.
code_1_inst ::= MPY.
code_1_inst ::= DIV.
code_1_inst ::= MOD.
code_1_inst ::= AND.
code_1_inst ::= OR.
code_1_inst ::= XOR.
code_1_inst ::= SHF.
code_1_inst ::= ROT.
code_1_inst ::= SET.
code_1_inst ::= LOD.
code_1_inst ::= STR.
code_1_inst ::= BTF.
code_1_inst ::= CMP.
code_1_inst ::= DJZ.

code_2_inst ::= ADD.
code_2_inst ::= SUB.
code_2_inst ::= MPY.
code_2_inst ::= DIV.
code_2_inst ::= MOD.
code_2_inst ::= AND.
code_2_inst ::= OR.
code_2_inst ::= XOR.
code_2_inst ::= SHF.
code_2_inst ::= ROT.
code_2_inst ::= SET.
code_2_inst ::= LOD.
code_2_inst ::= BTF.
code_2_inst ::= CMP.
code_2_inst ::= DJZ.

/*
code_3_inst ::= ADD.
code_3_inst ::= SUB.
code_3_inst ::= MPY.
code_3_inst ::= DIV.
code_3_inst ::= MOD.
code_3_inst ::= AND.
code_3_inst ::= OR.
code_3_inst ::= XOR.
code_3_inst ::= SHF.
code_3_inst ::= ROT.
code_3_inst ::= SET.
code_3_inst ::= LOD.
code_3_inst ::= BTF.
code_3_inst ::= CMP.
code_3_inst ::= DJZ.

code_4_inst ::= NEG.
code_4_inst ::= POP.

code_5_inst ::= INC.
code_5_inst ::= DEC.

code_6_inst ::= STR.

code_7_inst ::= STR.

code_8_inst ::= PSH.
code_8_inst ::= CAL.

code_9_inst ::= PSH.
code_9_inst ::= CAL.

code_10_inst ::= PSH.
code_10_inst ::= CAL.

code_11_inst ::= RET.

code_12_inst ::= JMP.
code_12_inst ::= JE.
code_12_inst ::= JNE.
code_12_inst ::= JL.
code_12_inst ::= JLE.
code_12_inst ::= JG.
code_12_inst ::= JGE.

code_13_inst ::= JMP.
code_13_inst ::= JE.
code_13_inst ::= JNE.
code_13_inst ::= JL.
code_13_inst ::= JLE.
code_13_inst ::= JG.
code_13_inst ::= JGE.

code_14_inst ::= JMP.
code_14_inst ::= JE.
code_14_inst ::= JNE.
code_14_inst ::= JL.
code_14_inst ::= JLE.
code_14_inst ::= JG.
code_14_inst ::= JGE.
*/

/*
T_SET
T_LOD
T_STR
T_PSH
T_POP
T_BTS
T_BTC
T_BTF
T_CAL
T_ADD
T_SUB
T_MPY
T_DIV
T_MOD
T_AND
T_OR
T_XOR
T_SHF
T_ROT
T_NEG
T_CMP
T_JMP
T_DJZ
T_INC
T_DEC
T_RET
T_LABEL
T_VARIABLE
T_CONSTANT
T_REGISTER
T_IDENTIFIER
T_NUMBER
T_PLUS
T_MINUS
T_COMMA
T_LBRACKET
T_RBRACKET
*/
