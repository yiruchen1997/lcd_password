%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
		
	extern int line_count;
	extern int word_count;
	extern char *yytext;
	
	int yylex();
	void yyerror(const char *s);
%}



%token PLUSPLUS MINUSMINUS ADD MINUS MOD BIGGER LESS BE LE EQ NE ASSIGN NOT AND OR INT DOUBLE FLOAT LONG SHORT BYTE CHAR STRING BOOLEAN IF ELSE FOR DO WHILE BREAK CONTINUE RETURN VOID CLASS CONST STATIC IMPORT NEW THIS PRINT READ THROW THROWS TRUE FALSE PUBLIC PROTECTED PRIVATE ABSTRACT FINAL PACKAGE EXTENDS SUPER TRANSIENT VOLATILE SYNCHRONIZED NATIVE INTERFACE SWITCH CASE DEFAULT TRY CATCH IMPLEMENTS FINALLY ID INTEGER REAL STR_CONST
%start goal
%%

/*program*/
goal               			:   class_declaration
							|   class_declaration goal
                			;
/*declaration*/
/*class*/
class_declaration   		:   CLASS ID class_body
                    		|   class_modifiers CLASS ID class_body
                   			;

class_modifiers     		:   class_modifier
                   			|   class_modifiers class_modifier
                    		;
                    		
class_modifier      		:   PUBLIC|ABSTRACT|FINAL
                    		;

class_body          		:   '{'  class_body_declaration '}'
                    		;

class_body_declaration      :   class_member_declaration
                    		|
                    		;

class_member_declaration    :   method_declaration
                    		|   field_declaration
                    		|   method_declaration class_member_declaration
                    		|   field_declaration class_member_declaration
                    		;
/*field*/
field_declaration           :   type_declaration
                    		|   field_modifiers type_declaration
                    		|   declare_new_var
                    		;

field_modifiers				:	field_modifier
							|	field_modifiers field_modifier
							;

field_modifier				:	PUBLIC|PROTECTED|PRIVATE|STATIC|FINAL|TRANSIENT|VOLATILE
							;			                     	
/*method*/
method_declaration       	:   method_modifiers type ID '(' formal_parameter_list ')' scope
                   			|   type ID '(' formal_parameter_list ')' scope
                    		|   ID '(' formal_parameter_list ')' scope
                    		;
                    		
method_modifiers			:	method_modifier
							|	method_modifiers method_modifier
							;
							
method_modifier				:	PUBLIC|PROTECTED|PRIVATE|STATIC|ABSTRACT|FINAL|SYNCHRONIZED|NATIVE
							;							                    		
/*formal parameter*/
formal_parameter_list   	:   formal_parameter
                        	|   formal_parameter_list ',' formal_parameter
                        	|
                        	;

formal_parameter        	:   type varaiable_declarator_id
                        	;   
/*variable*/
varaiable_declarator_id 	:   ID
                        	|   varaiable_declarator_id '[' ']' 
                        	|   ID ASSIGN INTEGER
                        	|	ID ASSIGN FLOAT
                        	;

variable_declarators		:	variable_declarator
							|	variable_declarators ',' variable_declarator
							;   
							
variable_declarator			:	ID
							|	ID ASSIGN expression
							;				                        	                 		
/*type*/                        
type_declaration		    :   type variable_declarators ';'
		                    |   array_definiation
		                    |   error
		                    ;                       

type						:	INT|LONG|SHORT|DOUBLE|FLOAT|CHAR|STRING|BYTE|BOOLEAN|VOID
							;					
/*array*/
array_definiation 			:   type '[' ']' ID ASSIGN NEW type '[' INTEGER ']' ';'
                    		;
/*statemet*/
scope   			        :   '{' statements '}'
							;
							
statements          		:   statement
                    		|   statements statement
                    		|
                    		;

statement					:   scope
							|	basic
                   			|   loop
                   			|   branch
                 			;

basic					    :	var ASSIGN expression ';'
							|	var PLUSPLUS ';'
							|	var MINUSMINUS ';'
							|	READ '(' var ')' ';'
							|	READ '(' ID ')' ';'
							|	PRINT '(' ID ')' ';'
							|	PRINT '(' STR_CONST ')' ';'
							|	PRINT '(' expression ')' ';'
							|	expression ';'
							|	type_declaration
							|	return_state
							|	declare_new_var
							;

var							:	ID
							|	ID '.' ID
							;
							
function					:	var '(' ')'
							|	var '(' argument ')'					
							;
							
argument					:	expression							
							|	expression ',' argument
							;	

return_state				:	RETURN ';'
							|	RETURN expression ';'
							;

declare_new_var				:	ID ID ASSIGN NEW ID '(' formal_parameter_list ')' ';'
							;

loop					    :	FOR '(' for_init ';' expression ';' for_update ')' statement_content
							|	DO statement_content WHILE '(' cond ')' ';'
							|	WHILE '(' cond ')' statement_content
							;

for_init					:	type_declaration
							;
							
for_update					:	ID PLUSPLUS
							|	ID MINUSMINUS
							|	PLUSPLUS ID
							|	MINUSMINUS ID
							;
							
cond						: 	expression compare expression							
							;
							
compare						:	BIGGER						
							|	LESS
							|	BE
							|	LE
							|	EQ
							|	NE
							;
							
statement_content			:	basic
							|	scope
							;
							
branch						:	IF '(' compare ')' statement_content
							|	IF '(' compare ')' statement_content branch_else
							|	SWITCH '(' expression ')' switch_scope
							;
							
branch_else					:	more_else_if							
							|	more_else_if branch_else
							;
							
more_else_if		        :   ELSE IF '(' compare ')' statement_content
                		    |   ELSE statement_content
                		    ;
                		    
switch_scope				:   '{' switch_statement_groups switch_labels '}'
                		    ;
                		    
switch_statement_groups		:	switch_statement_group                		    
                		    |	switch_statement_groups switch_statement_group
                		    ;
                		    
switch_statement_group		:   switch_labels expression
                		    ;
                		    
switch_labels				:   switch_label            		    
                		    |	switch_labels switch_label
                		    ;
                		    
switch_label                :	CASE expression	':'     
                		    |	DEFAULT ':'
                		    ;
/*expression*/
expression					:	object
							|	expression ADD object
							|	expression MINUS object
							;
							
object						:	element
							|	object '*' element							
							|	object '/' element
							;
							
element						:	ID							
							|	INTEGER
							|	FLOAT
							|	'(' expression ')'
							|	prefix ID
							|	ID postfix
							|	function
							;
							
prefix						:	ADD							
							|	MINUS
							|	PLUSPLUS
							|	MINUSMINUS
							;
							
postfix						:	PLUSPLUS
							|	MINUSMINUS	
							;

%%

int main(){
    printf("Line  1:");
    yyparse();
    printf("\n");
    
    return 0;
}

void yyerror(const char *s){
    	fprintf(stderr,"Line %2d: 1st char : %d, a syntax error at \"%s\"\n",line_count,word_count,yytext);
}
