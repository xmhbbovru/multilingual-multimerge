// java/MMerge.java rev. 22 July 2013 by Stuart Ambler.
// Merge of sorted arrays of ints.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Merge of k sorted arrays of ints, given in the form of an array of arrays,
// using either a simple algorithm linear in k, or an algorithm logarithmic in
// k, using a priority queue.  Either way, the dependence on n, the total
// length of all the vectors, is linear.  No exception or other error handling,
// nor much attention paid to information hiding; not made for library use.

package com.zulazon.samples;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ListIterator;
import java.util.PriorityQueue;

/**
 * Contains methods to merge sorted arrays of ints.
 */
public class MMerge {

 /**
  * Used by minptrix for cache elements.
  */
  class ValueValidPair {
    ValueValidPair (int value, boolean valid) {
      this.value = value;
      this.valid = valid;
    }
    int     value;  // cached value
    boolean valid;  // value valid (obtained from next but not consumed).
  }

  // Following two objects used by minptrix, no need to incur creation cost
  // on every call to that function.  During a single call to minptrix,
  // cacheObject holds the cache ValueValidPair for each element of its,
  // in turn.  minCacheObject holds the ValueValidPair for cache for the most
  // recently found minimum value found during a single call to minptrix.
  // Their persistence across calls to minptrix is not used.
  
  ValueValidPair cacheObject    = new ValueValidPair(Integer.MAX_VALUE, false);
  ValueValidPair minCacheObject = new ValueValidPair(Integer.MAX_VALUE, false);

  // minptrix is a function to find the minimum value pointed to by any of an
  // array its of iterators to arrays of ints.  Execution time is linear in
  // the total number of elements the iterators range over.  In finding the
  // minimum value, the corresponding iterator is incremented.  minptrix is
  // designed to be called repeatedly by function multimerge, until all the
  // iterators return false from hasNext.  It returns true if not all the
  // iterators returned false and minval[0] has been set to a value in an
  // element of arrays; otherwise minval[0] is set to Integer.MAX_VALUE.

  // Since java iterators don't have a peek function, the parameter cache
  // contains values obtained by calling next on the iterators in its,
  // cacheValid telling whether the cache contains such a value or not.
  // Elements of each index correspond, for the two ArrayList arguments,
  // so they are iterated in sync.

 /**
  * Finds minimum value pointed to by any of an array of iterators.
  */
  boolean minptrix(ArrayList<ListIterator<Integer> > its,
                   ArrayList<ValueValidPair> cache,
                   int minval[]) {
    boolean didExamine  = false;  // examined an element of an element of arrays
    int localMinval = Integer.MAX_VALUE;
    ListIterator<ValueValidPair> cacheIt = cache.listIterator();

    for (ListIterator<Integer> it : its) {
      cacheObject = cacheIt.next();
      
      if (!(it.hasNext() || cacheObject.valid))
        continue;

      int curval = cacheObject.value;

      if (!cacheObject.valid) {
        curval = it.next();
        cacheObject.value = curval;
        cacheObject.valid = true;
      }

      // This code works even if the same iterator supplies consecutive minima
      if ((!didExamine) || localMinval > curval) {
        if (didExamine)                 // minCacheObject has been set
          minCacheObject.valid = true;  // unconsume it
        cacheObject.valid = false;      // consume where the minimum came from
        minCacheObject = cacheObject;   // save to unconsume if find smaller
        didExamine = true;
        localMinval = curval;
      }
    }

    minval[0] = localMinval;

    return didExamine;
  }
  
  // Multimerge, linear in k.  Each element of arrays must be a sorted
  // array of int.  On return, output will be a sorted array containing
  // all the values in all the elements of arrays.

 /**
  * Multimerge with timing linear in the number of arrays to merge.
  * @param  arrays  an array of sorted arrays to merge
  * @return         one array with the results of the merge
  */
  public ArrayList<Integer> multimerge(ArrayList<ArrayList<Integer> > arrays) {
    int nrArrays = arrays.size();
    ArrayList<ListIterator<Integer> > its
                         = new ArrayList<ListIterator<Integer> >(nrArrays);
    ArrayList<ValueValidPair> cache = new ArrayList<ValueValidPair>(nrArrays);
    int totalNr = 0;
    
    ArrayList<Integer> output = new ArrayList<Integer>(totalNr);

    for (ListIterator<ArrayList<Integer> > ia = arrays.listIterator();
         ia.hasNext();) {
      ArrayList<Integer> array = ia.next();
      its.add(array.listIterator());
      cache.add(new ValueValidPair(Integer.MAX_VALUE, false));
      totalNr += array.size();
    }

    int minval[] = { Integer.MAX_VALUE };  // An array so minptrix can change it
    while (minptrix(its, cache, minval)) {
      output.add(minval[0]);
    }

    return output;
  }

  class IteratorValuePair implements Comparable<IteratorValuePair> {
    IteratorValuePair (ListIterator<Integer> it, int val) {
      this.it  = it;
      this.val = val;
    }
    public int compareTo (IteratorValuePair p) {
      if (val < p.val)
        return -1;
      else if (val == p.val)
        return 0;
      else
        return 1;
    }
    
    ListIterator<Integer> it;
    int                   val;
  }
  
  // Priority queue multimerge, logarithmic in k. Each element of arrays must
  // be a sorted array of int.  Returns a sorted array containing all the
  // valuesin all the elements of arrays.

 /**
  * Multimerge using a priority queue.
  * @param  arrays  an array of sorted arrays to merge
  * @return         one array with the results of the merge
  */
  public ArrayList<Integer> multimergePQ(ArrayList<ArrayList<Integer>> arrays) {
    PriorityQueue<IteratorValuePair> pq
                         = new PriorityQueue<IteratorValuePair> (arrays.size());
    int totalNr = 0;
    
    for (ListIterator<ArrayList<Integer> > ia = arrays.listIterator();
         ia.hasNext();) {
      ArrayList<Integer> array = ia.next();
      ListIterator<Integer> it = array.listIterator();
      if (it.hasNext())
        pq.add(new IteratorValuePair(it, it.next()));
      totalNr += array.size();
    }
    
    ArrayList<Integer> output = new ArrayList<Integer>(totalNr);

    while (!pq.isEmpty()) {
      IteratorValuePair p = pq.remove();
      output.add(p.val);
      if (p.it.hasNext()) {
        p.val = p.it.next();
        pq.add(p);
      }
    }

    return output;
  }
}