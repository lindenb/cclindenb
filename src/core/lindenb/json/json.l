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



extern int json_parse(Parser* ctx);


static NodePtr _parse(Parser* ctx)
	{
	return NULL;
	}

NodePtr Parser::parse(const char* s,size_t length)
	{
	if(s==NULL || length==0) return NULL;
	input_string=(char*)s;
	input_length=length;
	file_as_input=NULL;
	return _parse(this);
	}

NodePtr Parser::parse(FILE* in)
	{
	if(in==NULL) return NULL;
	file_as_input=in;
	input_string=NULL;
	return _parse(this);
	}


NodePtr Parser::parse(const char* s)
	{
	if(s==NULL) return NULL;
	return jsonNParse(s,strlen(s));
	}