%{
#include <ctype.h>
%}

alpha	[a-zA-Z]
%%
[.\?!^]" "?[A-Z]	{
						putchar(yytext[0]);
						putchar(tolower(yytext[yyleng-1]));
					}
[,.!\?:;]" "+{alpha}	{
						putchar(yytext[0]);
						putchar(yytext[yyleng-1]);
					}
" "+				putchar(' ');
