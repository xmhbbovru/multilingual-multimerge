// cc/mmerge.h rev. 22 July 2013 by Stuart Ambler.  Header for cc/mmerge.cc.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Merge of k sorted arrays of ints, given in the form of a vector of vectors,
// using either a simple algorithm linear in k, the number of vectors, or
// instead an algorithm that uses a priority queue, logarithmic in k.  Either
// way, the dependence on n, the total length of all the vectors, is linear.
// Minimal exception or other error handling.

#ifndef _HOME_STUART_PROJECTS_SAMPLES_CC_MMERGE_H_
#define _HOME_STUART_PROJECTS_SAMPLES_CC_MMERGE_H_

#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <queue>
#include <string>
#include <vector>

namespace com_zulazon_samples_cc_mmerge {

// Typedefs for both methods of merge; linear and priority queue.

typedef std::vector<int>                 IntVector;
typedef std::vector<int>::iterator       IntVectorIterator;
typedef std::vector<int>::const_iterator IntVectorConstIterator;

typedef std::vector<IntVector>                 IntVectorVector;
typedef std::vector<IntVector>::const_iterator IntVectorVectorConstIterator;

// Multimerge, linear in k.  Each element of arrays must be a sorted
// vector of int.  On return, *poutput will be a sorted vector containing
// all the values in all the elements of arrays.

void multimerge(const IntVectorVector &arrays, IntVector *poutput);

// Priority queue multimerge, logarithmic in k. Each element of arrays must be
// a sorted vector of int.  On return, *poutput will be a sorted vector
// containing all the values in all the elements of arrays.

void multimerge_pq(const IntVectorVector &arrays, IntVector *poutput);

}  // namespace com_zulazon_samples_cc_mmerge

#endif  // _HOME_STUART_PROJECTS_SAMPLES_CC_MMERGE_H_
