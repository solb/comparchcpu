# Sum up finite series

# Compute the sum from k=i to n of f(k)
# a0 := i (lower bound)
# a1 := n (upper bound)
# a2 := f ("lambda" function: int(int))
series_sum:
	sub	$sp, 4
	mov	%3($sp), $a0
	mov	%2($sp), $a1
	mov	%1($sp), $a2
	mov	%0($sp), $s0

	# begin test
	prnt	$sp
	prnt	%0($sp)
	prnt	%1($sp)
	prnt	%2($sp)
	prnt	%3($sp)
	prnt	%4($sp)
	prnt	$ra
	# end test

	mov	$s0, 0
	bne	%series_sum_basecase, $a0, $a1
	incr	$a0
	call	%series_sum
	add	$s0, $v
series_sum_basecase:
	mov	$a0, 3($sp)
	jal	$a2%
	add	$s0, $v

	mov	$a0, 3($sp)
	mov	$a1, 2($sp)
	mov	$a2, 1($sp)
	mov	$s0, 0($sp)
	add	$sp, 4
	ret

# Compute 2x given x
# a0 := x (number to double)
double:
	add	$v, $a0, $a0
	jmp	$ra%

main:
	# Initialize the stack pointer to the end of memory
	mov	$sp, 65535

	# Sum from k=1 to 2 of 2k
	mov	$a0, 1
	mov	$a1, 2
	mov	$a2, double
	call	%series_sum
	prnt	$v
