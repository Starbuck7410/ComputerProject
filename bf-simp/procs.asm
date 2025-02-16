# First, the program will load the typeface sector from disk to the final 512 bytes of memory, 
# It'll be printed by using a double loop which sets hwregisters 20 (address) and 21 (color) before calling interrupt 22
# Character will be in $a0, $a1 will be x and $a2 will be y.
# for example PRINT(64, 0, 0) will print onto the screen the @ symbol, which will be stored as:
# 0110 1001 1011 1011 1000 0111 = 69BB87 which is 3 bytes.
#  0110
#  1001
#  1011
#  1011
#  1000
#  0111
# ready?

.word 0 0  # at dmem[0] i will store which secotr is the typeface sector
#.disksector 0 0x00 0x00 0x00 0x22 0x22 0x02 0x55 0x00 0x00 0xAF 0xAA 0xFA 0x27 0x63 0x72 0x51   # 32-36
#.disksector 1 0x22 0x45 0x8B 0x8C 0xAC 0x22 0x00 0x00 0x12 0x22 0x21 0x84 0x44 0x48 0x52 0x50   # 37-41
#.disksector 2 0x00 0x00 0x27 0x20 0x00 0x00 0x48 0x00 0x07 0x00 0x00 0x00 0x04 0x11 0x22 0x44   # 42-47

#.disksector 3 0x69 0xBD 0x96 0x26 0x22 0x27 0x69 0x16 0x8F 0x69 0x21 0x96 0x26 0xAF 0x22 0xF8   # 48-52
#.disksector 4 0xE1 0x96 0x68 0xE9 0x96 0xF1 0x24 0x44 0x69 0x69 0x96 0x69 0x97 0x16 0x00 0x20   # 53-57
#.disksector 5 0x20 0x00 0x20 0x22 0x01 0x24 0x21 0x00 0xE0 0xE0 0x08 0x42 0x48 0xE1 0x64 0x04   # 58-63

#.disksector 6 0x69 0xBB 0x87 0x69 0x9F 0x99 0xE9 0xE9 0x9E 0x69 0x88 0x96 0xE9 0x99 0x9E 0xF8    # 64-68
#.disksector 7 0xE8 0x8F 0xF8 0xE8 0x88 0x69 0x8B 0x96 0x99 0xF9 0x99 0x72 0x22 0x27 0x11 0x11    # 69-73
#.disksector 8 0x96 0x9A 0xCC 0xA9 0x88 0x88 0x8F 0x9F 0xF9 0x99 0x9D 0xB9 0x99 0x69 0x99 0x96    # 74-79

#.disksector 9 0xE9 0x9E 0x88 0x69 0x99 0xA5 0x69 0x9E 0xA9 0x78 0x61 0x1E 0x72 0x22 0x22 0x99    # 80-84
#.disksector 10 0x99 0x96 0x99 0x99 0x66 0x99 0x9F 0xF9 0x99 0x66 0x99 0x55 0x52 0x22 0xF1 0x24  # 85-89
#.disksector 11 0x8F 0x32 0x22 0x23 0x44 0x22 0x11 0xC4 0x44 0x4C 0x4A 0x00 0x00 0x00 0x00 0x0F  # 90-94

#.disksector 12 0x42 0x00 0x00 0x00 0x79 0x97 0x88 0xE9 0x9E 0x00 0x69 0x87 0x11 0x79 0x97 0x00   # 95-99
#.disksector 13 0x6F 0x87 0x12 0x72 0x22 0x07 0x97 0x1E 0x88 0xE9 0x99 0x20 0x22 0x22 0x10 0x11   # 100-104 
#.disksector 14 0x96 0x88 0x9A 0xE9 0x22 0x22 0x21 0x00 0xEF 0x99 0x00 0xE9 0x99 0x00 0x69 0x96   # 105-112

#.disksector 15 0x0E 0x99 0xE8 0x07 0x99 0x71 0x00 0xE9 0x88 0x00 0x7C 0x3E 0x02 0x72 0x21 0x00   # 113-117
#.disksector 16 0x99 0x97 0x00 0x99 0x66 0x00 0x99 0xF7 0x00 0x96 0x69 0x09 0x97 0x1E 0x00 0xF2   # 118-122
#.disksector 17 0x4F 0x12 0x62 0x21 0x22 0x22 0x22 0x84 0x64 0x48 0x00 0x5A 0x00 0x00 0x00 0x00   # 123-127


add $sp, $imm1, $zero, $zero, 512, 0  # Initialize the stack pointer
lw $t0, $zero, $zero, $zero, 0, 0  # Load the word in dmem[0] to the correct sector
out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector $t0
mac $gp, $imm1, $imm2, $zero, 512, 7  # calculate memory address of 512*7 (last place where sector fits)
out $zero, $imm1, $zero, $gp, 16, 0                      # r/w to/from memory addresses 512 * 7
out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read

LOOP:
    WAIT_0:
            in $t1, $imm1, $zero, $zero, 17, 0                       # Read interrupt irq0
            beq $zero, $t1, $imm1, $imm2, 1, WAIT_0                   # Pause here until done writing (interrupt raised)

    add $t0, $t0, $imm1, $zero, 1, 0  # increment sector
    out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector $t0
    add $gp, $gp, $imm1, $zero, 16, 0  # increment memory address by 16
    out $zero, $imm1, $zero, $gp, 16, 0                      # r/w to/from memory addresses 512 * 7 + 16
    out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read
