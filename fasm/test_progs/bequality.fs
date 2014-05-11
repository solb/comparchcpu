	beq %goodareeq, 0, 0
	jmp %fail

goodareeq:
	beq %fail, 0, 1
	bne %fail, 0, 0
	bne %goodarenteq, 0, 1
	jmp %fail

goodarenteq:
	halt

fail:
	mov	$t0, 1
	prnt	$t0
