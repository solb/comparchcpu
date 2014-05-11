isZero:
	.word 0
notZero:
	.word 1

error:
	prnt %notZero
	halt

main:
	blt %error, notZero, isZero
	bgt %error, isZero, notZero
	beq %error, isZero, notZero
	bne %error, isZero, isZero
	ble %error, notZero, isZero
	bge %error, isZero, notZero
	biz %error, notZero
	bnz %error, isZero
	prnt %isZero
	halt
