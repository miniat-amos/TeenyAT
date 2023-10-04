;crappy test program reverses array including array terminating value which is arbitrarily set to #FFFF

main:
    SET $4 $3 ;set register 3 to the value of the zero register always zero
    SET $7 $3 ;set register 7 to the value of the zero register which is always zero
loopforlast:
    LOD $5 [$7 + Array] ;load first value of the Array
    CMP $5 #FFFF
    JE lastfound ;address of last element is in $7
    ADD $7 1
    JMP loopforlast
lastfound:
    LOD $5 [$4 + Array]
    LOD $6 [$7 +  Array]
    STR [$4 + Array] $6
    STR [$7 + Array] $5
    ADD $4 1
    SUB $7 1
    CMP $4 $7
    JGE endofprogram
    JUMP lastfound
endofprogram:

@data
Array: 
    2 45 12 3 67 34 10 #FFFF


;first pass
;first thing on each line can be a instruction or a label or a variable
;instructions start with a ascii letter and have no colon
;labels start with ascii and do have a colon
;variables start with a .
;data section starts with @

;if a label add to map with mapping the string to the current address remove the label line
;if @data ignore it for now
;if instruction figure out if it is teeny and increase the current address appropriately
;if variable add to map mapping the string to the value remove the variable assignment line

;second pass
;replace all instances of variables and labels in asm code with the value they represent

;third pass
;go through each instruction make the encoding and place it in memory in the correct location
;once we hit @data just place the whitespace seperated values in memory as half words or 16 bit values
