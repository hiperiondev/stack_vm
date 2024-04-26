.comment include test file 2
PUSH_INT 999
PUSH_INT 888
DROP
DROP
GOTO $HERE + 3
.label lbl_include2
ADD
GOTO lbl_include2
.comment END include test file 2