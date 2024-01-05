f:
# entry = .B1
# exits = .B1
# frameobj:%2 = { size:4, align:4 }
# frameobj:%4 = { size:4, align:4 }
# frameobj:%5 = { size:4, align:4 }
.B1:
# preds =
# succs =
	mv	def %1, kill a0
	mv	def %3, kill a1
	sw	kill %1, {frameobj:%2}
	sw	kill %3, {frameobj:%4}
	lw	def %6, {frameobj:%2}
	lw	def %7, {frameobj:%4}
	add	def %8, %6, %7
	sw	kill %8, {frameobj:%5}
	lw	def %9, {frameobj:%2}
	lw	def %10, {frameobj:%4}
	sub	def %11, %9, %10
	mv	kill a0, def %11
	ret

g:
# entry = .B1
# exits = .B1
.B1:
# preds =
# succs =
	ret

main:
# entry = .B1
# exits = .B1
# frameobj:%2 = { size:4, align:4 }
.B1:
# preds =
# succs =
	mv	def %1, kill a0
	sw	kill %1, {frameobj:%2}
	lw	def %3, {frameobj:%2}
	li	def %4, 1
	sub	def %5, %3, %4
	mv	kill a0, def %5
	ret
