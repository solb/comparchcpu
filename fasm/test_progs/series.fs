# Sum up finite series

# Compute the sum from k=i to n of f(k)
# a0 := i (lower bound)
# a1 := n (upper bound)
# a2 := f ("lambda" function: int(int))
# This is a recursive function that must be 'call'ed
series_sum:
	sub	$sp, 4
	mov	%3($sp), $a0
	mov	%2($sp), $a1
	mov	%1($sp), $a2
	mov	%0($sp), $s0

	mov	$s0, 0
	beq	%series_sum_basecase, $a0, $a1
	incr	$a0
	call	%series_sum
	add	$s0, $v
	mov	$a0, %3($sp)
series_sum_basecase:
	jal	$a2%
	add	$v, $s0

	mov	$a0, %3($sp)
	mov	$a1, %2($sp)
	mov	$a2, %1($sp)
	mov	$s0, %0($sp)
	add	$sp, 4
	ret

# Compute the sum from k=i to n of f(k) tail recursively
# a0 := i (lower bound)
# a1 := n (upper bound)
# a2 := f ("lambda function: int(int))
# s0 := 0 (and thereafter, the running sum)
# This is a normal function that should be 'jal'ed
# Precondition: The stack pointer doesn't point to the value -1 upon invocation!
series_sum_tail:
	beq	%series_sum_tail_subsequentruns, %0(%sp), 65535
	sub	$sp, 4
	mov	%3($sp), $ra
	mov	%2($sp), $s0
	mov	%1($sp), $s1
	mov	%0($sp), 65535
series_sum_tail_subsequentruns:
	beq	%series_sum_tail_basecaseonly, $a0, $a1
	mov	$s1, $a0
	jal	$a2%
	add	$a0, $s1, 1
	add	$s0, $v
	jmp	%series_sum_tail
series_sum_tail_basecaseonly:
	jal	$a2%
	add	$v, $s0, $v

	mov	$ra, %3($sp)
	mov	$s0, %2($sp)
	mov	$s1, %1($sp)
	add	$sp, 4
	jmp	$ra%

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
	prnt	$v	# Should be 6

	# Same thing
	mov	$a0, 1
	mov	$a1, 2
	mov	$a2, double
	mov	$s0, 0
	jal	%series_sum_tail
	prnt	$v	# Should be the same

	# Now let's check the sp, which shouldn't have changed
	prnt	$sp	# Should still be 0xffff
