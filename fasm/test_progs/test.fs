# clear register t0
	xor		$t0, $t0
	mov		$t1, $t0
	incr	$t1
	mov		$t2, $t1
	incr	$t2
	add		$t0, $t1, $t2
	sub		$t0, $t2
	decr	$t0
