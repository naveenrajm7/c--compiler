* C- Compilation to TM Code
* File: while_test.tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:    LDA  4,0(6) 	copy gp to sp &allocating global variables(if any)
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* Jump to main()
* Begin output()
  9:     LD  1,1(4) 	load param into ax
 10:    OUT  1,0,0 	output using ax
 11:    LDA  4,1(4) 	pop prepare
 12:     LD  7,-1(4) 	pop return addr
* End output()
* -> function:
 13:    LDA  4,-1(4) 	push prepare
 14:     ST  5,0(4) 	push old bp
 15:    LDA  5,0(4) 	let bp == sp
 16:    LDA  4,-1(4) 	allocate for local variables
* -> compound
* -> assign
* -> Id
 17:    LDA  2,-1(5) 	get local address
* <- Id
 18:    LDA  4,-1(4) 	push prepare
 19:     ST  2,0(4) 	protect bx
* -> Const
 20:    LDC  1,0(0) 	load const
* <- Const
 21:    LDA  4,1(4) 	pop prepare
 22:     LD  2,-1(4) 	recover bx
 23:     ST  1,0(2) 	assign: store
* <- assign
* -> while
* jump here after body
* -> Op
* -> Id
 24:    LDA  2,-1(5) 	get local address
 25:     LD  1,0(2) 	get variable value
* <- Id
 26:    LDA  4,-1(4) 	push prepare
 27:     ST  1,0(4) 	op: protect left
* -> Const
 28:    LDC  1,10(0) 	load const
* <- Const
 29:    LDA  4,1(4) 	pop prepare
 30:     LD  2,-1(4) 	op: recover left
 31:    SUB  1,2,1 	op <
 32:    JLT  1,2(7) 	br if true
 33:    LDC  1,0(0) 	false case
 34:    LDA  7,1(7) 	unconditional jmp
 35:    LDC  1,1(0) 	true case
* <- Op
* jump to end if test fails
* -> compound
* -> assign
* -> Id
 37:    LDA  2,-1(5) 	get local address
* <- Id
 38:    LDA  4,-1(4) 	push prepare
 39:     ST  2,0(4) 	protect bx
* -> Op
* -> Id
 40:    LDA  2,-1(5) 	get local address
 41:     LD  1,0(2) 	get variable value
* <- Id
 42:    LDA  4,-1(4) 	push prepare
 43:     ST  1,0(4) 	op: protect left
* -> Const
 44:    LDC  1,1(0) 	load const
* <- Const
 45:    LDA  4,1(4) 	pop prepare
 46:     LD  2,-1(4) 	op: recover left
 47:    ADD  1,2,1 	op +
* <- Op
 48:    LDA  4,1(4) 	pop prepare
 49:     LD  2,-1(4) 	recover bx
 50:     ST  1,0(2) 	assign: store
* <- assign
* -> call
* -> Id
 51:    LDA  2,-1(5) 	get local address
 52:     LD  1,0(2) 	get variable value
* <- Id
 53:    LDA  4,-1(4) 	push prepare
 54:     ST  1,0(4) 	push parameters
 55:    LDA  1,3(7) 	store returned PC
 56:    LDA  4,-1(4) 	push prepare
 57:     ST  1,0(4) 	push returned PC
 58:    LDC  7,9(0) 	jump to function
 59:    LDA  4,1(4) 	release parameters
* <- call
* <- compound
 60:    LDA  7,24(0) 	jump to test
 36:    JEQ  1,61(0) 	jump to end
* <- while
* <- compound
 61:    LDA  4,0(5) 	let sp == bp
 62:    LDA  4,2(4) 	pop prepare
 63:     LD  5,-2(4) 	pop old bp
 64:     LD  7,-1(4) 	pop return addr
* <- function
  3:    LDA  1,3(7) 	store returned PC
  4:    LDA  4,-1(4) 	push prepare
  5:     ST  1,0(4) 	push returned PC
  6:    LDC  7,13(0) 	jump to function
  7:    LDA  4,0(4) 	release local var
* End of execution.
  8:   HALT  0,0,0 	
