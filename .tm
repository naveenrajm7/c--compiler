* C- Compilation to TM Code
* File: .tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:    LDA  4,0(6) 	copy gp to sp
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
 20:    LDA  2,-1(5) 	get local address
* <- Id
 21:    LDA  4,-1(4) 	push prepare
 22:     ST  2,0(4) 	protect bx
* -> call
 23:    LDA  1,3(7) 	store returned PC
 24:    LDA  4,-1(4) 	push prepare
 25:     ST  1,0(4) 	push returned PC
 26:    LDC  7,9(0) 	jump to function
 27:    LDA  4,0(4) 	release parameters
* <- call
 28:    LDA  4,1(4) 	pop prepare
 29:     LD  2,-1(4) 	recover bx
 30:     ST  1,0(2) 	assign: store
* <- assign
* -> assign
* -> Id
 31:    LDA  2,-2(5) 	get local address
* <- Id
 32:    LDA  4,-1(4) 	push prepare
 33:     ST  2,0(4) 	protect bx
* -> Op
* -> Id
 34:    LDA  2,-3(5) 	get local address
 35:     LD  1,0(2) 	get variable value
* <- Id
 36:    LDA  4,-1(4) 	push prepare
 37:     ST  1,0(4) 	op: protect left
* -> Id
 38:    LDA  2,-1(5) 	get local address
 39:     LD  1,0(2) 	get variable value
* <- Id
 40:    LDA  4,1(4) 	pop prepare
 41:     LD  2,-1(4) 	op: recover left
 42:    ADD  1,2,1 	op +
* <- Op
 43:    LDA  4,1(4) 	pop prepare
 44:     LD  2,-1(4) 	recover bx
 45:     ST  1,0(2) 	assign: store
* <- assign
* -> call
* -> Id
 46:    LDA  2,-2(5) 	get local address
 47:     LD  1,0(2) 	get variable value
* <- Id
 48:    LDA  4,-1(4) 	push prepare
 49:     ST  1,0(4) 	push parameters
 50:    LDA  1,3(7) 	store returned PC
 51:    LDA  4,-1(4) 	push prepare
 52:     ST  1,0(4) 	push returned PC
 53:    LDC  7,12(0) 	jump to function
 54:    LDA  4,1(4) 	release parameters
* <- call
* <- compound
 55:    LDA  4,0(5) 	let sp == bp
 56:    LDA  4,2(4) 	pop prepare
 57:     LD  5,-2(4) 	pop old bp
 58:     LD  7,-1(4) 	pop return addr
* <- function
  3:    LDA  1,3(7) 	store returned PC
  4:    LDA  4,-1(4) 	push prepare
  5:     ST  1,0(4) 	push returned PC
  6:    LDC  7,16(0) 	jump to function
  7:    LDA  4,0(4) 	release local var
* End of execution.
  8:   HALT  0,0,0 	
