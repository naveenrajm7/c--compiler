/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for C- compiler            */
/* must come before other include files             */
/* C- Compiler Project				    */
/* ZEBRA 	                                    */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6

typedef enum 
    /* book-keeping tokens */
   {ENDFILE,ERROR,
    /* reserved words */
    IF,ELSE,INT,RETURN,VOID,WHILE,
    /* multicharacter tokens */
    ID,NUM,
    /* special symbols */
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI,GT,LTE,GTE,NE,LB,RB,LF,RF,COMMA
   } TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK,DecK} NodeKind;
typedef enum {IfK,WhileK,CallK,ReturnK,CompoundK} StmtKind;
typedef enum {OpK,ConstK,IdK,AssignK} ExpKind;
typedef enum {VarK,ArrayK,FunK} DecKind;
/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp;DecKind dec;} kind;
     union { TokenType op;
             int val;
             char * name; } attr;
             
     int value; /* array index */
     
     int isArray;       
     /*
     * If isParameter is TRUE, then this node declares an actual parameter
     *   to a function.
     */
    int isParameter;

    /* If isGlobal is TRUE, then the variable is a global */
    int isGlobal;
    
    /* parameter count and local variable count*/
    int param_size;
    
    int local_size;
    
    /* scope of the node */
    int scope;
     ExpType type; /* for type checking of exps */
   } TreeNode;


#define MAX_SCOPE 3
/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 
#endif
