out $zero, $zero, $imm2, $imm1, 0, 0		                # disable irq0 (hdd)

add $t0, $imm1, $zero, $zero, 7, 0                               # sector = 7

MAIN:
    out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector
    out $zero, $imm1, $zero, $imm2, 16, 32                       # r/w to/from memory addresses 32 - 48
    out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read

    LOOP0:
        in $t1, $imm1, $zero, $zero, 17, 0                       # Read interrupt irq0
        beq $zero, $t1, $imm1, $imm2, 1, LOOP0                   # Pause here until done reading (interrupt raised)

    add $t0, $imm1, $t0, $zero, 1, 0                             # sector++
    out $zero, $imm1, $zero, $t0, 15, 0                          # write to sector 8
    out $zero, $imm1, $zero, $imm2, 14, 2                        # initiate disk write

    LOOP1:
        in $t1, $imm1, $zero, $zero, 17, 0                       # Read interrupt irq0
        beq $zero, $t1, $imm1, $imm2, 1, LOOP1                   # Pause here until done writing (interrupt raised)

    beq $zero, $t0, $imm1, $imm2, 1, EXIT                        # If done with all 8 sectors exit the program
    sub $t0, $t0, $imm1, $zero, 2, 0                             # sector = sector - 2
    beq $zero, $zero, $zero, $imm2, 1, MAIN                      # loop again

    EXIT:
        halt $zero, $zero, $zero, $zero, 0, 0

    

#.disksector 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
#.disksector 7 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1