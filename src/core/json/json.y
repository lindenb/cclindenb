
%{
#define YYDEBUG 1
#include "json.h"

%}

%union	{
	double _double;
	char _bool;
	long _long;	
	struct { char* s;int length;} literal;
	JSONNodePtr node;
	JSONNodePtr array;
	JSONNodePtr object;
	JSONObjetEntryPtr entry;
	} 	

%pure-parser
%name-prefix="json_"
%locations
%defines
%error-verbose
%parse-param{BisonContextPtr context}
%lex-param{void* my_yyscanner}

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
	if(context->error_flag!=0)
		{
		JSONNodeFree($1);
		$1=NULL;
		}
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
	$$=JSONNodeArrayNew();
	}
	| ARRAY_OPEN error ARRAY_CLOSE
	{
	$$=NULL;
	}
	;

array_items: any
		{
		$$=JSONNodeArrayNew();
		if(JSONNodeArrayAppend($$,$1)!=1)
			{
			JSONNodeFree($$);
			JSONNodeFree($1);
			$$=NULL;
			}
			
		}
	   | array_items COMMA any 
	   	{
	   	$$=$1;
	   	if(JSONNodeArrayAppend($$,$3)!=1)
			{
			JSONNodeFree($$);
			JSONNodeFree($3);
			$$=NULL;
			}
	   	};

object: OBJECT_OPEN object_items OBJECT_CLOSE
	{ $$=$2;}
	| OBJECT_OPEN OBJECT_CLOSE
	{ $$=JSONNodeObjectNew();}
	;

object_items: object_item
		{
		$$= JSONNodeObjectNew();
		if(JSONNodeObjectPut($$,$1)!=1)
			{
			JSONNodeFree($$);
			JSONObjetEntryFree($1);
			$$=NULL;
			}
		}
	   | object_items COMMA object_item
		{
		$$=$1;
		if(JSONNodeObjectPut($1,$3)!=1)
			{
			JSONNodeFree($1);
			JSONObjetEntryFree($3);
			$$=NULL;
			}
		
		}
	    | object_items COMMA error
	    	{
	    	JSONNodeFree($1);
	    	$$=NULL;
	    	}
	   ;

object_item: literal DOTDOT any
	{
	$$ = JSONObjetEntryNew($1,$3);
	if($$==NULL)
		{
		JSONNodeFree($1);
		JSONNodeFree($3);
		}
	
	};

literal: LITERAL 	{
	$$=JSONNodeNewNString($1.s,$1.length);
	if($1.s!=NULL)
		{
		free($1.s);
		}
	} ;
floating: FLOATING 	{ $$=JSONNodeNewFloating($1);} ;
integer: INTEGER	{ $$=JSONNodeNewInteger($1);} ;
nil: NIL		{ $$=JSONNodeNewNull();} ;
boolean: BOOLEAN 	{ $$=JSONNodeNewBoolean($1);};


%%
//extern int json_lex(YYSTYPE* lvalp,void* scanner);

#ifdef TEST_THIS_CODE

int main(int argc,char** argv)
	{
	long i;
	for( i=0;i<10L;++i)
		{
		JSONNodePtr ptr=jsonParse("{'PI':3.14,'a':[null,true,false,2.0,{},[]]}");
		DataStream ds= JSONSerialize(ptr);
		if(ds==NULL)
			{
			printf("ds is null\n");
			}
		else
			{
			JSONNodePtr ptr2= JSONDeserialize(ds);
			JSONPrint(stderr,ptr2);
			JSONNodeFree(ptr2);
			
			}
		DataStreamFree(ds);
		JSONNodeFree(ptr);
		}
	return 0;
	
	}
#endif
