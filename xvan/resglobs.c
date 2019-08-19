
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
/*                                                                      */
/* <marnix.home@gmail.com>                                              */
/*                                                                      */
/* This file is part of XVAN, an Interactive Fiction authoring system.  */
/*                                                                      */
/* XVAN is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, version 3.                             */
/*                                                                      */
/* XVAN is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with XVAN.  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                      */
/************************************************************************/


#include <stdio.h>
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "resglobs.h"

/*************************/
/* Function declarations */
/*************************/

void    ResetGlobals(void);  /* oct 18 18 */

/************************/
/* Function definitions */
/************************/

void ResetGlobals(void)
{
  /* this function is used to reset the */
  /* globals in case of a restart       */

  testmode = 0;
  /* transcript  = 0; */  /* transcrip keeps its current value */

  active_entity = NO_ID;

  curr_loc      = NO_ID;
  direction     = NO_ID;
  prepos        = NO_ID;
  action        = NO_ID;
  actor         = PLAYER;
  subject       = NO_ID;
  specifier     = NO_ID;
  value         = 0;
  ordinal       = 0;

  indent        = 0;
  capital       = 1;
  article       = 0;
  muted         = 0;

  fire_timers   = OK;

  disambig_score = 0;

  nr_of_locs           = 0;
  nr_of_objs           = 0;
  nr_of_cflags         = 0;
  loc_flags_string_len = 0;
  nr_of_cattrs         = 0;
  nr_of_lattrs         = 0;
  nr_of_directions     = 0;
  nr_of_timers         = 0;

  sp                   = 0;
}
