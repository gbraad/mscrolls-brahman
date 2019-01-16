/*
 *
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <stdio.h>
#include "ofiles.h"
#include "tunnel.h"
#include "symbols.h"
#include "patch.h"
#include "memory.h"
#include "errors.h"

struct tunnel tunnels[MAXOBJFILES];
int n_tunnels = 0;

extern int patch;       /* !=0 if patching enabled */

#define IN_RANGE(src, dest) \
    (((dest) - (src)) <= 32767 && ((dest) - (src)) >= -32768)
#define abs(n) ((n)>=0?(n):-(n))

int SUCCEED(int type, int ad, const char* how)
{
    printf("patched ");
    if (how && *how) printf("%s, ", how);
    printf("successfully in");
    printfiles(type, ad);
    printf("\n");
    return ad;
}

void add_tunnel(int location)
{
    tunnels[n_tunnels].spare = 1;
    tunnels[n_tunnels++].location = location;
}

int patch_word(char* name, int type, int from, int to)
{
    int i, maxdist;
    struct tunnel *tp, *best;

    if (addtoword(name, type, from, to-from)) return 1; // ok
    if (!patch) return 1;
    
    if (type != N_TEXT)
    {
        printf("reference to data, patch unsuccessful\n");
        return 0;
    }

    /* look for an existing tunnel that is in range */
    for (i=0, tp=tunnels; i<n_tunnels; i++, tp++)
    {
        if (!tp->spare &&
            tp->destination == to && IN_RANGE(from, tp->location)) 
        {
            addtoword(name, type, from, tp->location-from);
            return SUCCEED(type, tp->location, "via existing tunnel");
        }
    }

    /* find the best empty tunnel which will work */
    /* best is defined to be furthest from the destination */
    maxdist = -1;
    best = 0;
    for (i=0, tp=tunnels; i<n_tunnels; i++, tp++)
    {
        if (tp->spare && IN_RANGE(from, tp->location) &&
            IN_RANGE(tp->location+2, to))
        {
            int d;
            if (from < to)
                d = to - (tp->location+2);
            else
                d = (tp->location+2) - to;
            
            if (d > maxdist)
            {
                maxdist = d;
                best = tp;
            }
        }
    }

    if (best)
    {
        /* got one ? */
        best->spare = 0;
        best->destination = to;
        addtoword("a patch", type, from, best->location-from); /* to the tunnel */
        addtoword(name, type, best->location+2, to-(best->location+2));
        return SUCCEED(type, best->location, "via new tunnel");
    }

    printf("attempting multiple tunnel relay\n");

    /* attempt a relay through 2 or more tunnels by recursing */

    // find spare tunnel furthest from source
    maxdist = -1;
    best = 0;
    for (i=0, tp=tunnels; i<n_tunnels; i++, tp++)
    {
        if (tp->spare && IN_RANGE(from, tp->location))
        {
            int d;
            if (from < to)
                d = tp->location - from;
            else
                d = from - tp->location;
            
            if (d > maxdist)
            {
                maxdist = d;
                best = tp;
            }
        }
    }

    if (best)
    {
        int relay;
        best->spare = 0;    /* remove from free list */
        
        /* attempt a patch from this tunnel to the destination */
        relay = patch_word(name, type, best->location+2, to);
        if (relay)
        {
            /* the patch was successful, so return success */
            /* note: SUCCEED will already have printed the success
               message at a lower level */
            /* fill in the branch to the first link */
            addtoword("a patch", type, from, best->location - from);
            return 1;
        }
        else
        {
            best->spare = 1;    /* patch failed, so tunnel still free */
            printf("patch failed\n");
            return 0;       /* return failure */
        }
    }

    printf("patch failed, no spare tunnels!\n");
    return 0;
}

