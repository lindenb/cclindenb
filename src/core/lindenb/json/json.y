
%{
#define YYDEBUG 1
#include "json.h"

%}

%union	{
	Node::json_floating_type _double;
	char _bool;
	Node::json_integer_type _long;
	std::string* literal;
	NodePtr node;
	ArrayNode* array;
	ObjectNode* object;
	std::pair<std::string*,NodePtr>* entry;
	} 	

%pure-parser
%name-prefix="json_"
%locations
%defines
%error-verbose
%parse-param{lindenb:json::Parser context}

%token COMMA DOTDOT ARRAY_OPEN ARRAY_CLOSE OBJECT_OPEN OBJECT_CLOSE  NIL
%token<literal> LITERAL
%token<_long> INTEGER
%token<_bool> BOOLEAN
%token<_double> FLOATING

%type<node> literal
%type<node> integer
%type<node> nil
%type<node> boolean
%type<node> floating
%type<array> array array_items
%type<object> object
%type<object> object_items
%type<entry> object_item
%type<node> any
%start input
%{

void yyerror(YYLTYPE* locp,BisonContextPtr context,const char* message)
	{
	if(context->error_stream!=NULL)
		{
		fprintf(context->error_stream,"ERROR:\n\t%s (line %d)\n",message,
			locp->first_line
			);
		}
	context->error_flag=1;
	}
#define my_yyscanner (context->my_yyscanner)


%}



%%

input: any
	{
	context->node=$1;
	};

any:      array { $$=$1;}
	| object  { $$=$1;}
        | literal  { $$=$1;}
        | nil { $$=$1;}
        | floating { $$=$1;}
        | integer { $$=$1;}
        | boolean { $$=$1;}
        | error { $$=NULL; context->error_flag=1;}
        | any error { JSONNodeFree($1); $$=NULL;context->error_flag=1;}
        ;

array: ARRAY_OPEN array_items ARRAY_CLOSE
		{
		$$=$2;
		}
	| ARRAY_OPEN  ARRAY_CLOSE
		{
		$$= new ArrayNode;
		}
	;

array_items: any
		{
		$$=new ArrayNode;
		$$->vector().push_back($1);
		}
	   | array_items COMMA any 
	   	{
	   	$$=$1;
	   	$$->vector().push_back($3);
	   	};

object: OBJECT_OPEN object_items OBJECT_CLOSE
	{ $$=$2;}
	| OBJECT_OPEN OBJECT_CLOSE
	{ $$=new ObjectNode();}
	;

object_items: object_item
		{
		$$= new ObjectNode;
		$$->map().insert(std::pair<std::string,NodePtr>(
			*($1->first),
			$1->second
			);
		delete $1->first;
		}
	   | object_items COMMA object_item
		{
		$$=$1;
		$$->map().insert(std::pair<std::string,NodePtr>(
			*($3->first),
			$3->second
			);
		delete $3->first;
		}
	   ;

object_item: literal DOTDOT any
	{
	$$ = new std::pair<std::string*,NodePtr>($1,$3);
	};

literal: LITERAL
	{
	$$=new StringNode(*$1);
	delete $1;
	} ;
floating: FLOATING 	{ $$= new FloatingNode($1);} ;
integer: INTEGER	{ $$= new IntegerNode($1);} ;
nil: NIL		{ $$= new NullNode();} ;
boolean: BOOLEAN 	{ $$= new BooleanNode($1);};


%%
