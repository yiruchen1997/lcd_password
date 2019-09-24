%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

#define KEYWORD 1
#define SYMBOL 2
#define OPERATER 3
#define NUM 4
#define IDENTIFIER 5

int i = 0, j = 0;

int word_count = 1;
int line_count = 1; 
int key = -1;
int scope = 0, duplicate = 0;
char dup_word[100] = "";

char *symbol_table1[2000];
char *symbol_table2[2000];
char *symbol_table3[2000];
char *symbol_table4[2000];
char *symbol_table5[2000];
int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0;

void create(){
	num1 = 0; num2 = 0; num3 = 0; num4 = 0; num5 = 0;scope = 0;
	for(i = 0;i<2000; i++){
		symbol_table1[i] = NULL;
		symbol_table2[i] = NULL;
		symbol_table3[i] = NULL;
		symbol_table4[i] = NULL;
		symbol_table5[i] = NULL;
	}
}
int lookup(char *s){
	if(scope < 0) printf("ERROR\n");
	if(scope == 1){
		for(i = 0; i<num1; i++){
			if(!strcmp(s,symbol_table1[i])){
				if(key == KEYWORD ||key == IDENTIFIER){
					duplicate = 1;
					strcpy(dup_word, s);
				}
				return 0;
			}
		}
	}
	if(scope == 2){
		for(i = 0; i<num2; i++){
			if(!strcmp(s,symbol_table2[i])){
				if(key == KEYWORD ||key == IDENTIFIER){
					duplicate = 1;
					strcpy(dup_word, s);
				}
				return 0;
			}
		}
	}
	if(scope == 3){
		for(i = 0; i<num3; i++){
			if(!strcmp(s,symbol_table3[i])){
				if(key == KEYWORD ||key == IDENTIFIER){
					duplicate = 1;
					strcpy(dup_word, s);
				}
				return 0;
			}
		}
	}
	if(scope == 4){
		for(i = 0; i<num4; i++){
			if(!strcmp(s,symbol_table4[i])){
				if(key == KEYWORD ||key == IDENTIFIER){
					duplicate = 1;
					strcpy(dup_word, s);
				}
				return 0;
			}
		}
	}
	if(scope == 5){
		for(i = 0; i<num5; i++){
			if(!strcmp(s,symbol_table5[i])){
				if(key == KEYWORD ||key == IDENTIFIER){
					duplicate = 1;
					strcpy(dup_word, s);
				}
				return 0;
			}
		}
	}
	return -1;
}
void insert(char *s){
	if(scope < 0) printf("ERROR\n");
	if(scope == 1){
		symbol_table1[num1]=(char*) malloc(sizeof(char*));
		strcpy(symbol_table1[num1],s);
		num1++;
	}
	if(scope == 2){
		symbol_table2[num2]=(char*) malloc(sizeof(char*));
		strcpy(symbol_table2[num2],s);
		num2++;
	}
	if(scope == 3){
		symbol_table3[num3]=(char*) malloc(sizeof(char*));
		strcpy(symbol_table3[num3],s);
		num3++;
	}
	if(scope == 4){
		symbol_table4[num4]=(char*) malloc(sizeof(char*));
		strcpy(symbol_table4[num4],s);
		num4++;
	}
	if(scope == 5){
		symbol_table5[num5]=(char*) malloc(sizeof(char*));
		strcpy(symbol_table5[num5],s);
		num5++;
	}
}
void clear(int scope){
	if(scope == 1){
		for(i = 0;i<2000;i++)
			symbol_table1[i] = NULL;
			num1 = 0;
	}
	if(scope == 2){
		for(i = 0;i<2000;i++)
			symbol_table2[i] = NULL;
			num2 = 0;
	}
	if(scope == 3){
		for(i = 0;i<2000;i++)
			symbol_table3[i] = NULL;
			num3 = 0;
	}
	if(scope == 4){
		for(i = 0;i<2000;i++)
			symbol_table4[i] = NULL;
			num4 = 0;
	}
	if(scope == 5){
		for(i = 0;i<2000;i++)
			symbol_table5[i] = NULL;
			num5 = 0;
	}
}
%}

