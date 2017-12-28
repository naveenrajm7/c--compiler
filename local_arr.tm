* C- Compilation to TM Code
* File: local_arr.tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:    LDA  4,0(6) 	copy gp to sp &allocating global variables(if any)
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
 19:    LDA  4,-21(4) 	allocate for local variables
* -> compound
* -> array element
 20:    LDA  2,0(5) 	get local array address
 21:    LDA  4,-1(4) 	push prepare
 22:     ST  2,0(4) 	protect array address
 23:    LDA  4,1(4) 	pop prepare
 24:     LD  2,-1(4) 	recover array address
 25:    SUB  2,2,1 	get address of array element
 26:     LD  1,0(2) 	get value of array element
* <- array element
* -> assign
* -> array element
 27:    LDA  2,0(5) 	get local array address
 28:    LDA  4,-1(4) 	push prepare
 29:     ST  2,0(4) 	protect array address
* -> Const
 30:    LDC  1,2(0) 	load const
* <- Const
 31:    LDA  4,1(4) 	pop prepare
 32:     LD  2,-1(4) 	recover array address
 33:    SUB  2,2,1 	get address of array element
* <- array element
 34:    LDA  4,-1(4) 	push prepare
 35:     ST  2,0(4) 	protect bx
* -> call
 36:    LDA  1,3(7) 	store returned PC
 37:    LDA  4,-1(4) 	push prepare
 38:     ST  1,0(4) 	push returned PC
 39:    LDC  7,9(0) 	jump to function
 40:    LDA  4,0(4) 	release parameters
* <- call
 41:    LDA  4,1(4) 	pop prepare
 42:     LD  2,-1(4) 	recover bx
 43:     ST  1,0(2) 	assign: store
* <- assign
* -> call
* -> array element
 44:    LDA  2,0(5) 	get local array address
 45:    LDA  4,-1(4) 	push prepare
 46:     ST  2,0(4) 	protect array address
* -> Const
 47:    LDC  1,2(0) 	load const
* <- Const
 48:    LDA  4,1(4) 	pop prepare
 49:     LD  2,-1(4) 	recover array address
 50:    SUB  2,2,1 	get address of array element
 51:     LD  1,0(2) 	get value of array element
* <- array element
 52:    LDA  4,-1(4) 	push prepare
 53:     ST  1,0(4) 	push parameters
 54:    LDA  1,3(7) 	store returned PC
 55:    LDA  4,-1(4) 	push prepare
 56:     ST  1,0(4) 	push returned PC
 57:    LDC  7,12(0) 	jump to function
 58:    LDA  4,1(4) 	release parameters
* <- call
* <- compound
 59:    LDA  4,0(5) 	let sp == bp
 60:    LDA  4,2(4) 	pop prepare
 61:     LD  5,-2(4) 	pop old bp
 62:     LD  7,-1(4) 	pop return addr
* <- function
  3:    LDA  1,3(7) 	store returned PC
  4:    LDA  4,-1(4) 	push prepare
  5:     ST  1,0(4) 	push returned PC
  6:    LDC  7,16(0) 	jump to function
  7:    LDA  4,0(4) 	release local var
* End of execution.
  8:   HALT  0,0,0 	
