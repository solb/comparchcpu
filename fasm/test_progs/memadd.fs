onevar:
.word 5

anothervar:
.word 6

destvar:
.word 0

main:
	add %destvar, %onevar, %anothervar
	add %destvar, 0
