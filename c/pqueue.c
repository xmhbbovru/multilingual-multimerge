// c/pqueue.c rev. 22 July 2013 by Stuart Ambler.
// Priority queue implementation; see c/pqueue.h for further notes.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

#include "./pqueue.h"

// Assumes index > 1.

static inline int compare_value (IntPriorityQueue *ppq, int index) {
  return **((ppq->array[index]).p_p_int);
}

static inline int parent_index (int index) {
  return index / 2;
}

static inline int left_child_index (int index) {
  return 2 * index;
}

static inline void swap (IntPriorityQueue *ppq, int index_0, int index_1) {
  PointerPointerPair temp = ppq->array[index_0];
  ppq->array[index_0] = ppq->array[index_1];
  ppq->array[index_1] = temp;
}

static inline void copy (IntPriorityQueue *ppq, int src_index, int dest_index) {
  ppq->array[dest_index] = ppq->array[src_index];
}

IntPriorityQueue *priority_queue_alloc(int size) {
  if (size < 1)
    return NULL;
 
  IntPriorityQueue *ppq = (IntPriorityQueue *) malloc(sizeof(IntPriorityQueue));
  if (ppq == NULL)
    return NULL;
  
  ppq->size      = size;
  ppq->allocated = (int) exp2(ceil(log2((double) size))) + 1;  // index 0 unused
  ppq->occupied  = 0;

  ppq->array = (PointerPointerPair *) malloc(  ppq->allocated
                                             * sizeof(PointerPointerPair));
  if (ppq->array == NULL) {
    free (ppq);
    return NULL;
  }

  return ppq;
}

void priority_queue_push(IntPriorityQueue  *ppq,
                         PointerPointerPair ppp) {
  if (ppq->occupied >= ppq->size)
    return;

  int j;
  int k = ++ppq->occupied;
  ppq->array[k] = ppp;
  while (k > 1) {
    j = parent_index(k);
    if (compare_value (ppq, k) < compare_value (ppq, j)) {
      swap (ppq, j, k);
      k = j;
    }
    else {
      break;
    }
  }
}

PointerPointerPair *priority_queue_top(IntPriorityQueue *ppq) {
  if (ppq == NULL || ppq->occupied <= 0)
    return NULL;

  return &ppq->array[1];
}

// Algorithm adapted from Algorithms, Fourth Edition, by Sedgewick and Wayne,
// Pearson, 2011, pp. 316, 318.

void priority_queue_pop(IntPriorityQueue *ppq) {
  if (ppq == NULL || ppq->occupied <= 0)
    return;

  if (ppq->occupied == 1) {
    ppq->occupied = 0;
    return;
  }

  copy (ppq, ppq->occupied, 1);
  --ppq->occupied;

  int j;
  int k = 1;
  while ((j = left_child_index(k)) <= ppq->occupied) {
    if (   j < ppq->occupied
        && compare_value (ppq, j + 1) < compare_value (ppq, j))
      ++j;
    if (compare_value (ppq, k) <= compare_value (ppq, j))
      break;
    swap (ppq, k, j);
    k = j;
  }
}  

void priority_queue_free(IntPriorityQueue *ppq) {
  if (ppq != NULL) {
    free (ppq->array);
    free (ppq);
  }
}
