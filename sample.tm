* C- Compilation to TM Code
* File: sample.tm
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
 19:    LDA  4,0(4) 	allocate for local variables
* -> compound
* -> if
* -> Op
* -> Id
 20:    LDA  2,3(5) 	get param variable address
 21:     LD  1,0(2) 	get variable value
* <- Id
 22:    LDA  4,-1(4) 	push prepare
 23:     ST  1,0(4) 	op: protect left
* -> Const
 24:    LDC  1,0(0) 	load const
* <- Const
 25:    LDA  4,1(4) 	pop prepare
 26:     LD  2,-1(4) 	op: recover left
 27:    SUB  1,2,1 	op ==
 28:    JEQ  1,2(7) 	br if true
 29:    LDC  1,0(0) 	false case
 30:    LDA  7,1(7) 	unconditional jmp
 31:    LDC  1,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> return
* -> Id
 33:    LDA  2,2(5) 	get param variable address
 34:     LD  1,0(2) 	get variable value
* <- Id
 35:    LDA  4,0(5) 	let sp == bp
 36:    LDA  4,2(4) 	pop prepare
 37:     LD  5,-2(4) 	pop old bp
 38:     LD  7,-1(4) 	pop return addr
* <- return
* if: jump to end belongs here
 32:    JEQ  1,7(7) 	if: jmp to else
* -> return
* -> call
* -> Op
* -> Id
 40:    LDA  2,2(5) 	get param variable address
 41:     LD  1,0(2) 	get variable value
* <- Id
 42:    LDA  4,-1(4) 	push prepare
 43:     ST  1,0(4) 	op: protect left
* -> Op
* -> Op
* -> Id
 44:    LDA  2,2(5) 	get param variable address
 45:     LD  1,0(2) 	get variable value
* <- Id
 46:    LDA  4,-1(4) 	push prepare
 47:     ST  1,0(4) 	op: protect left
* -> Id
 48:    LDA  2,3(5) 	get param variable address
 49:     LD  1,0(2) 	get variable value
* <- Id
 50:    LDA  4,1(4) 	pop prepare
 51:     LD  2,-1(4) 	op: recover left
 52:    DIV  1,2,1 	op /
* <- Op
 53:    LDA  4,-1(4) 	push prepare
 54:     ST  1,0(4) 	op: protect left
* -> Id
 55:    LDA  2,3(5) 	get param variable address
 56:     LD  1,0(2) 	get variable value
* <- Id
 57:    LDA  4,1(4) 	pop prepare
 58:     LD  2,-1(4) 	op: recover left
 59:    MUL  1,2,1 	op *
* <- Op
 60:    LDA  4,1(4) 	pop prepare
 61:     LD  2,-1(4) 	op: recover left
 62:    SUB  1,2,1 	op -
* <- Op
 63:    LDA  4,-1(4) 	push prepare
 64:     ST  1,0(4) 	push parameters
* -> Id
 65:    LDA  2,3(5) 	get param variable address
 66:     LD  1,0(2) 	get variable value
* <- Id
 67:    LDA  4,-1(4) 	push prepare
 68:     ST  1,0(4) 	push parameters
 69:    LDA  1,3(7) 	store returned PC
 70:    LDA  4,-1(4) 	push prepare
 71:     ST  1,0(4) 	push returned PC
 72:    LDC  7,16(0) 	jump to function
 73:    LDA  4,2(4) 	release parameters
* <- call
 74:    LDA  4,0(5) 	let sp == bp
 75:    LDA  4,2(4) 	pop prepare
 76:     LD  5,-2(4) 	pop old bp
 77:     LD  7,-1(4) 	pop return addr
* <- return
 39:    LDA  7,38(7) 	jmp to end
* <- if
* <- compound
* <- function
* -> function:
 78:    LDA  4,-1(4) 	push prepare
 79:     ST  5,0(4) 	push old bp
 80:    LDA  5,0(4) 	let bp == sp
 81:    LDA  4,-2(4) 	allocate for local variables
* -> compound
* -> assign
* -> Id
 82:    LDA  2,-1(5) 	get local address
* <- Id
 83:    LDA  4,-1(4) 	push prepare
 84:     ST  2,0(4) 	protect bx
* -> call
 85:    LDA  1,3(7) 	store returned PC
 86:    LDA  4,-1(4) 	push prepare
 87:     ST  1,0(4) 	push returned PC
 88:    LDC  7,9(0) 	jump to function
 89:    LDA  4,0(4) 	release parameters
* <- call
 90:    LDA  4,1(4) 	pop prepare
 91:     LD  2,-1(4) 	recover bx
 92:     ST  1,0(2) 	assign: store
* <- assign
* -> assign
* -> Id
 93:    LDA  2,-2(5) 	get local address
* <- Id
 94:    LDA  4,-1(4) 	push prepare
 95:     ST  2,0(4) 	protect bx
* -> call
 96:    LDA  1,3(7) 	store returned PC
 97:    LDA  4,-1(4) 	push prepare
 98:     ST  1,0(4) 	push returned PC
 99:    LDC  7,9(0) 	jump to function
100:    LDA  4,0(4) 	release parameters
* <- call
101:    LDA  4,1(4) 	pop prepare
102:     LD  2,-1(4) 	recover bx
103:     ST  1,0(2) 	assign: store
* <- assign
* -> call
* -> call
* -> Id
104:    LDA  2,-2(5) 	get local address
105:     LD  1,0(2) 	get variable value
* <- Id
106:    LDA  4,-1(4) 	push prepare
107:     ST  1,0(4) 	push parameters
* -> Id
108:    LDA  2,-1(5) 	get local address
109:     LD  1,0(2) 	get variable value
* <- Id
110:    LDA  4,-1(4) 	push prepare
111:     ST  1,0(4) 	push parameters
112:    LDA  1,3(7) 	store returned PC
113:    LDA  4,-1(4) 	push prepare
114:     ST  1,0(4) 	push returned PC
115:    LDC  7,16(0) 	jump to function
116:    LDA  4,2(4) 	release parameters
* <- call
117:    LDA  4,-1(4) 	push prepare
118:     ST  1,0(4) 	push parameters
119:    LDA  1,3(7) 	store returned PC
120:    LDA  4,-1(4) 	push prepare
121:     ST  1,0(4) 	push returned PC
122:    LDC  7,12(0) 	jump to function
123:    LDA  4,1(4) 	release parameters
* <- call
* <- compound
124:    LDA  4,0(5) 	let sp == bp
125:    LDA  4,2(4) 	pop prepare
126:     LD  5,-2(4) 	pop old bp
127:     LD  7,-1(4) 	pop return addr
* <- function
  3:    LDA  1,3(7) 	store returned PC
  4:    LDA  4,-1(4) 	push prepare
  5:     ST  1,0(4) 	push returned PC
  6:    LDC  7,78(0) 	jump to function
  7:    LDA  4,0(4) 	release local var
* End of execution.
  8:   HALT  0,0,0 	
