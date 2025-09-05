main:   
        add $gp, $imm1, $zero, $zero, 0x100, 0
        sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp to 2048
        jal  $ra, $zero, $zero, $imm1, circle, 0        # call circle
        halt $zero, $zero, $zero, $zero, 0, 0

circle:
        add  $sp, $sp, $imm1, $zero, -4, 0
        sw   $s0, $sp, $imm1, $zero, 0, 0
        sw   $s1, $sp, $imm1, $zero, 1, 0
        sw   $s2, $sp, $imm1, $zero, 2, 0
        sw   $ra, $sp, $imm1, $zero, 3, 0
        lw   $s0, $imm1,   $zero, $zero, 0x100, 0   # s0=radius
        add  $s1, $zero, $zero, $zero, 0, 0

loop_i:
        bgt  $zero, $s1, $imm1, $imm2, 8, end_i
        add  $s2, $zero, $zero, $zero, 0, 0

loop_j:
        bgt  $zero, $s2, $imm1, $imm2, 8, end_j
        add  $a0, $s1, $zero, $zero, 0, 0    # y=i
        add  $a1, $s2, $zero, $zero, 0, 0    # x=j
        add  $a2, $s0, $zero, $zero, 0, 0    # r=radius
        jal  $ra, $zero, $zero, $imm1, inside_circle, 0
        sll  $t0, $s1,  $imm1, $zero, 8, 0
        add  $t0, $t0,  $s2,   $zero, 0, 0
        add  $zero, $zero, $zero, $zero, 0, 0   # monitoraddr
        add  $zero, $zero, $zero, $zero, 0, 0   # monitordata
        add  $zero, $zero, $zero, $zero, 0, 0  # monitorcmd=1
        add  $zero, $zero, $zero, $zero, 0, 0  # monitorcmd=0
        add  $s2, $s2, $imm1, $zero, 1, 0
        beq  $zero, $zero, $zero, $imm1, loop_j, 0

end_j:
        add  $s1, $s1, $imm1, $zero, 1, 0
        beq  $zero, $zero, $zero, $imm1, loop_i, 0

end_i:
        lw   $s0, $sp, $imm1, $zero, 0, 0
        lw   $s1, $sp, $imm1, $zero, 1, 0
        lw   $s2, $sp, $imm1, $zero, 2, 0
        add  $zero, $zero, $zero, $zero, 0xBE, 0xBE
        lw   $ra, $sp, $imm1, $zero, 3, 0
        add  $sp, $sp, $imm1, $zero, 4, 0
        beq  $zero, $zero, $zero, $ra, 0, 0   # return

inside_circle:
        sub  $t0, $a0, $imm1, $zero, 3, 0   # dy = y-127
        sub  $t1, $a1, $imm1, $zero, 3, 0   # dx = x-127
        mac  $t0, $t0,  $t0,   $zero, 0, 0    # t0=(y-127)^2
        mac  $t0, $t1,  $t1,   $t0,   0, 0    # t0+=(x-127)^2
        mac  $t1, $a2,  $a2,   $zero, 0, 0    # t1=r^2
        add  $v0, $imm1,$zero, $zero, 0, 0    # v0=0
        bgt  $zero, $t0, $t1, $imm1, out_ic, 0 # if dist^2>r^2 => skip
        add  $v0, $imm1, $zero, $zero, 8, 0 # v0=8
out_ic:
        beq  $zero, $zero, $zero, $ra, 0, 0

# Data: radius=100
.word 0x100, 2

### My findings are as follows:

### Adding anything before line 47 breaks everything unexplainably.
### You can try by shifting this bunch of zeroes marked with
### "What the FUCK" up one line and running the program (It'll fail). 
### I suspected it's a labeling issue, but I tested it and the labels
### are completely identical in a fail and success scenario. 