bne $zero, $t0, $imm1, $imm2, 18, LOOP  # if sector != 18, go to LOOP

mac $gp, $imm1, $imm2, $zero, 512, 7  # calculate memory address of 512*7 (last place where sector fits)
add $t0, $zero, $zero, $zero, 0, 0  # reset $t0 for program start
add $a1, $imm1, $zero, $zero, 0, 0
add $a2, $imm1, $zero, $zero, 0, 0

# My favorite part is when shraga said "it's printin' time" and then printed all over the bad g-
# Ohh yeah. It's printin' time.


halt $0, $0, $0, $0, 0, 0


# PRINT PROCEDURE

# $a0 is the character in ascii
# $a1 is the x coordinate
# $a2 is the y coordinate
# $t0 is the loop index
# $t1 is the output bit (which will get lshifted by 8)
# $t2 is the address calculated by x + y*256
# $s0 is the character address offset from gp
# $s1 is the character byte

PRINT:

    blt $zero, $a1, $imm1, $imm2, 250, NOPE_3
    add $a1, $zero, $zero, $zero, 0, 0
    add $a2, $a2, $imm1, $zero, 7, 0

    NOPE_3:
    add $sp, $sp, $imm1, $zero, -5, 0  # push $ra
    sw $t0, $sp, $imm1, $zero, 0, 0
    sw $t1, $sp, $imm1, $zero, 1, 0
    sw $t2, $sp, $imm1, $zero, 2, 0
    sw $s0, $sp, $imm1, $zero, 3, 0
    sw $s1, $sp, $imm1, $zero, 4, 0

    
    # Let's start with 1 byte:
    add $s0, $a0, $imm1, $zero, -32, 0  # Handle ascii printable character offset of 32 chars
    mac $s0, $s0, $imm1, $zero, 3, 0  # $s0 = $s0 * 3 (each character is 3 bytes)



    PRINT_BYTE:

        lw $s1, $gp, $s0, $zero, 0, 0  # Load the character from memory

        add $t0, $imm1, $zero, $zero, 128, 0  # mask = 0x10000000

        LOOP_BYTE:
            add $t1, $imm1, $zero, $zero, 0, 0  # reset $t1
            and $t1, $t0, $s1, $imm1, 0x0FF, 0  # $t1 = $t0 & $s0   
            mac $t2, $a2, $imm1, $a1, 256, 0  # $t2 = $a1 + $a2*256 
            out $zero, $imm1, $zero, $t2, 20, 0  # set address
            beq $zero, $t1, $zero, $imm2, 0, NOPE_1  # if $t1 == 0, go to NOPE_1

            # NOPE
            add $t1, $imm1, $zero, $zero, 255, 0  # $t1 = 255
        NOPE_1:
            out $zero, $imm1, $zero, $t1 , 21, 0  # set color
            out $zero, $imm1, $zero, $imm2, 22, 1  # call interrupt 22
            srl $t0, $t0, $imm1, $zero, 1, 0  # $t0 = $t0 >> 1
            add $a1, $a1, $imm1, $zero, 1, 0  # $a1++
            bne $zero, $t0, $imm1, $imm2, 8, NOPE_2  # if $t0 != 0x1000, go to NOPE_2

            # NOPE
            add $a1, $a1, $imm1, $zero, -4, 0  # $a1 -= 4, return to the beginning of the line
            add $a2, $a2, $imm1, $zero, 1, 0  # $a2++
        NOPE_2:
            bne $zero, $t0, $imm1, $imm2, 0, LOOP_BYTE  # if $t0 != 0, go to LOOP_BYTE  

            
            add $s0, $s0, $imm1, $zero, 1, 0  # $s0++
            add $a1, $a1, $imm1, $zero, -4, 0  # $a1 -= 4, return to the beginning of the line
            add $a2, $a2, $imm1, $zero, 1, 0  # $a2++

            # max character address = ($a0 - 32) * 3 + 3
            sub $t0, $a0, $imm1, $zero, 32, 0  # $t0 = $a0 - 32
            mac $t0, $t0, $imm1, $imm1, 3, 0  # $t0 = $t0 * 3 + 3

            bne $zero, $s0, $t0, $imm2, 0, PRINT_BYTE  # if $s0 != ($a0 - 32) * 3 + 3, go print another byte

            add $a1, $a1, $imm1, $zero, 5, 0  # $a0++
            add $a2, $a2, $imm1, $zero, -6, 0  # $a1 += 6

            lw $t0, $sp, $imm1, $zero, 0, 0  # pop $t0
            lw $t1, $sp, $imm1, $zero, 1, 0  # pop $t1
            lw $t2, $sp, $imm1, $zero, 2, 0  # pop $t2
            lw $s0, $sp, $imm1, $zero, 3, 0  # pop $s0
            lw $s1, $sp, $imm1, $zero, 4, 0  # pop $s1
            add $sp, $sp, $imm1, $zero, 5, 0  # return the stack pointer to its original position
            beq $zero, $zero, $zero, $ra, 0, 0  # return to caller