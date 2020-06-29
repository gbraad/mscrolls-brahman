%{
#include <ctype.h>
%}
%%
[.?!]" "[a-z]	{	yytext[yyleng-1] = toupper(yytext[yyleng-1]);
					printf("%s", yytext); }
