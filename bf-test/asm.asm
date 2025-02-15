add $s0, $imm1, $imm1, $zero, 1024, 0  # Initialize the memory pointer
add $sp, $imm1, $zero, $zero, 512, 0  # Initialize the output stack pointer
# Program starts here

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store


add $s0, $s0, $imm1, $zero, 1, 0  # Move pointer to the right

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store

lw $t0, $s0, $zero, $zero, 0, 0  # load current byte
sw $t0, $sp, $zero, $zero, 0, 0  # store in output stack
add $sp, $sp, $imm1, $zero, 1, 0  # increment output stack pointer


add $s0, $s0, $imm1, $zero, 1, 0 # Move pointer to the left

lw $t0, $s0, $zero, $zero, 0, 0  # load current byte
sw $t0, $sp, $zero, $zero, 0, 0  # store in output stack
add $sp, $sp, $imm1, $zero, 1, 0  # increment output stack pointer

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store

lw $t0, $s0, $zero, $zero, 0, 0  # load
sw $t0, $s0, $zero, $imm1, 1, 0  # add and store

lw $t0, $s0, $zero, $zero, 0, 0  # load current byte
sw $t0, $sp, $zero, $zero, 0, 0  # store in output stack
add $sp, $sp, $imm1, $zero, 1, 0  # increment output stack pointer


halt $zero, $zero, $zero, $zero, 0, 0 # Stop the program

