out $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
out $zero, $zero, $imm2, $imm1, 1, 0		# enable irq0 (hdd)
out $zero, $imm1, $zero, $imm2, 6, INT      # set irqhandler as L3
in $s1, $imm1, $zero, $zero 6 0
sw $s1, $imm1, $zero, $zero, 11 0
sw $imm1 $imm2 $zero $zero 1 12
out $zero $imm1 $zero $imm2 16 32 # read to memory addresses 32 - 48
out $zero $imm1 $zero $imm2 14 1 # initiate disk read
out $zero $imm1 $zero $imm2 15 1 # write to sector 1
add $zero $zero $zero $zero 0 0
add $zero $zero $zero $zero 0 0
add $zero $zero $zero $zero 0 0
beq $zero $zero $zero $imm1 9 0
halt $zero $zero $zero $zero 0 0

INT:
    out $zero $imm1 $zero $imm2 14 2 # initiate disk write
    sw $imm1 $imm2 $zero $zero 69 60
    halt $zero $zero $zero $zero 0 0
    reti $zero $zero $zero $zero 0 0
.word 2 0x45
#.interrupt 0x56
.disksector 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
#.interrupt 12