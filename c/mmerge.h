// c/mmerge.h rev. 22 July 2013 by Stuart Ambler.  Header file for c/mmerge.c.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Merge of k sorted arrays of ints, given in the form of an array of pointers
// to arrays, using either a simple algorithm linear in k, the number of
// arrays, or instead an algorithm that uses a priority queue, logarithmic in k.
// Either way, the dependence on n, the total length of all the vectors, is
// linear.  Minimal attention given to error checking.

#ifndef _HOME_STUART_PROJECTS_SAMPLES_C_MMERGE_H_
#define _HOME_STUART_PROJECTS_SAMPLES_C_MMERGE_H_

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Multimerge, linear in k.  Each element of arrays must be a sorted
// vector of int.  On return, *poutput will be a sorted vector containing
// all the values in all the elements of arrays.  Returns false if error.

bool multimerge(int nr_arrays, int lens[nr_arrays], int *arrays[nr_arrays],
                int total_nr, int output[total_nr]);

// Priority queue multimerge, logarithmic in k. Each element of arrays must be
// a sorted vector of int.  On return, *poutput will be a sorted vector
// containing all the values in all the elements of arrays.  Returns false if
// error.

bool multimerge_pq(int nr_arrays, int lens[nr_arrays], int *arrays[nr_arrays],
                   int total_nr, int  output[total_nr]);

#endif  // _HOME_STUART_PROJECTS_SAMPLES_C_MMERGE_H_
