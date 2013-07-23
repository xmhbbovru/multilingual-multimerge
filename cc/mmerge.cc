// cc/mmerge.cc rev. 22 July 2013 by Stuart Ambler.
// Merge of k sorted arrays of ints.  See cc/mmerge.h for further comments.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

#include "./mmerge.h"

namespace com_zulazon_samples_cc_mmerge {

// Typedefs for both methods of merge.

typedef std::vector<IntVectorConstIterator> IntVectorConstIteratorVector;
typedef std::vector<IntVectorConstIterator>::iterator
                                           IntVectorConstIteratorVectorIterator;

// Classes and typedef only for the priority queue method.

class IteratorPointerPair {
 public:
  // The second argument to the first constructor could be a const ptr for
  // that constructor, but later elsewhere is used as a plain ptr.
  IteratorPointerPair(const IntVectorVectorConstIterator &i,
                      IntVectorConstIterator *p) : it_to_vec_(i),
                                                   ptr_const_it_(p) {}
  // To comply with Google style guidelines, this code should define a
  // copy constructor and assignment operator, also getter functions to
  // use in place of direct access to fields, which should be private.
  // However, experiment showed that these things cost about an 18% increase
  // in execution time, so they're commented out as follows, and uses in
  // multimerge_pq of the getters are replaced with direct access.
  // IteratorPointerPair(const IteratorPointerPair &ipp)
  //                                          : it_to_vec_(ipp.it_to_vec_),
  //                                         ptr_const_it_(ipp.ptr_const_it_) {}
  // IteratorPointerPair & operator= (const IteratorPointerPair &ipp) {
  //   it_to_vec_    = ipp.it_to_vec_;
  //   ptr_const_it_ = ipp.ptr_const_it_;
  // }
  //  IntVectorVectorConstIterator &it_to_vec() {
  //    return it_to_vec_;
  //  }
  //  IntVectorConstIterator *ptr_const_it() {
  //    return ptr_const_it_;
  //  }

  // private:
  //  friend class IteratorPointerPairReverseCompare;
  IntVectorVectorConstIterator it_to_vec_;  //  *it_to_vec_ is a vector
  IntVectorConstIterator *ptr_const_it_;    // **ptr_const_it_ must be an
                                            // element of *it_to_vec
};

class IteratorPointerPairReverseCompare {
 public:
  // As in the IteratorPointPair comment, the following are commented out,
  // though the difference in execution time measured was 1%, which may have
  // been normal variation.
  // IteratorPointerPairReverseCompare() = default;
  // IteratorPointerPairReverseCompare
  //                                const IteratorPointerPairReverseCompare &) {
  // }
  // IteratorPointerPairReverseCompare & operator= (
  //                        const IteratorPointerPairReverseCompare &) = delete;

  bool operator()(const IteratorPointerPair &it0,
                  const IteratorPointerPair &it1) const {
    return (**(it0.ptr_const_it_) > **(it1.ptr_const_it_));
  }
};

typedef std::priority_queue<IteratorPointerPair,
                            std::vector<IteratorPointerPair>,
                            IteratorPointerPairReverseCompare> IntPriorityQueue;

// In C rather than STL terms (i.e. loosely speaking), minptrix is a function
// to find the minimum value pointed to by an array pvcits of pointers
// that point to elements of the corresponding (vector) elements of arrays.
// Thus the number of (vector) elements of arrays must equal the number of
// elements of pvcits.  Execution time is linear in this number.  After
// finding the minimum value, the (iterator) element of pvcits that points
// to it is incremented.  minptrix is designed to be called repeatedly by
// function multimerge, until all elements of pvcits are at the ends of the
// corresponding (vector) elements of arrays.  It returns true if not all the
// pointers were at the ends (past the ends) of their vectors and *pminval
// has been set to a value in an element of arrays; otherwise *pminval is set
// to INT_MAX.

static bool minptrix(const IntVectorVector   &arrays,
                     IntVectorConstIteratorVector *pvcits,
                     int *pminval) {
  bool did_examine = false;  // did examine an element of an element of arrays
  IntVectorConstIteratorVectorIterator minit;
  int  minval;

  IntVectorVectorConstIterator                ia = arrays.begin();
  for (IntVectorConstIteratorVectorIterator iits = pvcits->begin();
       iits != pvcits->end(); ++iits, ++ia) {
    if (*iits == ia->end()) {
      continue;
    }

    int curval = **iits;

    if ((!did_examine) || minval > curval) {
      did_examine = true;
      minval      = curval;
      minit       = iits;
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

void multimerge(const IntVectorVector &arrays, IntVector *poutput) {
  IntVectorConstIteratorVector its;
  int total_nr = 0;

  its.reserve(arrays.size());
  for (IntVectorVectorConstIterator ia = arrays.begin();
       ia != arrays.end(); ++ia) {
    its.push_back(ia->begin());
    total_nr += ia->size();
  }

  poutput->clear();
  poutput->reserve(total_nr);

  int minval;
  while (minptrix(arrays, &its, &minval)) {
    poutput->push_back(minval);
  }
}

// Priority queue multimerge, logarithmic in k.

void multimerge_pq(const IntVectorVector &arrays, IntVector *poutput) {
  IntVectorVectorConstIterator ia;
  IntVectorConstIteratorVector its;
  IntPriorityQueue pq;
  int total_nr = 0;

  its.reserve(arrays.size());
  for (ia = arrays.begin(); (ia != arrays.end()); ++ia) {
    its.push_back(ia->begin());
    pq.push(IteratorPointerPair(ia, &(its.back())));
    total_nr += ia->size();
  }
  poutput->clear();
  poutput->reserve(total_nr);

  IntVectorVectorConstIterator  it_to_vec;
  IntVectorConstIterator       *ptr_const_it = nullptr;
  IteratorPointerPair  it_pval(it_to_vec, ptr_const_it);
  int minval;

  for (int i = 0; i < total_nr; ++i) {
    it_pval = pq.top();
    pq.pop();
    minval = **(it_pval.ptr_const_it_);
    ++(*(it_pval.ptr_const_it_));
    if (*(it_pval.ptr_const_it_) != it_pval.it_to_vec_->end()) {
      pq.push(IteratorPointerPair(it_pval));
    }
    poutput->push_back(minval);
  }
}

}  // namespace com_zulazon_samples_cc_mmerge
