#floating words for auto incr and auto decr
.word 0
.word 12

addr2:
	.word 0
address:
	.word addr2
	.word 0

main:

	mov $s0, 0
	mov $s1, 1

#Test validator 1 short word

	mov $t0, $s0
	xor $t0, $s0
	prnt $t0
	xor $t0, $s1
	prnt $t0
	xor $t0, $s1
	prnt $t0

#Test validator 4 short word

	mov $t0, $s0
	lnot $t0
	prnt $t0
	lnot $t0
	prnt $t0

#Test writing to a 2xDisplacement from ImmediateValue

	mov %address($t0)($t0), 42
	prnt %address($t0)($t0)

#Test writing to Displacement from 2xDisplacement
	
	mov %address($s1), %address($t0)($t0)
	prnt %address($s1)

#Test writing to autodecrement from Displacement
	mov $t0, 0	
	mov $t0-, %address($s1)
	prnt $t0
	incr $t0
	prnt $t0%

#Test writing to autoincrement from autodecrement
	mov $t0, 0	
	add $t0+, $t0-	
	prnt $t0%	
	
#Test writing to 2xScaled from autoincrement
	mov $s2, address
	mov $s2($t0)($t0), $t0+
	decr $t0
	prnt $s2($t0)($t0)

#Test writing to Scaled from 2xScaled
	mov $s2($s1), $s2($t0)($t0)
	prnt $s2($s1)

#Test writing to RegisterIndirect from Scaled
	add $t0, address, 1
	mov $t0%, $s2($s1)
	prnt $t0%

#Test writing to Register from Register Indirect
	mov $t1, $t0%
	prnt $t1

#Test writing to PC Relative from Register
	mov %addr2, 50
	mov %addr2($s1), 60
	mov %-126@, $t1
	prnt %-128@

#Test writing to Immediate Address from PC Relative
	mov %addr2, %-129@

#Test reading an immediate address
	prnt %addr2

	halt
