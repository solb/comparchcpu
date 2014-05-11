# A program to test the logical operations (Treated in c style)

mov $s0, 0
mov $s1, 1

land $t0, $s0, $s0
land $t1, $s0, $s1
land $t2, $s1, $s1

call %print

lor $t0, $s0, $s0
lor $t1, $s0, $s1
lor $t2, $s1, $s1

call %print

lxor $t0, $s0, $s0
lxor $t1, $s0, $s1
lxor $t2, $s1, $s1

call %print

lnand $t0, $s0, $s0
lnand $t1, $s0, $s1
lnand $t2, $s1, $s1

call %print

lnor $t0, $s0, $s0
lnor $t1, $s0, $s1
lnor $t2, $s1, $s1

call %print

lxnor $t0, $s0, $s0
lxnor $t1, $s0, $s1
lxnor $t2, $s1, $s1

call %print

lnot $t0, $s0
lnot $t1, 5000
mov $t2, 0

call %print
halt

print:
	prnt $t0
	prnt $t1
	prnt $t2
	ret
