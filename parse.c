/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the C- compiler    */
/* C- project    				    */
/* ZEBRA Inc.	                                    */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode *declaration_list(void);
static TreeNode *declaration(void);
static TreeNode *var_declaration(void);
static TreeNode *param_list(void);
static TreeNode *param(void);
static TreeNode *compound_statement(void);
static TreeNode *local_declarations(void);
static TreeNode *statement_list(void);
static TreeNode *statement(void);
static TreeNode *expression_statement(void);
static TreeNode *selection_statement(void);
static TreeNode *iteration_statement(void);
static TreeNode *return_statement(void);
static TreeNode *expression(void);
static TreeNode *simple_expression(TreeNode *parsedown) ;
static TreeNode *additive_expression(TreeNode *parsedown);
static TreeNode *term(TreeNode *parsedown);
static TreeNode *factor(TreeNode *parsedown);
static TreeNode *args(void);
static TreeNode *arg_list(void);
static TreeNode *var_call(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

static ExpType type_specifier()
{
    ExpType t_type = Void;

    switch(token)
    {
    case INT:  t_type = Integer; token=getToken(); break;
    case VOID: t_type = Void; token=getToken(); break;
    default: {
             syntaxError("unexpected type ->");
             printToken(token, tokenString);
             fprintf(listing, "\n");
             break;
        }
    }

    return t_type;
}



static TreeNode *declaration_list(void)
{
    TreeNode *t;
    TreeNode *p;

    t= declaration();
    p = t;

    while (token != ENDFILE)
    {
	TreeNode *q; 

	q = declaration();
	if (q != NULL) 
	{
	    if(t==NULL) t=p=q;
	    else
	    {
            	p->sibling = q;
            	p = q;
            }
	}
    }

    return t;
}



static TreeNode *declaration(void)
{
    TreeNode  *t = NULL;
    ExpType   decType;
    char      *identifier;
    
    /* difficult to determine function or variable or array declaration till we find '(' or ';' or  '[' respectively  , so till then store type & id */ 	
    decType = type_specifier();   /* get type of declaration */

    identifier = copyString(tokenString);  /*get identifier */
    match(ID);
    
    /* now decide which declaration */
    switch(token)
    {
    case SEMI:     /* variable declaration */

        t = newDecNode(VarK);

	if (t != NULL)
	{
	    t->type = decType;
	    t->attr.name = identifier;
	}
	
	match(SEMI);
	break;
	
    case LB:  /* array declaration */
	t = newDecNode(ArrayK);

	if (t != NULL)
	{
	    t->type = decType;
	    t->attr.name = identifier;
	}
	
	match(LB);
	
	if (t != NULL) {
		t->value = atoi(tokenString);
		t->isArray=TRUE;
	}
	match(NUM);
	match(RB);
	match(SEMI);
	break;
	
    case LPAREN:   /* function declaration */
	t = newDecNode(FunK);

	if (t != NULL)
	{
	   
	    t->type = decType;
	    t->attr.name = identifier;
	}
	
	match(LPAREN);
	if (t != NULL) t->child[0] = param_list();
	match(RPAREN);
	if (t != NULL) t->child[1] = compound_statement();
	break;
	
    default:
	syntaxError("unexpected token ");
	printToken(token, tokenString);
        fprintf(listing, "\n");
	token = getToken();
	break;
    }


    return t;
}


static TreeNode *var_declaration(void)
{
    TreeNode  *t = NULL;
    ExpType   decType;
    char      *identifier;
    

    decType = type_specifier();

    identifier = copyString(tokenString);
    match(ID);
    
    switch(token)
    {
    case SEMI:     /* variable declaration */

	t = newDecNode(VarK);

	if (t != NULL)
	{
	    t->type = decType;
	    t->attr.name = identifier;
	}
	
	match(SEMI);
	break;
	
    case LB:  /* array declaration */
	t = newDecNode(ArrayK);

	if (t != NULL)
	{
	    t->type = decType;
	    t->attr.name = identifier;
	}
	
	match(LB);
	
	if (t != NULL) {
		t->value = atoi(tokenString);
		t->isArray=TRUE;
	}
	match(NUM);
	match(RB);
	match(SEMI);
	break;
	
    default:
	syntaxError("unexpected token ");
	printToken(token, tokenString);
        fprintf(listing, "\n");
	token = getToken();
	break;
    }

    return t;
}

static TreeNode *param_list(void)
{
    TreeNode *t;
    TreeNode *p;
    TreeNode *q;
    int param_count=0;
    if (token == VOID)
    {
        match(VOID);
        return NULL;  /* No parameter ,VOID */
    }

    t = param();
    p = t;
    param_count++;
    while ((t != NULL) && (token == COMMA))
    {
	match(COMMA);
	q = param();
	if (q != NULL)
        {
            param_count++;
	    p->sibling = q;
	    p = q;
        }
    }
    t->param_size=param_count;
    return t;
}


static TreeNode *param(void)
{
    TreeNode *t;
    ExpType  parmType;
    char     *identifier;
    

    parmType = type_specifier();  /* get type of formal parameter */
 
    identifier = copyString(tokenString);
    match(ID);

    /* array-type formal parameter */
    if (token == LB)
    {
	match(LB);
	match(RB);

	t = newDecNode(ArrayK);
	t->isArray=TRUE;
    }
    else
	t = newDecNode(VarK);
	
    if (t != NULL)
    {
	t->attr.name = identifier;
	t->value = 0;
	t->type = parmType;
	t->isParameter = TRUE;
    }
    
    return t;
}





static TreeNode *compound_statement(void)
{
    TreeNode *t = NULL;

    match(LF);

    if ((token != RF) && (t = newStmtNode(CompoundK)))   //follow of compoud & tree!=NULL
    {
	if ( (token==INT )|| (token==VOID))
	    t->child[0] = local_declarations();
	if (token != RF)
	    t->child[1] = statement_list();
    }

    match(RF);

    return t;
}


static TreeNode *local_declarations(void)
{
    TreeNode *t;
    TreeNode *p;
    TreeNode *q;
    int local_var=0;
    /* find first variable declaration, if it exists */
    if ( (token==INT )|| (token==VOID))
	t = var_declaration();

    /* subsequent variable declarations */
    if (t != NULL)
    {
        local_var++;
	p = t;

	while (( (token==INT )|| (token==VOID)))
	{
	    q = var_declaration();
	    if (q != NULL)
	    {
	        local_var++;
		p->sibling = q;
		p = q;
	    }
	}
    }
    t->local_size=local_var;
    return t;
}


static TreeNode *statement_list(void)
{
    TreeNode *t = NULL;
    TreeNode *p;
    TreeNode *q;

    /*first statement */
    if (token != RF)	// Follow(statement_list)={ '}' }
    {
        t = statement();
        p = t;

    /*subsequest statement */
        while (token != RF)
        {
            q = statement();
            if ((p != NULL) && (q != NULL))
            {
                p->sibling = q;
                p = q;
            }
        }
    }

   return t;
}


static TreeNode *statement(void)
{
    TreeNode *t = NULL;

    switch(token)
    {
    case IF: 
        t = selection_statement();
        break;
    case WHILE: 
        t = iteration_statement();
        break;
    case RETURN:
        t = return_statement();
        break;
    case LF:
        t = compound_statement();
        break;
    case ID:
    case SEMI:
    case LPAREN:
    case NUM:
        t = expression_statement();
        break;
    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }

    return t;
}


static TreeNode *expression_statement(void)
{
    TreeNode *t = NULL;

   if (token == SEMI) 
        match(SEMI);
    else if (token != RF)   //follow(expression_statement)
    {
        t = expression();
        match(SEMI);
    }

    return t;
}


static TreeNode *selection_statement(void)
{
    TreeNode *tree;
    TreeNode *expr;
    TreeNode *ifStmt;
    TreeNode *elseStmt = NULL;

    match(IF);
    match(LPAREN);
    expr = expression();
    match(RPAREN);
    ifStmt = statement();

    if (token == ELSE)
    {
	match(ELSE);
	elseStmt = statement();
    }

    tree = newStmtNode(IfK);
    if (tree != NULL)
    {
	tree->child[0] = expr;
	tree->child[1] = ifStmt;
	tree->child[2] = elseStmt;
    }

    return tree;
}


static TreeNode *iteration_statement(void)
{
    TreeNode *tree;
    TreeNode *expr;
    TreeNode *stmt;

    match(WHILE);
    match(LPAREN);
    expr = expression();
    match(RPAREN);
    stmt = statement();
    
    tree = newStmtNode(WhileK);
    if (tree != NULL)
    {
	tree->child[0] = expr;
	tree->child[1] = stmt;
    }

    return tree;
}


static TreeNode *return_statement(void)
{
    TreeNode *tree;
    TreeNode *expr = NULL;

    match(RETURN);

    tree = newStmtNode(ReturnK);
    if (token != SEMI) 		//follow(return_statement)
	expr = expression();

    if (tree != NULL) 
	tree->child[0] = expr;

    match(SEMI);

    return tree;
}


static TreeNode *expression(void)
{
    TreeNode *tree = NULL;
    TreeNode *lvalue = NULL;
    TreeNode *rvalue = NULL;
    int gotLvalue = FALSE;  /* boolean */

    /*
     *  At this point in the parse, we need to make a choice between
     *   parsing either an assignment or a simple-expression.
     *  To make the decision, we need to parse ident_statement *now*.
     */

    if (token == ID)
    {
        // fprintf(listing, ">>>   Parsing ident_statement\n");
        lvalue = var_call();
      //  if(lvalue!=NULL)
      //  printf("%d %s\n",lvalue->param_size,lvalue->attr.name);
        gotLvalue = TRUE;
    }

    /* Assignment? */
    if ((gotLvalue == TRUE) && (token == ASSIGN))
    {
        if ((lvalue != NULL) && (lvalue->nodekind == ExpK) && 
            (lvalue->kind.exp == IdK))
        {
           // fprintf(listing, ">>>   Generating node for ASSIGN\n"); 
            match(ASSIGN);
            rvalue = expression();

            tree = newExpNode(AssignK);
            if (tree != NULL)
            {
                tree->child[0] = lvalue;
                tree->child[1] = rvalue;
            }
        }
        else
        { 
            syntaxError("attempt to assign to something not an lvalue\n");
            token = getToken();
        }
    }
    else
        tree = simple_expression(lvalue);
	//if(lvalue!=NULL)printf("%d %s\n",lvalue->param_size,lvalue->attr.name);
    return tree;
}


static TreeNode *simple_expression(TreeNode *passdown)
{
    TreeNode *tree;
    TreeNode *lExpr = NULL;
    TreeNode *rExpr = NULL;
    TokenType operator;

  //   fprintf(listing, "*** Entered simple_expression()\n");

    lExpr = additive_expression(passdown);

    if ((token == LTE) || (token == GTE) || (token == GT) 
     || (token == LT) || (token == EQ) || (token == NE))
    {                                                          /* if only relop subsequent additive_expression call */
        operator = token;
        match(token);
        rExpr = additive_expression(NULL);

        tree = newExpNode(OpK);
        if (tree != NULL) 
        {
            tree->child[0] = lExpr;
            tree->child[1] = rExpr;
            tree->attr.op = operator;
        }
    }
    else 
        tree = lExpr;

    return tree;
}


static TreeNode *additive_expression(TreeNode *passdown)
{
    TreeNode *tree;
    TreeNode *newNode;

    //fprintf(listing, "*** Entered additive_expression()\n");

    tree = term(passdown);

    while ((token == PLUS) || (token == MINUS))    //first(addop)
    {
        newNode = newExpNode(OpK);
        if (newNode != NULL)
        {
            newNode->child[0] = tree;
            newNode->attr.op = token;
            tree = newNode;
            match(token);
            tree->child[1] = term(NULL);
        }
    }

    return tree;
}


static TreeNode *term(TreeNode *passdown)
{
    TreeNode *tree;
    TreeNode *newNode;

    //fprintf(listing, "*** Entered term()\n"); 

    tree = factor(passdown);

    while ((token == TIMES) || (token == OVER))
    {
        newNode = newExpNode(OpK);

        if (newNode != NULL)
        {
            newNode->child[0] = tree;
            newNode->attr.op = token;
            tree = newNode;
            match(token);
            newNode->child[1] = factor(NULL);
        }
    }

    return tree;
}


static TreeNode *factor(TreeNode *passdown)
{
    TreeNode *tree = NULL;

     //fprintf(listing, "*** Entered factor()\n");

    /* If the subtree in "passdown" is a Factor, pass it back. */
    if (passdown != NULL)
    {
    //    fprintf(listing, ">>>   Returning passdown subtree\n"); 
    //printf("%d this  %s\n",passdown->param_size,passdown->attr.name);
        return passdown;
    }

    if (token == ID)
    {
        tree = var_call();
    }
    else if (token == LPAREN)
    {
        match(LPAREN);
        tree = expression();
        match(RPAREN);
    }
    else if (token == NUM)
    {
        tree = newExpNode(ConstK);
        if (tree != NULL)
        {
            tree->attr.val = atoi(tokenString);
            tree->type = Integer;
        }
        match(NUM);
    }
    else
    {
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
    }

    return tree;
}


static TreeNode *var_call(void)
{
    TreeNode *tree;
    TreeNode *expr = NULL;
    TreeNode *arguments = NULL;
    char *identifier;


   // fprintf(listing, "*** Entered ident_statement()\n");

    if (token == ID)
        identifier = copyString(tokenString);
    match(ID);

    if (token == LPAREN)   /*call statement */
    {
        match(LPAREN);
        arguments = args();
        match(RPAREN);

        tree = newStmtNode(CallK);
        if (tree != NULL)
        {
            tree->child[0] = arguments;
            if(arguments!=NULL)
            	tree->param_size=arguments->param_size;
            else
            	tree->param_size=0;	
            tree->attr.name = identifier;
           // printf("%d %s\n",tree->param_size,tree->attr.name);
        }
    }
    /*variable statement */
    else		/*Array variable*/
    {
        if (token == LB)
        {
            match(LB);
            expr = expression(); 
            match(RB);
        }
	/*variable */
        tree = newExpNode(IdK);
        if (tree != NULL)
        {
            tree->child[0] = expr;
            tree->attr.name = identifier;
        }
    }

    return tree;
}


static TreeNode *args(void)
{
    TreeNode *tree = NULL;
	
   // fprintf(listing, "*** Entered args()\n");
    //tree->param_size=0;
    if (token != RPAREN)	//follow(args)
        tree = arg_list();

    return tree;
}


static TreeNode *arg_list(void)
{
    TreeNode *tree;
    TreeNode *ptr;
    TreeNode *newNode;
    int arg_count=0;
   // fprintf(listing, "*** Entered arg_list()\n"); 

    tree = expression();
    ptr = tree;
    arg_count++;
    while (token == COMMA)
    {
        match(COMMA);
        newNode = expression();
	arg_count++;
        if ((ptr != NULL) && (tree != NULL))
        {
            
            ptr->sibling = newNode;
            ptr = newNode;
        }
    }
    tree->param_size=arg_count;	
	
    return tree;
}



/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = declaration_list();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
