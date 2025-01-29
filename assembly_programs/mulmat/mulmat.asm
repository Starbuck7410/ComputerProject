main:       
		sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp to 2048
		add $sp,   $sp,    $imm1, $zero, -4,        0      # allocate 4 spaces in stack
                sw  $s0,   $zero,  $sp,   $zero,  0,         0      # store $s0 in stack
                sw  $s1,   $imm1,  $sp,   $zero,  1,         0      # store $s1 in stack
                sw  $s2,   $imm1,  $sp,   $zero,  2,         0      # store $s2 in stack
                sw  $ra,   $imm1,  $sp,   $zero,  3,        0      # store $ra in stack
                add $a0,   $imm1,  $zero,  $zero,  0,        0      # set rows_i = 0 
                add $a1,   $imm1,  $zero,  $zero,  0,        0      # set columns_i = 0 
                add $a2,   $imm1,  $zero,  $zero,  0x100,    0      # pointer to matrix

loopi:          
                bgt $zero, $a0,    $imm1,  $imm2, 3,         end    # end case if row_i>3
loopj:          
                bgt $zero, $a1,    $imm1,  $imm2, 3,         loopi  # end case if column_i > 3
                jal $ra,   $zero,  $zero,  $imm1, vector_multi,0      # call vector_multi for row i - column j
                add $a1,   $imm1,  $a1,  $zero, 1,         0      # increment columns_i
                blt $zero, $a1,    $imm1,  $imm2, 4,         loopj 
                                                                    #end of loop j in loop i
                add $a0,   $imm1,  $a0,  $zero,  1,        0      #rows_i++
                add $a1,   $imm1,  $zero,  $zero,  0,        0      #columns_i = 0 
                beq $zero, $zero,  $zero,  $imm1,   loopi,    0
vector_multi:
                mac $s1,   $a0,    $imm1, $a2,    4,         0      # pointer to left vector
                add $s2,   $a1,    $a2,   $imm1,  16,        0      # pointer to right vector
                add $t0,   $zero,  $zero  $zero,  0,         0      # vectors elements index - ve_i [0:3]
                add $s0,   $zero,  $zero  $zero,  0,         0      # initialize sum
LT:             
                bgt $zero, $t0,    $imm1, $imm2,  3,         store   # End dot product if ve_i > 3
                lw  $t1,   $s1,    $zero  $zero,  0,         0      # load element of left vector
                lw  $t2,   $s2,    $zero  $zero,  0,         0      # load element of right vector
                mac $s0,   $t1,    $t2    $s0,    0,         0      # multiply and add to $s0
                add $t0,   $imm1,  $t0    $zero,  1,         0      # ve_i ++
                add $s1,   $imm1,  $s1    $zero,  1,         0      # move to left vector next element
                add $s2,   $imm1,  $s2    $zero,  4,         0      # move to right vector next element
                beq $zero, $zero,  $zero  $imm1   LT,        0
store:          
                mac $t1,   $a0,    $imm1, $a1,    4,         0      # calculate memory offset in order to save multiplication
                sw  $s0,   $t1,    $imm1, $zero,  0x120,     0      # store result in the result matrix    
                beq $zero, $zero,  $zero  $ra     0,         0      # return to caller
end:           
                lw  $sp,   $zero,  $s0,   $zero,  0,         0      # load $s0 from stack
                lw  $sp,   $imm1,  $s1,   $zero,  4,         0      # load $s1 from stack
                lw  $sp,   $imm1,  $s2,   $zero,  8,         0      # load $s2 from stack
                lw  $sp,   $imm1,  $ra,   $zero,  12,        0      # load $ra from stack
                add $sp,   $sp,    $imm1, $zero,  16,        0      # deallocate stack space            
		halt $zero, $zero, $zero, $zero,  0,         0	    # halt program
.word 256,1
.word 257,2
.word 258,3
.word 259,4
.word 260,5
.word 261,6
.word 262,7
.word 263,8
.word 264,9
.word 265,10
.word 266,11
.word 267,12
.word 268,13
.word 269,14
.word 270,15
.word 271,16
.word 272,17
.word 273,18
.word 274,19
.word 275,20
.word 276,21
.word 277,22
.word 278,23
.word 279,24
.word 280,25
.word 281,26
.word 282,27
.word 283,28
.word 284,29
.word 285,30
.word 286,31
.word 287,32

 