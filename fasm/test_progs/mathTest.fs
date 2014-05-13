#Tests add, sub, lsft, rsft, rasft, incr and decr

mov $s0, 0
mov $s1, 1

add $t0, $s0, $s0
add $t1, $s0, $s1
add $t2, $s1, $s1

jal %printf

sub $t0, $s0, $s0
sub $t1, $s0, $s1
sub $t2, $s1, $s1

jal %printf

lsft $t0, $s0, $s0
lsft $t1, $s1, $s0
lsft $t2, $s1, $s1

jal %printf

rsft $t0, $s0, $s0
rsft $t1, $s1, $s0
rsft $t2, $s1, $s1

jal %printf

mov $s2, 0
sub $s2, 2
rasft $t0, $s0, $s0
rasft $t1, $s1, $s1
rasft $t2, $s2, $s1

jal %printf

incr $s0
decr $s1
mov $t0, $s0
mov $t1, $s1
mov $t2, 0

jal %printf
halt

printf:
	prnt $t0
	prnt $t1
	prnt $t2
	jmp $ra%

