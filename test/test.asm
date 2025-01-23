#out $zero $imm1 $zero $imm2 16 0
out $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
out $zero, $imm1, $zero, $imm2, 6, INT		# set irqhandler as L3
in $s1, $imm1, $zero, $zero 6
sw $s1, $imm1, $zero, $zero, 11 0
sw $imm1 $imm2 $zero $zero 1 12
# out $zero $imm1 $zero $imm2 14 1
add $zero $zero $zero $zero 0 0
add $zero $zero $zero $zero 0 0
add $zero $zero $zero $zero 0 0
add $zero $zero $zero $zero 0 0
halt $zero $zero $zero $zero 0 0

INT:
    sw $imm1 $imm2 $zero $zero 69 60
    reti $zero $zero $zero $zero 0 0
.word 2 0x45
.interrupt 0x56
.disksector 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0