/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


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



