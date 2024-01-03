f:
# entry = .B1
# exits = .B1
# frameobj:%2 = { size:4, align:4 }
# frameobj:%4 = { size:4, align:4 }
# frameobj:%5 = { size:4, align:4 }
.B1:
	sw	%1, {frameobj:%2}
	sw	%3, {frameobj:%4}
	lw	%6, {frameobj:%2}
	lw	%7, {frameobj:%4}
	add	%8, %6, %7
	sw	%8, {frameobj:%5}
	lw	%9, {frameobj:%2}
	lw	%10, {frameobj:%4}
	sub	%11, %9, %10
	mv	a0, %11
	ret

g:
# entry = .B1
# exits = .B1
.B1:
	ret

main:
# entry = .B1
# exits = .B1
# frameobj:%2 = { size:4, align:4 }
.B1:
	sw	%1, {frameobj:%2}
	lw	%3, {frameobj:%2}
	li	%4, 1
	sub	%5, %3, %4
	mv	a0, %5
	ret
