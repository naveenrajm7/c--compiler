* C- Compilation to TM Code
* File: global_check.tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:    LDA  4,-1(6) 	copy gp to sp &allocating global variables(if any)
  2:     ST  0,0(0) 	clear location 0
* End of standard prelude.
* Jump to main()
* Begin input()
  9:     IN  1,0,0 	read input into ax
 10:    LDA  4,1(4) 	pop prepare
 11:     LD  7,-1(4) 	pop return addr
* End input()
* Begin output()
 12:     LD  1,1(4) 	load param into ax
 13:    OUT  1,0,0 	output using ax
 14:    LDA  4,1(4) 	pop prepare
 15:     LD  7,-1(4) 	pop return addr
* End output()
* -> function:
 16:    LDA  4,-1(4) 	push prepare
 17:     ST  5,0(4) 	push old bp
 18:    LDA  5,0(4) 	let bp == sp
 19:    LDA  4,-2(4) 	allocate for local variables
* -> compound
* -> assign
* -> Id
 20:    LDA  2,-1(6) 	get global address
* <- Id
 21:    LDA  4,-1(4) 	push prepare
 22:     ST  2,0(4) 	protect bx
* -> Const
 23:    LDC  1,21(0) 	load const
* <- Const
 24:    LDA  4,1(4) 	pop prepare
 25:     LD  2,-1(4) 	recover bx
 26:     ST  1,0(2) 	assign: store
* <- assign
* -> assign
* -> Id
 27:    LDA  2,-1(5) 	get local address
* <- Id
 28:    LDA  4,-1(4) 	push prepare
 29:     ST  2,0(4) 	protect bx
* -> call
 30:    LDA  1,3(7) 	store returned PC
 31:    LDA  4,-1(4) 	push prepare
 32:     ST  1,0(4) 	push returned PC
 33:    LDC  7,9(0) 	jump to function
 34:    LDA  4,0(4) 	release parameters
* <- call
 35:    LDA  4,1(4) 	pop prepare
 36:     LD  2,-1(4) 	recover bx
 37:     ST  1,0(2) 	assign: store
* <- assign
* -> assign
* -> Id
 38:    LDA  2,-2(5) 	get local address
* <- Id
 39:    LDA  4,-1(4) 	push prepare
 40:     ST  2,0(4) 	protect bx
* -> Op
* -> Id
 41:    LDA  2,-1(6) 	get global address
 42:     LD  1,0(2) 	get variable value
* <- Id
 43:    LDA  4,-1(4) 	push prepare
 44:     ST  1,0(4) 	op: protect left
* -> Id
 45:    LDA  2,-1(5) 	get local address
 46:     LD  1,0(2) 	get variable value
* <- Id
 47:    LDA  4,1(4) 	pop prepare
 48:     LD  2,-1(4) 	op: recover left
 49:    ADD  1,2,1 	op +
* <- Op
 50:    LDA  4,1(4) 	pop prepare
 51:     LD  2,-1(4) 	recover bx
 52:     ST  1,0(2) 	assign: store
* <- assign
* -> call
* -> Id
 53:    LDA  2,-2(5) 	get local address
 54:     LD  1,0(2) 	get variable value
* <- Id
 55:    LDA  4,-1(4) 	push prepare
 56:     ST  1,0(4) 	push parameters
 57:    LDA  1,3(7) 	store returned PC
 58:    LDA  4,-1(4) 	push prepare
 59:     ST  1,0(4) 	push returned PC
 60:    LDC  7,12(0) 	jump to function
 61:    LDA  4,1(4) 	release parameters
* <- call
* <- compound
 62:    LDA  4,0(5) 	let sp == bp
 63:    LDA  4,2(4) 	pop prepare
 64:     LD  5,-2(4) 	pop old bp
 65:     LD  7,-1(4) 	pop return addr
* <- function
  3:    LDA  1,3(7) 	store returned PC
  4:    LDA  4,-1(4) 	push prepare
  5:     ST  1,0(4) 	push returned PC
  6:    LDC  7,16(0) 	jump to function
  7:    LDA  4,0(4) 	release local var
* End of execution.
  8:   HALT  0,0,0 	
