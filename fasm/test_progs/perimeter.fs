#A program that gets the perimeter of a rectangle
#initialize a box and put it in an array

box1:
	.word   5
    .word   4
	.word   0

box2:
	.word   2
	.word   3
	.word   0

box3:
	.word   10 2
	.word   0

arrayOfBox:
	.word   0
	.word   3
	.word	6
	.word   -1

main:
	mov $t1, 9
    mov $s0, 0
	mov $t4, 0
loop:
	mov $t0, 0
    lsft $t2, $t1($s0)($t0), 1
    incr $t0
    lsft $t3, %arrayOfBox($s0)($t0), 1
    incr $t0
    add %arrayOfBox($s0)($t0), $t2, $t3
	incr $s0
	blt %loop, $t4, %arrayOfBox($s0)
    halt
