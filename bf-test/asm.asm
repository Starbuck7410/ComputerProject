# First, the program will load the typeface sector from disk to the final 512 bytes of memory, 
# It'll be printed by using a double loop which sets hwregisters 20 (address) and 21 (color) before calling interrupt 22
# Character will be in $a0, $a1 will be x and $a2 will be y.
# x and y will be calculated using the screen size (256x256) and character size ((4+1)*(6+1))
# for example PRINT(64, 0, 0) will print onto the screen the @ symbol, which will be stored as:
# 0110 1001 1011 1011 1000 0111 = 69BB87 which is 3 bytes.
# ready?

.word 0 0  # at dmem[0] i will store which secotr is the typeface sector 

# just the first few characters... note the last one stops in the middle.
#.disksector 0 0x00 0x00 0x00 0x22 0x22 0x02 0x55 0x00 0x00 0xAF 0xAA 0xFA 0x27 0x63 0x72 0x00   

lw $t0, $zero, $zero, $zero, 0, 0  # Load the word in dmem[0] to the correct sector
out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector $t0
mac $gp, $imm1, $imm2, $zero, 512, 7  # calculate memory address of 512*7 (last place where sector fits)
out $zero, $imm1, $zero, $gp, 16, 0                      # r/w to/from memory addresses 32 - 48
out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read
add $t0, $zero, $zero, $zero, 0, 0  # reset $t0 for program start

# My favorite part is when shraga said "it's printin' time" and then printed all over the bad g-
# It's printin' time.

add $a0, $imm1, $zero, $zero, 33, 0
add $a1, $imm1, $zero, $zero, 0, 0
add $a2, $imm1, $zero, $zero, 0, 0

jal $ra, $zero, $zero, $imm1, PRINT, 0

halt $0, $0, $0, $0, 0, 0


# $t0 is the loop index
# $t1 is the output bit (which will get lshifted by 8)
# $t2 is the address calculated by x + y*256

PRINT:
    
    # Let's start with 1 byte:
    add $t0, $imm1, $zero, $zero, 128, 0

    LOOP:

        add $t1, $imm1, $zero, $zero, 0, 0  # reset $t1
        and $t1, $t0, $a0, $zero, 0, 0  # $t1 = $t0 & $a0   
        mac $t2, $a2, $imm1, $a1, 256, 0  # $t2 = $a1 + $a2*256 
        out $zero, $imm1, $zero, $t2, 20, 0  # set address
        beq $t1, $zero, $zero, $imm2, 0, NOPE  # if $t1 == 0, go to NOPE
        add $t1, $imm1, $zero, $zero, 255, 0  # $t1 = 255
        
    NOPE:
        out $zero, $imm1, $zero, $imm2 , 21, 255  # set color
        # out $zero, $imm1, $zero, $t1 , 21, 0  # set color
        out $zero, $imm1, $zero, $imm2, 22, 1  # call interrupt 22
        srl $t0, $t0, $imm1, $zero, 1, 0  # $t0 = $t0 >> 1
        add $a1, $a1, $imm1, $zero, 1, 0  # $a1++
        bne $zero, $t0, $imm1, $imm2, 0, LOOP  # if $t0 != 0, go to LOOP  
        beq $zero, $zero, $zero, $ra, 0, 0  # return to caller

        

        
        