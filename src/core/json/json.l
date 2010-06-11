%option reentrant
%option prefix="json_"
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno
%option stack

%{
#include <stdio.h>
#include "json.h"
#include "json.tab.h"

#define CAPACITY 1000

#define YY_INPUT(buf,result,max_size) \
             { \
             if(yyget_extra(yyscanner)->file_as_input!=NULL)\
             	{\
             	int c = fgetc(yyget_extra(yyscanner)->file_as_input);\
             	if(c==EOF) { result=YY_NULL;} else {buf[0]=c; result=1;}\
             	}\
             else\
             	{\
		int nAvail=yyget_extra(yyscanner)->input_length - yyget_extra(yyscanner)->input_pos ;\
		int nToRead=(nAvail<max_size?nAvail:max_size);\
		if(nToRead==0) \
			{\
			result = YY_NULL;\
			}\
		else \
			{\
			memcpy(buf,\
				&(yyget_extra(yyscanner)->input_string)[yyget_extra(yyscanner)->input_pos],\
				sizeof(char)*nToRead\
				);\
			result=nToRead;\
			yyget_extra(yyscanner)->input_pos+=nToRead;\
			}\
		}\
             }


#define YY_EXTRA_TYPE BisonContextPtr
	

//forward declaration
void transferBufferToYYSVAL(yyscan_t yyscanner);


#define FLEX_BUFFER_APPEND(c) {\
	DataStream ds=(DataStream)yyget_extra(yyscanner)->flexbuff;\
	if(ds!=NULL)\
		{\
		if(DataStreamAppendC(ds,c)!=1)\
			{\
			DataStreamFree(ds);\
			yyget_extra(yyscanner)->flexbuff=NULL;\
			}\
		}\
	}
%}

%x IN_COMMENT APOS_STRING QUOT_STRING

%%

<IN_COMMENT>{
"*/"      yy_pop_state(yyscanner);
.|\n	  ;
}

<APOS_STRING>\' {
		yy_pop_state(yyscanner);
		FLEX_BUFFER_APPEND('\0');
		transferBufferToYYSVAL(yyscanner);
		return LITERAL;
		}
<QUOT_STRING>\" {
		yy_pop_state(yyscanner);
		FLEX_BUFFER_APPEND('\0');
		transferBufferToYYSVAL(yyscanner);
		return LITERAL;
		}
<APOS_STRING,QUOT_STRING>{
\\n	{  FLEX_BUFFER_APPEND('\n');}
\\t	{  FLEX_BUFFER_APPEND('\t');}
\\r	{  FLEX_BUFFER_APPEND('\r');}
\\b	{  FLEX_BUFFER_APPEND('\b');}
\\v	{  FLEX_BUFFER_APPEND('\v');}
\\\"	{  FLEX_BUFFER_APPEND('\"');}
\\\'	{  FLEX_BUFFER_APPEND('\'');}
\\\\	{  FLEX_BUFFER_APPEND('\\');}
.|\n	{  FLEX_BUFFER_APPEND(yyget_text(yyscanner)[0]);}
<<EOF>> {
	DataStream ds=(DataStream)yyget_extra(yyscanner)->flexbuff;
	yyget_extra(yyscanner)->flexbuff=NULL;
	DataStreamFree(ds);
	}
}

"/*"              yy_push_state(IN_COMMENT,yyscanner);

\"  {yy_push_state(QUOT_STRING,yyscanner); yyget_extra(yyscanner)->flexbuff=(void*)DataStreamNew();}
\'  {yy_push_state(APOS_STRING,yyscanner); yyget_extra(yyscanner)->flexbuff=(void*)DataStreamNew();}
\[ return ARRAY_OPEN;
\] return ARRAY_CLOSE;
\{ return OBJECT_OPEN;
\} return OBJECT_CLOSE;
\: return DOTDOT;
, return COMMA;
true  {yyget_lval(yyscanner)->_bool=1; return BOOLEAN;}
false {yyget_lval(yyscanner)->_bool=0; return BOOLEAN;} 
null { return NIL;} 
[-+]?[[:digit:]]+	{  yyget_lval(yyscanner)->_long = atol(yyget_text(yyscanner)); return INTEGER;}
[-+]?([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)? {  yyget_lval(yyscanner)->_double = atof(yyget_text(yyscanner)); return FLOATING;}
[ \t\n]		;
.	{return yyget_text(yyscanner)[0]; }
%%

void transferBufferToYYSVAL(yyscan_t yyscanner)
	{
	char* copy=NULL;
	DataStream ptr=(DataStream)(yyget_extra(yyscanner)->flexbuff);
	if(ptr!=NULL)
		{
		copy= malloc(DataStreamSize(ptr)+1);
		if(copy!=NULL)
			{
			memcpy(copy,ptr->data,sizeof(char)*DataStreamSize(ptr));
			ptr->data[DataStreamSize(ptr)]=0;
			yyget_lval(yyscanner)->literal.s= copy;
			yyget_lval(yyscanner)->literal.length= DataStreamSize(ptr);
			}
		}
	if(copy==NULL)
		{
		yyget_extra(yyscanner)->error_flag=1;
		yyget_lval(yyscanner)->literal.s=NULL;
		yyget_lval(yyscanner)->literal.length=-1;
		}
	DataStreamFree(ptr);
	yyget_extra(yyscanner)->flexbuff=NULL;
	}

extern int json_parse(BisonContextPtr context);


static JSONNodePtr _parse(BisonContextPtr ctx)
	{
	//YYSTYPE test;
	
	yyscan_t scanner;
	
	if(json_lex_init_extra(ctx,&scanner)!=0)
		{
		return NULL;
		}
	ctx->my_yyscanner=scanner;
	json_set_debug(1,scanner);	

	json_parse(ctx);	
	yylex_destroy(scanner);
	return ctx->node;
	}

JSONNodePtr jsonNParse(const char* s,int length)
	{
	BisonContext ctx;
	if(s==NULL) return NULL;
	memset(&ctx,0,sizeof(BisonContext));
	ctx.input_string=(char*)s;
	ctx.input_length=length;
	
	return _parse(&ctx);
	}

JSONNodePtr jsonParseFile(FILE* in)
	{
	BisonContext ctx;
	if(in==NULL) return NULL;
	memset(&ctx,0,sizeof(BisonContext));
	ctx.file_as_input=in;
	return _parse(&ctx);
	}


JSONNodePtr jsonParse(const char* s)
	{
	if(s==NULL) return NULL;
	return jsonNParse(s,strlen(s));
	}