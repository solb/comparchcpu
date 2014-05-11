#A test of all bitwise operators

mov $s0, 0
mov $s1, 3

and $t0, $s0, $s0
and $t1, $s0, $s1
and $t2, $s1, $s1

jal %printf

or $t0, $s0, $s0
or $t1, $s0, $s1
or $t2, $s1, $s1

jal %printf

xor $t0, $s0, $s0
xor $t1, $s0, $s1
xor $t2, $s1, $s1

jal %printf

nand $t0, $s0, $s0
nand $t1, $s0, $s1
nand $t2, $s1, $s1

jal %printf

nor $t0, $s0, $s0
nor $t1, $s0, $s1
nor $t2, $s1, $s1

jal %printf

xnor $t0, $s0, $s0
xnor $t1, $s0, $s1
xnor $t2, $s1, $s1

jal %printf

not $t0, $s0
not $t1, $s1
mov $t2, 0

jal %printf

neg $t0, $s0
neg $t1, $s1
mov $t2, 0

jal %printf

halt

printf:
	prnt $t0
	prnt $t1
	prnt $t2
	jmp $ra%