add "+"
minus "-"
plusplus "++"
minusminus "--"
bigger ">"
less "<"
mod "%"
le "<="
be ">="
eq "=="
ne "!="
assign "="
not "!"
and "&&"
or "||"

int int
long long
short short
double double
byte byte
float float
char char
string String
boolean boolean
void void

static static
const const
new new
final final
import import
public public
protected protected
private private
class class
this this
read read
print print
return return

if if
else else
do do
while while
for for
switch switch
case case 
default default
break break
continue continue
true true
false false

try try
catch catch
package package
synchronized synchronized
transient transient
volatile volatile
abstract abstract
extends extends
throw throw
throws throws
super super
native native
interface interface
implements implements
finally finally

id [A-Za-z_$][A-Za-z0-9_\$]*
integer [-]?[0-9]+
real [-]?[0-9]+[\.][0-9]+|[-]?[0-9]+\.[0-9]+[eE]\+[0-9]+
string_const ["].*["]

change_line \n
Whitespace [\ \t\r]
comment [/][\*][^\*]*.*[^\*]*[\*][/]|[/][/].*
character .

%%

{add} {printf("%s", yytext);word_count += yyleng;return ADD;}
{minus} {printf("%s", yytext);word_count += yyleng;return MINUS;}
{plusplus} {printf("%s", yytext);word_count += yyleng;return PLUSPLUS;}
{minusminus}  {printf("%s", yytext);word_count += yyleng;return MINUSMINUS;}
{bigger} {printf("%s", yytext);word_count += yyleng;return BIGGER;}
{less} {printf("%s", yytext);word_count += yyleng;return LESS;}
{mod} {printf("%s", yytext);word_count += yyleng;return MOD;}
{le} {printf("%s", yytext);word_count += yyleng;return LE;}
{be} {printf("%s", yytext);word_count += yyleng;return BE;}
{eq} {printf("%s", yytext);word_count += yyleng;return EQ;}
{ne} {printf("%s", yytext);word_count += yyleng;return NE;}
{assign} {printf("%s", yytext);word_count += yyleng;key = 0;return ASSIGN;}
{not} {printf("%s", yytext);word_count += yyleng;return NOT;}
{and} {printf("%s", yytext);word_count += yyleng;return AND;}
{or} {printf("%s", yytext);word_count += yyleng;return OR;}

"." {printf("%s", yytext);word_count += yyleng;return '.';}
"," {printf("%s", yytext);word_count += yyleng;return ',';}
":" {printf("%s", yytext);word_count += yyleng;return ':';}
";" {printf("%s", yytext);word_count += yyleng;return ';';}
"/" {printf("%s", yytext);word_count += yyleng;return '/';}
"[" {printf("%s", yytext);word_count += yyleng;return '[';}
"]" {printf("%s", yytext);word_count += yyleng;return ']';}
"{" {printf("%s", yytext);word_count += yyleng;scope++;return '{';}
"(" {printf("%s", yytext);word_count += yyleng;return '(';}
")" {printf("%s", yytext);word_count += yyleng;return ')';}
"}" {printf("%s", yytext);word_count += yyleng;clear(scope);scope--;return '}';}

{int} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return INT;}
{long} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return LONG;}
{short} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return SHORT;} 
{double} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return DOUBLE;}
{byte} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return BYTE;} 
{float} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return FLOAT;}
{char} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return CHAR;}
{string} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return STRING;}
{boolean} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return BOOLEAN;}
{void} {printf("%s", yytext);word_count += yyleng;key = KEYWORD;return VOID;}

