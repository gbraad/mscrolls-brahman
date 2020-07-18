/*******************************************************************************
 * Stefan Bylund 2018
 *
 * ESXDOS utility functions.
 ******************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <arch/zxn/esxdos.h>

#include "esxdos_util.h"
#include "ide_friendly.h"

int esx_getc(unsigned char filehandle) __z88dk_fastcall
{
    unsigned int num_read;
    int c = 0;

    errno = 0;
    num_read = esx_f_read(filehandle, &c, 1);
    return (errno || (num_read != 1)) ? EOF : c;
}

char *esx_gets(unsigned char filehandle, char *str, unsigned int num)
{
    int c;
    unsigned int i = 0;

    while (((c = esx_getc(filehandle)) != '\n') && (c != EOF) && (i < num - 1))
    {
        str[i++] = c;
    }

    str[i] = '\0';

    return ((i == 0) && (c == EOF)) ? NULL : str;
}
