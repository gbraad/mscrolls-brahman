/*******************************************************************************
 * Stefan Bylund 2018
 *
 * ESXDOS utility functions.
 ******************************************************************************/

#ifndef _ESXDOS_UTIL_H
#define _ESXDOS_UTIL_H

int esx_getc(unsigned char filehandle) __z88dk_fastcall;

char *esx_gets(unsigned char filehandle, char *str, unsigned int num);

#endif
