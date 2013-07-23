// c/pqueue.h rev. 22 July 2013 by Stuart Ambler.  Header for c/pqueue.c.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Priority queue as heap, minimum at top.  Elements are structs designed to be
// useful in c/mmerge.c implementation, and are sorted by **p_p_int.

#ifndef _HOME_STUART_PROJECTS_SAMPLES_C_PQUEUE_H_
#define _HOME_STUART_PROJECTS_SAMPLES_C_PQUEUE_H_

#include <limits.h>
#include <math.h>
#include <stdlib.h>

struct PointerPointerPair_ {
  int **p_int_array;  //  *p_int_array is an array of int
  int **p_p_int;      // **p_p_int must be an element of *p_int_array
};
typedef struct PointerPointerPair_ PointerPointerPair;

struct IntPriorityQueue_ {
  int allocated;
  int size;
  int occupied;
  PointerPointerPair *array;
};
typedef struct IntPriorityQueue_ IntPriorityQueue;

// Returns NULL if unable to allocate.
IntPriorityQueue   *priority_queue_alloc (int size);
// Does nothing if the push would cause size to be exceeded.
void                priority_queue_push  (IntPriorityQueue  *ppq,
                                          PointerPointerPair ppp);
// Returns NULL if the priority queue is empty.
PointerPointerPair *priority_queue_top   (IntPriorityQueue  *ppq);
// Does nothing if the priority queue is empty.
void                priority_queue_pop   (IntPriorityQueue  *ppq);
void                priority_queue_free  (IntPriorityQueue  *ppq);

#endif  // _HOME_STUART_PROJECTS_SAMPLES_C_PQUEUE_H_