{static} {printf("%s", yytext);word_count += yyleng;return STATIC;} 
{const} {printf("%s", yytext);word_count += yyleng;return CONST;}
{new} {printf("%s", yytext);word_count += yyleng;return NEW;} 
{final} {printf("%s", yytext);word_count += yyleng;return FINAL;} 
{import} {printf("%s", yytext);word_count += yyleng;return IMPORT;}
{public} {printf("%s", yytext);word_count += yyleng;return PUBLIC;} 
{protected} {printf("%s", yytext);word_count += yyleng;return PROTECTED;}
{private} {printf("%s", yytext);word_count += yyleng;return PRIVATE;}
{class} {printf("%s", yytext);word_count += yyleng;return CLASS;}
{this} {printf("%s", yytext);word_count += yyleng;return THIS;} 
{read} {printf("%s", yytext);word_count += yyleng;return READ;}
{print} {printf("%s", yytext);word_count += yyleng;return PRINT;}
{return} {printf("%s", yytext);word_count += yyleng;return RETURN;}

{if} {printf("%s", yytext);word_count += yyleng;return IF;}
{else} {printf("%s", yytext);word_count += yyleng;return ELSE;}
{do} {printf("%s", yytext);word_count += yyleng;return DO;}
{while} {printf("%s", yytext);word_count += yyleng;return WHILE;}
{for} {printf("%s", yytext);word_count += yyleng;return FOR;}
{switch} {printf("%s", yytext);word_count += yyleng;return SWITCH;}
{case} {printf("%s", yytext);word_count += yyleng;return CASE;}
{default} {printf("%s", yytext);word_count += yyleng;return DEFAULT;}
{break} {printf("%s", yytext);word_count += yyleng;return BREAK;}
{continue} {printf("%s", yytext);word_count += yyleng;return CONTINUE;}
{true} {printf("%s", yytext);word_count += yyleng;return TRUE;}
{false} {printf("%s", yytext);word_count += yyleng;return FALSE;}

{try} {printf("%s", yytext);word_count += yyleng;return TRY;}
{catch} {printf("%s", yytext);word_count += yyleng;return CATCH;}
{package} {printf("%s", yytext);word_count += yyleng;return PACKAGE;}
{synchronized} {printf("%s", yytext);word_count += yyleng;return SYNCHRONIZED;}
{transient} {printf("%s", yytext);word_count += yyleng;return TRANSIENT;}
{volatile} {printf("%s", yytext);word_count += yyleng;return VOLATILE;}
{abstract} {printf("%s", yytext);word_count += yyleng;return ABSTRACT;}
{extends} {printf("%s", yytext);word_count += yyleng;return EXTENDS;}
{throw} {printf("%s", yytext);word_count += yyleng;return THROW;}
{throws} {printf("%s", yytext);word_count += yyleng;return THROWS;}
{super} {printf("%s", yytext);word_count += yyleng;return SUPER;}
{native} {printf("%s", yytext);word_count += yyleng;return NATIVE;}
{interface} {printf("%s", yytext);word_count += yyleng;return INTERFACE;}
{implements} {printf("%s", yytext);word_count += yyleng;return IMPLEMENTS;}
{finally} {printf("%s", yytext);word_count += yyleng;return FINALLY;}

{id} {
	if(lookup(yytext) == -1 && key == KEYWORD){insert(yytext);}
	printf("%s",yytext);
	word_count += yyleng;
	key = IDENTIFIER;
	return ID;
}
{integer} {printf("%s", yytext);word_count += yyleng;key = NUM;return INTEGER;}
{real} {printf("%s", yytext);word_count += yyleng;return REAL;}
{string_const} {printf("%s", yytext);word_count += yyleng;return STR_CONST;}

{change_line} {
    if(duplicate == 1){
		printf("\n> '%s' is a duplicate identifier.", dup_word);
		duplicate = 0;
		strcpy(dup_word, "");
	}
	line_count++;
	printf("\nLine %2d:", line_count);
	word_count = 1;key = 0;
}
{Whitespace} {printf("%s", yytext);word_count+=yyleng;}
{comment} {printf("%s", yytext);}
{character} {
	printf("*[error] Line: %2d, 1st char: %d, %s is an \"error\".\n", line_count, word_count, yytext);
	word_count++;
}

%%

int yywrap(){
    return 1;
}
