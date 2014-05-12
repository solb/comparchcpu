#Creates a program that tests sets

mov $s0, 0
mov $s1, 1

slt $t0, $s0, $s0
slt $t1, $s0, $s1
slt $t2, $s1, $s0

call %printf

sgt $t0, $s0, $s0
sgt $t1, $s0, $s1
sgt $t2, $s1, $s0

call %printf

seq $t0, $s0, $s0
seq $t1, $s0, $s1
seq $t2, $s1, $s0

call %printf

sne $t0, $s0, $s0
sne $t1, $s0, $s1
sne $t2, $s1, $s0

call %printf

sle $t0, $s0, $s0
sle $t1, $s0, $s1
sle $t2, $s1, $s0

call %printf

sge $t0, $s0, $s0
sge $t1, $s0, $s1
sge $t2, $s1, $s0

call %printf

siz $t0, $s0
siz $t1, $s1
mov $t2, 10

call %printf

snz $t0, $s0
snz $t1, $s1
mov $t2, 10

call %printf
halt

printf:
	prnt $t0
	prnt $t1
	prnt $t2
	ret
