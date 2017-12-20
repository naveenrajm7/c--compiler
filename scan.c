/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the C-- compiler  */
/* C-- project    				    */
/* ZEBRA Inc.	                                    */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
   { START,INEQ,INCOMMENT1,INCOMMENT2,INGTE,INLTE,INNE,IND,INNUM,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"int",INT},{"else",ELSE},{"return",RETURN},
      {"while",WHILE},{"void",VOID}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{  
   int inCom=0;  /* inCom=0 single line comment inCom=1 multi-line comment */
   /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == '=')
           state = INEQ;
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE;
         else if (c == '>')
           state = INGTE;
	 else if (c == '<')
           state = INLTE;
	 else if (c == '!')
           state = INNE;
	 else if (c == '/')
	 {
	   save = FALSE;
           state = IND;
	 }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               currentToken = ENDFILE;
               break;
             case ',':
               currentToken = COMMA;
               break;
             case '{':
               currentToken = LF;
               break;
	     case '}':
               currentToken = RF;
               break;
             case '+':
               currentToken = PLUS;
               break;
             case '-':
               currentToken = MINUS;
               break;
             case '*':
               currentToken = TIMES;
               break;
             case '[':
               currentToken = LB;
               break;
	     case ']':
               currentToken = RB;
               break;
             case '(':
               currentToken = LPAREN;
               break;
             case ')':
               currentToken = RPAREN;
               break;
             case ';':
               currentToken = SEMI;
               break;
             default:
               currentToken = ERROR;
               break;
           }
         }
         break;
       case IND:
     
         if ((c == '*'))
         { state = INCOMMENT1;
           inCom=1;
           save=FALSE;
         }
         else if ((c == '/'))
         { state = INCOMMENT1;
           inCom=0;
           save=FALSE;
         }
         else
	 { state = DONE;
	   ungetNextChar();
	   tokenString[tokenStringIndex++]=(char)'/';
	   currentToken=OVER;
	 }
         break;
       case INNE:
         state = DONE;
         if (c == '=')
           currentToken = NE;
         else
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           currentToken = ERROR;
         }
         break;
       case INGTE:
	 state=DONE;
	 if(c=='=')
	   currentToken = GTE;
	 else
	 {
	    ungetNextChar();
	    currentToken = GT;
	 }
	 break;
       case INLTE:
	 state=DONE;
	 if(c=='=')
	   currentToken = LTE;
	 else
	 {
	    ungetNextChar();
	    currentToken = LT;
	 }
	 break;
       case INEQ:
	 state=DONE;
	 if(c=='=')
	   currentToken = EQ;
	 else
	 {
	    ungetNextChar();
	    currentToken = ASSIGN;
	 }
	 break;
       case INCOMMENT1:
	 save=FALSE;
	 if(c=='*')
	   state=INCOMMENT2;
	 else if(c=='\n' && !inCom)
	   state=START;  
	 break;
       case INCOMMENT2:
	 save=FALSE;
	 if(c=='/'&& inCom)
	   state=START;
	 else if(c=='*')
	   state=INCOMMENT2;
	 else
	   state=INCOMMENT1;	
	 break; 	
       case INNUM:
         if (!isdigit(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
         }
         break;
       case INID:
         if (!isalpha(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} /* end getToken */

