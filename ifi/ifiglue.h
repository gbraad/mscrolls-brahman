
#pragma once

#ifdef IFI_BUILD

// in case these are macros
#undef getchar
#undef putchar
#undef puts

#define getchar ifi_getchar
#define putchar ifi_putchar
#define printf ifi_printf
#define gets ifi_gets
#define fgets ifi_fgets
#define puts ifi_puts
#define scanf ifi_scanf
#define gets_s ifi_gets_s

#ifdef __cplusplus
#define C_BEGIN extern "C" {
#define C_END }
#else
#define C_BEGIN
#define C_END
#endif

C_BEGIN
extern int ifi_getchar();
extern int ifi_putchar(int);
extern int ifi_printf(const char*, ...);
extern char* ifi_gets(char*);
extern char* ifi_gets_s(char* s, size_t sz);
extern char* ifi_fgets(char* s, int n, FILE*);
extern int ifi_puts(const char*);
extern int ifi_scanf(const char*,...);
extern int ifi_main(int, char**);
extern const char* ifi_getRequest();
extern void ifi_emitResponse(const char* json);
C_END

// need to move main into the main loop for IFIClient

#define main ifi_main

#endif // IFI_BUILD



