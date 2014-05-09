memory:
	.word 9
	.word 10

main:
	mov	$t0, 7
	prnt	$t0
	add	$t0, 6
	seq	$t2, $t0%, 8
	prnt	$t0+
	prnt	%memory
	mov	$t1, 1
	prnt	%memory($t1)
	incr	%memory($t1)
	prnt	%memory($t1)
	rsft	$t0%, 8
	and	$t0%, 15
	sub	$t0%, 2
	prnt	$t0%
