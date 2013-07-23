// c/mmerge.c rev. 22 July 2013 by Stuart Ambler.
// Merge of k sorted arrays of ints.  See c/mmerge.h for further comments.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

#include <stdbool.h>

#include "./mmerge.h"
#include "./pqueue.h"

// minptrix is a function to find the minimum value pointed to by an array
// p_p_int of pointers that point to elements of the corresponding (array)
// elements of arrays.  Thus the number of (array) elements of arrays must
// equal the number of elements of p_p_int.  Execution time is linear in this
// number.  After finding the minimum value, the (pointer) element of p_p_int
// that points to it is incremented.  minptrix is designed to be called
// repeatedly by function multimerge, until all elements of p_p_int are after
// the ends of the corresponding (array) elements of arrays.  It returns true
// if not all the pointers were past the ends of their arrays and *pminval
// has been set to a value in an element of arrays; otherwise *pminval is set
// to INT_MAX.

static bool minptrix(int nr_arrays, int lens[nr_arrays], int *arrays[nr_arrays],
                     int **p_p_int, int *pminval) {
  bool  did_examine = false;  // did examine an element of an element of arrays
  int **minit;
  int   minval;
  int **p_int_array = arrays;

  for (int i = 0; i < nr_arrays; ++i, ++p_int_array, ++p_p_int) {
    if (*p_p_int - *p_int_array >= lens[i]) {
      continue;
    }

    int curval = **p_p_int;

    if ((!did_examine) || minval > curval) {
      did_examine = true;
      minval      = curval;
      minit       = p_p_int;
    }
  }

  if (did_examine) {
    *pminval = minval;
    ++(*minit);
  } else {
    *pminval = INT_MAX;
  }

  return did_examine;
}

// Multimerge, linear in k.

bool multimerge(int nr_arrays, int lens[nr_arrays], int *arrays[nr_arrays],
                int total_nr, int output[total_nr]) {
  int **p_int_array = arrays;
  int **array_int_p = (int **) malloc(nr_arrays * sizeof (int *));
  if (array_int_p == NULL)
    return false;
  int **p_p_int = array_int_p;
  
  for (int i = 0; i < nr_arrays; ++i, ++p_int_array, ++p_p_int)
    *p_p_int = *p_int_array;
  
  int minval;
  while (minptrix(nr_arrays, lens, arrays, array_int_p, &minval))
    *output++ = minval;
  
  free (array_int_p);
  return true;
}

// Priority queue multimerge, logarithmic in k.

bool multimerge_pq(int nr_arrays, int lens[nr_arrays], int *arrays[nr_arrays],
                   int total_nr, int output[total_nr]) {
  int **p_int_array = arrays;
  int **array_int_p = (int **) malloc(nr_arrays * sizeof (int *));
  if (array_int_p == NULL)
    return false;
  int **p_p_int = array_int_p;
  
  IntPriorityQueue *ppq = priority_queue_alloc (nr_arrays);  
  if (ppq == NULL) {
    free (array_int_p);
    return false;
  }
  
  PointerPointerPair ppp;
  
  for (int i = 0; i < nr_arrays; ++i, ++p_int_array, ++p_p_int) {
    *p_p_int = *p_int_array;
    ppp.p_int_array = p_int_array;
    ppp.p_p_int     = p_p_int;
    priority_queue_push (ppq, ppp);
  }
  
  int minval;
  PointerPointerPair *pppp;

  for (int i = 0; i < total_nr; ++i) {
    pppp = priority_queue_top (ppq);
    if (pppp == NULL) {  // should't occur
      free (array_int_p);
      return false;
    }
    ppp = *pppp;
    priority_queue_pop (ppq);
    minval = **(ppp.p_p_int);
    ++(*(ppp.p_p_int));
    if (*(ppp.p_p_int) - *(ppp.p_int_array) < lens[ppp.p_int_array - arrays]) {
      priority_queue_push (ppq, ppp);
    }
    *output = minval;
    output++;
  }

  priority_queue_free (ppq);
  free (array_int_p);
  return true;
}
