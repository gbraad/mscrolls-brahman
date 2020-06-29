%{
#include <ctype.h>
%}

cap		[A-Z]
%%

[.!?]\ *{cap}	{	yytext[yyleng-1] = tolower(yytext[yyleng-1]);
			printf("%s", yytext); }
