
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "spantree.h"


/*************************/
/* Function declarations */
/*************************/

int32_t InitSpanTree(spanTree**, int32_t**);
int32_t ExpandTree(int32_t, spanTree*);
int32_t BuildRoute(int32_t, int32_t*, spanTree*);


/************************/
/* Function definitions */
/************************/

int32_t InitSpanTree(tree, route)
 spanTree **tree;
 int32_t  **route;
{
  int i = 0;

  /* Initialize spanning tree and route. */
  if ((*tree = (spanTree *) malloc(nr_of_locs*sizeof(spanTree))) == NULL) {
    sprintf(outputline, "XeqInitSpanTree(): error in malloc() for tree.\n");
    return(ERROR);
  }
  if ((*route = (int32_t *) malloc(nr_of_locs*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "XeqInitSpanTree(): error in malloc() for route.\n");
    Output(outputline);
    return(ERROR);
  }
  for (i=0; i<nr_of_locs; i++) {
    (*tree)[i].level    = -1;
    (*tree)[i].from_loc = NO_ID;
    (*tree)[i].from_dir = NO_ID;
    (*route)[i]         = NO_ID;
  }
  return(OK);
}

int32_t BuildRoute(to, route, tree)
 int32_t  to;
 int32_t  *route;
 spanTree *tree;
{
  int index = 0;
  int i     = 0;

  /* this functions creates a route by working */
  /* backwards from to until it reaches from   */
  /* tree must have been built by ExpandTree() */

  route[0] = to;
  index    = to-FIRST_LOCATION_ID;
  i = 0;

  /* tree record with level 0 is starting location */
  while (tree[index].level != 0 && i <nr_of_locs) {
    route[++i] = tree[index].from_loc; /* can also use from_dir */
    index = tree[index].from_loc - FIRST_LOCATION_ID;
  }
  if (index == nr_of_locs) {
    PrintString("BuildRoute(): error, no starting point.\n");
    return(ERROR);
  }
  return(OK);
}

int32_t ExpandTree(level, tree)
 int32_t  level;
 spanTree *tree;
{
  int i = 0;
  int j = 0;

  int32_t to_loc        = NO_ID;
  int32_t tree_changed  = 0;

  /* check all locations in tree for level-1 and update tree if so */
  for (i=0; i<nr_of_locs; i++) {
    if (tree[i].level == level-1) {
      /* add locs that are reachable from this one */
      for (j=0; j<nr_of_directions; j++) {
        to_loc = exit_data[i*nr_of_directions+j];
        if (to_loc != NO_ID) {
          /* check if to_loc has already been updated earlier in path */
          if (tree[to_loc-FIRST_LOCATION_ID].level == -1) {
            /* struct has no been touched yet */
            tree[to_loc-FIRST_LOCATION_ID].from_loc = i+FIRST_LOCATION_ID;
            tree[to_loc-FIRST_LOCATION_ID].from_dir = j+first_direction_id;
            tree[to_loc-FIRST_LOCATION_ID].level    = level;
            tree_changed = 1;
          }
        }
      }
    }
  }
  return(tree_changed);
}

