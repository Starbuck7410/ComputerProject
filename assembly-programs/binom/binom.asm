
main:
    lw $a0, $imm1, $zero, $zero, 256, 0             # Load n from memory address 0x100 to $a0
    lw $a1, $imm1, $zero, $zero, 257, 0             # Load k from memory address 0x101 to $a1
    bgt $zero, $a1, $a0, $imm2, 0, ZERO	            # if k > n, return 0
    sll $sp, $imm1, $imm2, $zero, 1, 11		        # set $sp = 1 << 11 = 2048
    jal $ra, $zero, $zero, $imm1, binomial, 0       # Call the recursive function
    sw $v0, $imm1, $zero, $zero, 258, 0             # Store the result at memory address 0x102
    halt $zero, $zero, $zero, $zero, 0, 0

binomial:
    sub $sp $sp, $imm1, $zero, 4, 0		            # allocate space in stack
    sw $ra, $sp, $imm1, $zero, 0, 0 	            # save return address
    sw $a0, $sp, $imm1, $zero, 1, 0	                # save n
    sw $a1, $sp, $imm1, $zero, 2, 0	                # save k
    sw $s0, $sp, $imm1, $zero, 3, 0		            # save s0

    # Check the base case: k = 0 or k = n
    bne $zero, $a1, $zero, $imm1, second_check, 0   # first check is if k=0
    add $v0, $imm1, $zero, $zero, 1, 0	            # v0 = 1
    beq $zero, $zero, $zero, $imm2, 0, L2	        # jump to L2
    second_check:
    bne $zero, $a0, $a1, $imm1, L1, 0               # second check is if n=k
    add $v0, $imm1, $zero, $zero, 1, 0	            # v0 = 1
    beq $zero, $zero, $zero, $imm2, 0, L2	        # jump to L2

    L1:
        # Calculate C(n-1, k-1)
        sub $a0, $a0, $imm1, $zero, 1, 0            # a0 = n-1
        sub $a1, $a1, $imm1, $zero, 1, 0	        # a1 = k-1	
        jal $ra, $zero, $zero, $imm1, binomial, 0   
        add $s0, $v0, $zero, $zero, 0, 0	        # save  first result in $s0
    
        # Calculate C(n-1, k)
        lw $a1, $sp, $imm1, $zero, 2, 0	    	    # k-1 -> k
        jal $ra, $zero, $zero, $imm1, binomial, 0
        add $v0, $v0, $s0, $zero, 0, 0		        # add the two results to v0
    L2:
        lw $ra, $sp, $imm1, $zero, 0, 0	    	    # restore return address
        lw $a0, $sp, $imm1, $zero, 1, 0	            # restore n
        lw $a1, $sp, $imm1, $zero, 2, 0	            # restore k
        lw $s0, $sp, $imm1, $zero, 3, 0	            # restore s0
        add $sp $sp, $imm1, $zero, 4,0		        # close stack
        beq $zero, $zero, $zero, $ra, 0, 0	        # jr $ra
    ZERO:
        add $v0, $zero, $zero, $zero, 0, 0	        # return 0
        sw $zero, $imm1, $zero, $zero, 258, 0         # Store the result at memory address 0x102
        halt $zero, $zero, $zero, $zero, 0, 0
        

.word 0x100, 6
.word 0x101, 3
