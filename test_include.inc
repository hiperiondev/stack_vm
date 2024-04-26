.comment include test file
PUSH_INT 12
PUSH_INT 13
DROP
.include test_include2.inc
DROP
GOTO $HERE + 11
.label lbl_include
ADD
GOTO lbl_include
.comment END include test file