/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"


static int scope_a=0;
/* counter for variable memory locations */
static int location[MAX_SCOPE] = {0,0,0};


/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      {
        case CompoundK:
        	scope_a=scope_a+1;
        	break;
        case CallK:
           // printf("%d %s\n",t->param_size,t->attr.name);
           t->scope=0;
          /* all functios are global so searching in global scope */
          if (st_lookup(t->attr.name,0) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,-1,0,0);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,-1,0,0);
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          t->scope=t->isParameter==1?scope_a+1:scope_a;
          if (st_lookup(t->attr.name,t->isParameter==1?scope_a+1:scope_a) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location[scope_a]++,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          break;
          
        default:
          break;
      }
      break;
    case DecK:
       switch(t->kind.dec)
       { case VarK:
           t->scope=t->isParameter==1?scope_a+1:scope_a;
	   if (st_lookup(t->attr.name,t->isParameter==1?scope_a+1:scope_a) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location[scope_a]++,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          break;
        case ArrayK:
           t->scope=t->isParameter==1?scope_a+1:scope_a;
           if (st_lookup(t->attr.name,t->isParameter==1?scope_a+1:scope_a) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location[scope_a]++,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0,t->isParameter==1?scope_a+1:scope_a,t->isParameter==1?1:0);
          break;
        case FunK:
            t->scope=0; /* to parse tree */
           /* all functios are global so searching in global scope */
            if (st_lookup(t->attr.name,0) == -1)  
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,-1,0,0);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,-1,0,0);
          break;          
        }    
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { 
   case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          if ((t->attr.op == EQ) || (t->attr.op == LT) || (t->attr.op == LTE)|| (t->attr.op == GT)
               || (t->attr.op == GTE)|| (t->attr.op == NE))
            t->type = Boolean;
          else
            t->type = Integer;
          break;
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
          break;  
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
      
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case ReturnK:
          if(t->child[0]!=NULL) {
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"return of non-integer value");
           } 
          break;
        case CallK:
          if(t->child[0]!=NULL) {
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"call of non-integer value");
          }
          t->type=Integer;  
          break;
        case WhileK:
          if (t->child[1]->type == Integer)
            typeError(t->child[1],"while test is not Boolean");
          break;
        default:
          break;
      }
      break;
        
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
