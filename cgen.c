/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

/* to store local var of main 
*/
static int main_locals = 0;

/* getValue:
 * 1 - store value in ax
 * 0 - store address in bx
 */
static int getValue=1;

/* prototype for internal recursive code generator */
static void cGen (TreeNode * tree);


static int tmp;

/* isRecursive:
 * 1 - cGen will recurse on sibling
 * 0 - cGen won't recurse on sibling
 */
static int isRecursive = 1;


/* stack used for call */
TreeNode* paramStack[10];
int top = 0;

/* stack routines*/
int pushParam(TreeNode* param)
{
    if(top == SIZE) 
      return 1;

    paramStack[top++] = param;
    return 0;
}

TreeNode* popParam()
{
  if(top == 0)
    return NULL;

  return paramStack[--top];
}  

/* emit one instruction to get the address of a var,
 * store the address in bx,
 * we can access the var by bx[0]
 */
void emitGetAddr(TreeNode *var)
{

  switch(var->scope){
    case 0:
        if(var->isArray){
          emitRM("LDA",bx,-(st_lookup(var->attr.name,0)),gp,"get global array address");
        }
        else{
          emitRM("LDA",bx,-1-(st_lookup(var->attr.name,0)),gp,"get global address");
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if(var_lookup(var->attr.name,var->scope)->isParam)
        {
        	 if(var->isArray){
          		emitRM("LD",bx,2+(st_lookup(var->attr.name,var->scope)),bp,"get param array address");
       		 }
        	else
        	{
          		emitRM("LDA",bx,2+(st_lookup(var->attr.name,var->scope)),bp,"get param variable address");
        	}
        }
        else
        {
        	if(var->isArray ){
         		 emitRM("LDA",bx,-(st_lookup(var->attr.name,var->scope)),bp,"get local array address");
        	}
        	else{
          		emitRM("LDA",bx,-1-(st_lookup(var->attr.name,var->scope)),bp,"get local address");
        	}
        }
        break;

  }
}


/* Procedure genDec generates code at an declaration node */
static void genDec( TreeNode * tree)
{ int loc;
  BucketList fun;
  TreeNode * p1, * p2;
  switch (tree->kind.exp) {

    case FunK:
    	      if(!strcmp(tree->attr.name,"main"))
    	      		main_locals=tree->child[1]->local_size;
              if (TraceCode) emitComment("-> function:");

              p1 = tree->child[0];/*parameter*/
              p2 = tree->child[1];/*body*/

              fun= fun_lookup(tree->attr.name,0);
              fun->fun_start = emitSkip(0);

              
              /*prepare bp & sp*/
              emitRM("LDA",sp,-1,sp,"push prepare");
              emitRM("ST",bp,0,sp,"push old bp");
              emitRM("LDA",bp,0,sp,"let bp == sp");
              emitRM("LDA",sp,p2->child[0]!=NULL?-((p2->child[0])->local_size):0,sp,"allocate for local variables");

              /*push param symtab, prepare for body*/
              
             // pushTable(fun->symbolTable);
              /*generate body*/
              cGen(p2); 
            //  popTable();

              /*generate return code for void functions*/
              if(tree->type == Void){
                  /*return*/
                  emitRM("LDA",sp,0,bp,"let sp == bp");
                  emitRM("LDA",sp,2,sp,"pop prepare");
                  emitRM("LD",bp,-2,sp,"pop old bp");
                  emitRM("LD",pc,-1,sp,"pop return addr");
              }

              if (TraceCode) emitComment("<- function");
              
              break;
    case VarK :
           /* if(TraceCode) emitComment("-> variable");
  	     emitGetAddr(tree);

             if(getValue){
              if(tree->isArray)
               emitRM("LDA",ax,0,bx,"get array variable value( == address)");
             else
                emitRM("LD",ax,0,bx,"get variable value");

             }
             if(TraceCode) emitComment("<- variable");
            */ 	
      break;
    case ArrayK :
    
       if(tree->isGlobal){
             if(TraceCode) emitComment("-> array element");
             p1 = tree->child[0];/*index expression*/

           //  var = lookup_var(tree->attr.name);
             emitGetAddr(tree);

             /* protect bx*/
             emitRM("LDA",sp,-1,sp,"push prepare");
             emitRM("ST",bx,0,sp,"protect array address");

             tmp = getValue;
             getValue = 1;
             cGen(p1);
             getValue = tmp;

             /* recover bx*/
             emitRM("LDA",sp,1,sp,"pop prepare");
             emitRM("LD",bx,-1,sp,"recover array address");

             emitRO("SUB",bx,bx,ax,"get address of array element");
             if(getValue)
                emitRM("LD",ax,0,bx,"get value of array element");

             if(TraceCode) emitComment("<- array element");
 	}
            break; /* ArrayK */

     default:
               emitComment("BUG: Unknown declaration");
               break;

  }
} /* genDec */




/* Procedure genStmt generates code at a statement node */
static void genStmt( TreeNode * tree)
{ TreeNode * p1, * p2, * p3;
  BucketList fun;
  int savedLoc1,savedLoc2,currentLoc;
  //int loc;
  switch (tree->kind.stmt) {

      case IfK :
         if (TraceCode) emitComment("-> if") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         p3 = tree->child[2] ;
         /* generate code for test expression */
         cGen(p1);
         savedLoc1 = emitSkip(1) ;
         emitComment("if: jump to else belongs here");
         /* recurse on then part */
         cGen(p2);
         savedLoc2 = emitSkip(1) ;
         emitComment("if: jump to end belongs here");
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc1) ;
         emitRM_Abs("JEQ",ax,currentLoc,"if: jmp to else");
         emitRestore() ;
         /* recurse on else part */
         cGen(p3);
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc2) ;
         emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
         emitRestore() ;
         if (TraceCode)  emitComment("<- if") ;
         break; /* if_k */

      case WhileK:
             if (TraceCode) emitComment("-> while") ;
             p1 = tree->child[0] ;
             p2 = tree->child[1] ;
             savedLoc1 = emitSkip(0);
             emitComment("jump here after body");
             /* generate code for test */
             cGen(p1);
             savedLoc2 = emitSkip(1);
             emitComment("jump to end if test fails");
             /* generate code for body */
             cGen(p2);
             emitRM("LDA",pc,savedLoc1,zero,"jump to test");
             currentLoc = emitSkip(0);
             emitBackup(savedLoc2);
             emitRM("JEQ",ax,currentLoc,zero,"jump to end");
             emitRestore();
             if (TraceCode)  emitComment("<- while") ;
             break; 

      case ReturnK:
            if (TraceCode) emitComment("-> return");
            p1 = tree->child[0];
            /*Only calculate non-voild value*/
            if(p1!=NULL) 
              cGen(p1);

            /*return*/
            emitRM("LDA",sp,0,bp,"let sp == bp");
            emitRM("LDA",sp,2,sp,"pop prepare");
            emitRM("LD",bp,-2,sp,"pop old bp");
            emitRM("LD",pc,-1,sp,"pop return addr");

            if (TraceCode) emitComment("<- return");
            break;

      case CallK:
             if (TraceCode) emitComment("-> call") ;
             p1 = tree->child[0];/*arguments*/

             while(p1 != NULL){
                pushParam(p1);
               p1 = p1->sibling;
             }

            /* first - push parameters */
             isRecursive = 0;
            while( (p1 = popParam()) != NULL){
                cGen(p1);
                emitRM("LDA",sp,-1,sp,"push prepare");
                emitRM("ST",ax,0,sp,"push parameters");
            }
             isRecursive = 1;

             /*second - call function*/
             fun = fun_lookup(tree->attr.name,0);
            /* emitCall(fun); */
  	emitRM("LDA",ax,3,pc,"store returned PC");
  	emitRM("LDA",sp,-1,sp,"push prepare");
  	emitRM("ST",ax,0,sp,"push returned PC");
  	emitRM("LDC",pc,fun->fun_start,0,"jump to function");
  	emitRM("LDA",sp,(tree->child[0])!=NULL?(tree->child[0])->param_size:0,sp,"release parameters");  
  //	printf("%d %s\n",(tree->child[0])!=NULL?(tree->child[0])->param_size:0,tree->attr.name);                       

             if (TraceCode)  emitComment("<- call") ;
             break; 
             
      case CompoundK:
            if (TraceCode) emitComment("-> compound");
              p1 = tree->child[0];/*local declarations*/
              p2=tree->child[1]; /*statements */
 		
 	      if(p1!=NULL)	
              cGen(p1);
              if(p2!=NULL)	
              cGen(p2);
 
              if (TraceCode) emitComment("<- compound");
              break;
      default:
         break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp( TreeNode * tree)
{ //int loc;
  TreeNode * p1, * p2;
  switch (tree->kind.exp) {

    case AssignK:
             if (TraceCode) emitComment("-> assign") ;
             p1 = tree->child[0];/*left*/
             p2 = tree->child[1];/*right*/
             /* left value (get its address -> bx)*/
             getValue = 0;
             cGen(p1);
             /* protect bx*/
             emitRM("LDA",sp,-1,sp,"push prepare");
             emitRM("ST",bx,0,sp,"protect bx");
             /* right value -> ax*/
             getValue = 1;
             cGen(p2);
             /* recover bx*/
             emitRM("LDA",sp,1,sp,"pop prepare");
             emitRM("LD",bx,-1,sp,"recover bx");
             /* now we can assign*/
             emitRM("ST",ax,0,bx,"assign: store");
             if (TraceCode)  emitComment("<- assign") ;
             break; 

    case ConstK :
      if (TraceCode) emitComment("-> Const") ;
      /* gen code to load integer constant using LDC */
      emitRM("LDC",ax,tree->attr.val,0,"load const");
      if (TraceCode)  emitComment("<- Const") ;
      break; /* ConstK */
    
    case IdK :
      if (TraceCode) emitComment("-> Id") ;
      //loc = st_lookup(tree->attr.name,tree->scope);
      emitGetAddr(tree);
      
      if(getValue)
      {
        if(tree->isArray)
      	emitRM("LDA",bx,0,bx,"get array variable value");
      	else
        emitRM("LD",ax,0,bx,"get variable value");
      }  
      if (TraceCode)  emitComment("<- Id") ;
      break; /* IdK */

    case OpK :
         if (TraceCode) emitComment("-> Op") ;
         p1 = tree->child[0];
         p2 = tree->child[1];
         /* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("LDA",sp,-1,sp,"push prepare");
         emitRM("ST",ax,0,sp,"op: protect left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
	 emitRM("LDA",sp,1,sp,"pop prepare");
         emitRM("LD",bx,-1,sp,"op: recover left");
         
         switch (tree->attr.op) {
            case PLUS :
               emitRO("ADD",ax,bx,ax,"op +");
               break;
            case MINUS :
               emitRO("SUB",ax,bx,ax,"op -");
               break;
            case TIMES :
               emitRO("MUL",ax,bx,ax,"op *");
               break;
            case OVER :
               emitRO("DIV",ax,bx,ax,"op /");
               break;
            case LT :
               emitRO("SUB",ax,bx,ax,"op <") ;
               emitRM("JLT",ax,2,pc,"br if true") ;
               emitRM("LDC",ax,0,0,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ax,1,0,"true case") ;
               break;
            case EQ :
               emitRO("SUB",ax,bx,ax,"op ==") ;
               emitRM("JEQ",ax,2,pc,"br if true");
               emitRM("LDC",ax,0,0,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ax,1,0,"true case") ;
               break;
             case LTE :
               emitRO("SUB",ax,bx,ax,"op <=") ;
               emitRM("JLE",ax,2,pc,"br if true") ;
               emitRM("LDC",ax,0,0,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ax,1,0,"true case") ;
               break;
             case GT :
                   emitRO("SUB",ax,bx,ax,"op >") ;
                   emitRM("JGT",ax,2,pc,"br if true") ;
                   emitRM("LDC",ax,0,0,"false case") ;
                   emitRM("LDA",pc,1,pc,"unconditional jmp") ;
                   emitRM("LDC",ax,1,0,"true case") ;
                   break;
             case GTE :
                   emitRO("SUB",ax,bx,ax,"op >=") ;
                   emitRM("JGE",ax,2,pc,"br if true") ;
                   emitRM("LDC",ax,0,0,"false case") ;
                   emitRM("LDA",pc,1,pc,"unconditional jmp") ;
                   emitRM("LDC",ax,1,0,"true case") ;
                   break;
             case NE :
                   emitRO("SUB",ax,bx,ax,"op !=") ;
                   emitRM("JNE",ax,2,pc,"br if true") ;
                   emitRM("LDC",ax,0,0,"false case") ;
                   emitRM("LDA",pc,1,pc,"unconditional jmp") ;
                   emitRM("LDC",ax,1,0,"true case") ;
                   break;           
            default:
               emitComment("BUG: Unknown operator");
               break;
         } /* case op */
         if (TraceCode)  emitComment("<- Op") ;
         break; /* OpK */

    default:
      break;
  }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen( TreeNode * tree)
{ if (tree != NULL)
  { switch (tree->nodekind) {
      case StmtK:
        genStmt(tree);
        break;
      case ExpK:
        genExp(tree);
        break;
      case DecK:
        genDec(tree);  
      default:
        break;
    }
    if(isRecursive)
    cGen(tree->sibling);
  }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = malloc(strlen(codefile)+7);
   BucketList fun;  /*function bucket */


   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment("C- Compilation to TM Code");
   emitComment(s);
   /* generate standard prelude */
   emitComment("Standard prelude:");
   emitRM("LD",gp,0,zero,"load maxaddress from location 0");
   emitRM("LDA",sp,syntaxTree->kind.dec!=FunK?-(syntaxTree->param_size):0,gp,"copy gp to sp &allocating global variables(if any)");
   emitRM("ST",zero,0,zero,"clear location 0");
   emitComment("End of standard prelude.");
   
   /*jump to main */
      if (TraceCode) emitComment("Jump to main()");
   int loc = emitSkip(6); /*A call consumes 5 instructions, and we need halt after main()*/

   /*defining Input & output fuction as if they were in-built(global) */
   /* if only necessary  i,e. if they are used in program */ 
   fun = fun_lookup("input",0);
   if(fun!=NULL){
   if (TraceCode) emitComment("Begin input()");
   fun->fun_start = emitSkip(0);
   emitRO("IN",ax,0,0,"read input into ax");
   emitRM("LDA",sp,1,sp,"pop prepare");
   emitRM("LD",pc,-1,sp,"pop return addr");
   if (TraceCode) emitComment("End input()");
   }
   
   fun = fun_lookup("output",0);
   if(fun!=NULL){
   if (TraceCode) emitComment("Begin output()");
   fun->fun_start = emitSkip(0);
   emitRM("LD",ax,1,sp,"load param into ax");
   emitRO("OUT",ax,0,0,"output using ax");
   emitRM("LDA",sp,1,sp,"pop prepare");
   emitRM("LD",pc,-1,sp,"pop return addr");
   if (TraceCode) emitComment("End output()");
   }
   
   
   
   /* generate code for TINY program */
   cGen(syntaxTree);
   /* finish */
   
    /* Fill up jump-to-main code */
   emitBackup(loc);
   fun = fun_lookup("main",0);
   if(fun==NULL)
   {
   	fprintf(stderr,"main not found\n");
   }
   
  emitRM("LDA",ax,3,pc,"store returned PC");
  emitRM("LDA",sp,-1,sp,"push prepare");
  emitRM("ST",ax,0,sp,"push returned PC");
  emitRM("LDC",pc,fun->fun_start,0,"jump to function");
  emitRM("LDA",sp,main_locals,sp,"release local var");
   
   
   emitComment("End of execution.");
   emitRO("HALT",0,0,0,"");
}
