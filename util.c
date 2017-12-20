/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for  C-- compiler           			    */
/* C-- Compiler Project				    */
/* ZEBRA 	                                    */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case INT:
    case ELSE:
    case VOID:
    case WHILE:
    case RETURN:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    case ASSIGN: fprintf(listing,"="); break;
    case LT: fprintf(listing,"<"); break;
    case EQ: fprintf(listing,"=="); break;
    case LPAREN: fprintf(listing,"("); break;
    case RPAREN: fprintf(listing,")"); break;
    case SEMI: fprintf(listing,";"); break;
    case PLUS: fprintf(listing,"+"); break;
    case MINUS: fprintf(listing,"-"); break;
    case TIMES: fprintf(listing,"*"); break;
    case OVER: fprintf(listing,"/"); break;
    case GT:fprintf(listing,">");break;
    case LTE:fprintf(listing,"<=");break;
    case GTE:fprintf(listing,">=");break;
    case NE:fprintf(listing,"!=");break;
    case LB:fprintf(listing,"[");break;
    case RB:fprintf(listing,"]\n");break;
    case LF:fprintf(listing,"{\n");break;
    case RF:fprintf(listing,"}\n");break;
    case COMMA:fprintf(listing,",\n");break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function newDecNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newDecNode(DecKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DecK;
    t->kind.dec = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
    int i;

    INDENT;

    while (tree != NULL)  /* try not to visit null tree children */
    {
        printSpaces();

        /* Examine node type, and base output on that. */
        if (tree->nodekind == DecK)
        {
            switch(tree->kind.dec)
            {
            case VarK:
                fprintf(listing,"[Variable declaration \"%s\" of type \"%s\"]\n"
			, tree->attr.name, tree->variableDataType==Integer?"Integer":"Void");
                break;
            case ArrayK:
                fprintf(listing, "[Array declaration \"%s\" of size %d"
                        " and type \"%s\"]\n",
                      tree->attr.name, tree->value, tree->variableDataType==INT?"Integer":"Void");
                break;
            case FunK:
                fprintf(listing, "[Function declaration \"%s()\""
                        " of return type \"%s\"]\n", 
                        tree->attr.name, tree->variableDataType==INT?"Integer":"Void");
                break;
            default:
                fprintf(listing, "<<<unknown declaration type>>>\n");
		break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch(tree->kind.exp)
            {
            case OpK:
                fprintf(listing, "[Operator \"");
                printToken(tree->attr.op, "");
                fprintf(listing, "\"]\n");
                break;
            case IdK:
                fprintf(listing, "[Identifier \"%s", tree->attr.name);
                if (tree->value != 0) /* array indexing */
                    fprintf(listing, "[%d]", tree->value);
                fprintf(listing, "\"]\n");
                break;
            case ConstK:
                fprintf(listing, "[Literal constant \"%d\"]\n", tree->attr.val);
                break;
            case AssignK:
                fprintf(listing, "[Assignment]\n");
                break;
            default:
                fprintf(listing, "<<<unknown expression type>>>\n");
                break;
            }
        }
	else if (tree->nodekind == StmtK)
	{
	    switch(tree->kind.stmt)
	    {
		case CompoundK:
		    fprintf(listing, "[Compound statement]\n");
		    break;
                case IfK:
                    fprintf(listing, "[IF statement]\n");
                    break;
                case WhileK:
                    fprintf(listing, "[WHILE statement]\n");
                    break;
                case ReturnK:
                    fprintf(listing, "[RETURN statement]\n");
                    break;
                case CallK:
                    fprintf(listing, "[Call to function \"%s()\"]\n",
                             tree->attr.name);
                    break;
		default:
		    fprintf(listing, "<<<unknown statement type>>>\n");
		    break;
	    }
	}
        else
            fprintf(listing, "<<<unknown node kind>>>\n");

        for (i=0; i<MAXCHILDREN; ++i)
            printTree(tree->child[i]);

        tree = tree->sibling;
    }

    UNINDENT;
}

