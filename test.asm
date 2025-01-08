testLabel:
 # hello $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
 # testing comments
 sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp = 1 << 11 = 2048
 out $zero, $imm1, $zero, $imm2, 6, fib		# set irqhandler as L3
 lw $a0, $zero, $imm2, $zero, 0, 64		# get x from address 64
 jal $ra, $zero, $zero, $imm2, 0, fib		# calc $v0 = fib(x)
 sw $zero, $zero, $imm2, $v0, 0, 65		# store fib(x) in 65
 halt $zero, $zero, $zero, $zero, 0, 0		# halt
.word 14 442
AnotherTestLabel:
 # hello $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
 # testing comments
 sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp = 1 << 11 = 2048
 out $zero, $imm1, $zero, $imm2, 6, fib		# set irqhandler as L3
 lw $a0, $zero, $imm2, $zero, 0, 64		# get x from address 64
 jal $ra, $zero, $zero, $imm2, 0, fib		# calc $v0 = fib(x)
 sw $zero, $zero, $imm2, $v0, 0, fib		# store fib(x) in 65
 halt $zero, $zero, $zero, $zero, 0, 0		# halt

fib:
.word 15 444
