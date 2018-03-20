#include <assert.h>
#include "ificlient.h"

extern IFIClient* ifi;

extern "C"
{
    
int ifi_getchar()
{
    assert(ifi);
    return ifi->getchar();
}

int ifi_putchar(int c)
{
    assert(ifi);
    return ifi->putchar(c);
}

int ifi_printf(const char* m, ...)
{
    assert(ifi);

    va_list args;
    va_start(args, m);
    int n = ifi->vprintf(m, args);
    va_end(args);
    return n;
}

char* ifi_gets(char* s)
{
    char* p = s;
    int c;
    
    while ((c = ifi_getchar()) != EOF && c != '\n') *p++ = c;
    *p = 0;
    return c == EOF && p == s ? 0 : s;
}

char* ifi_gets_s(char* s, size_t sz)
{
    char* p = s;
    int c;

    if (!sz) return 0;
    --sz; 
    
    while ((c = ifi_getchar()) != EOF && c != '\n')
        if (p - s < (int)sz) *p++ = c;

    *p = 0;
    return c == EOF && p == s ? 0 : s;
}

char* ifi_fgets(char* s, int n, FILE* fp)
{
    if (fp != stdin) return fgets(s, n, fp);
    if (n > 1)
    {
        // n-1 space for newline
        s = ifi_gets_s(s, n-1);
        if (s)
        {
            // append newline
            size_t sz = strlen(s);
            s[sz++] = '\n';
            s[sz] = 0;
        }
    }
    else
    {
        *s = 0;
    }
    return s;
}

int ifi_puts(const char* s)
{
    while (*s) ifi_putchar(*s++);
    ifi_putchar('\n');
    return 0;
}

int ifi_scanf(const char* f, ...)
{
   assert(0); // no
   return 0;
}

const char* ifi_getRequest()
{
    // C version 
    return ifi->getRequest();
}

void ifi_emitResponse(const char* json)
{
    // C version
    ifi->emitResponse(json);
}

} // extern C
