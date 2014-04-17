# A program that totals the contents of an array, stowing its result in s1.

# An array of 10 words, each initialized to 0.
arr:
.word 0 10

# The main label. Execution begins from here.
main:
	mov		$s0, 10
	mov		$s1, 0

# Loop through the elements of the array, calling fun on each.
loop:
	decr	$s0
	mov		$a0, $s0		# Tell fun which array index to use.
	jal		%fun
	add		$s1, $v			# Total the results.
	biz		$s0, %loop
	halt

fun:
	mov		$v, %arr($a0)
	jmp		$ra
