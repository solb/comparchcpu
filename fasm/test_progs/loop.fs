	mov		$s0, 10
	mov		$s1, 0

loop:
	mov		$a0, $s0
	jal		%fun
	add		$s1, $v
	decr	$s0
	bnz		%loop, $s0
    add     $s1, 0	
    halt

fun:
	mov		$v, $a0
	jmp		$ra%
