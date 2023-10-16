;crappy test program reverses array including array terminating value which is arbitrarily set to #FFFF
JMP !print
!Screen #8000

!Array
   @2 45 12 3 67 34 10 #ffff

!print
    SET $7 $z
!loop
    LOD $5 $7 !Array
    STR $7 !Screen $5
    INC $7
    CMP $5 65535
    JE !main
    JMP !loop

!main
    SET $4 $z ;set register 3 to the value of the zero register always zero
    SET $7 $z ;set register 7 to the value of the zero register which is always zero
!loopforlast
    LOD $5 $7 !Array ;load first value of the Array
    CMP $5 65535
    JE !lastfound ;address of last element is in $7
    INC $7
    JMP !loopforlast
!lastfound
    LOD $5 $4 !Array
    LOD $6 $7 !Array
    STR $4 !Array $6
    STR $7 !Array $5
    INC $4
    DEC $7
    CMP $4 $7
    JGE !endofprogram
    JMP !lastfound
!endofprogram

!printtwo
    SET $7 $Z
!looptwo
    LOD $5 $7 !Array
    STR $7 !Screen $5
    INC $7
    CMP $5 65535
    JE !print
    JMP !looptwo


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
