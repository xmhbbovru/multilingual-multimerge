#!/usr/bin/env python
""" Functions to merge sorted lists of ints.
python/mmerge.py rev. 22 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

Input is in the form of a list of k lists, and the merge uses either a simple
algorithm linear in k, or an algorithm that uses a priority queue and is
logarithmic in k.  Either way, the dependence on n, the total length of all the
lists, is linear.  Not much exception or other error handling.
"""
import heapq
import sys


class MMergeLinear(object):
    """ Class to support linear k-way merge.
    """
    def __init__(self, gens):
        """ Initialize instance variables.
        Args: self, list of generators (used to call minptrix).
        """
        self.cache = [None] * len(gens)
        self.gen_empty = [False] * len(gens)
        i = 0
        for gen in gens:
            try:
                self.cache[i] = gen.next ()
            except StopIteration as exc:
                self.gen_empty[i]  = True
            i = i + 1
        self.cache_min_index = None
        self.cache_min = None

    def minptrix(self, gens):
        """Finds minimum value for any of a list of generators,
        Args: self, list of generators (side-effect: are called) of int.
        Returns: bool minimum value valid, minimum value.
        Execution time is linear in the total number of elements the
        generators return.  In finding the minimum value, next is called.
        minptrix is designed to be called repeatedly, with the same list of
        generators, by method multimerge, until calling next has raised a
        StopIteration exception for all the generators.
        """
        # Since the generators don't have a peek function, the instance variable
        # cache contains values obtained by calling next on each generator, or
        # None if a StopIteration exception was raised by the call.  Instance
        # variable cache_min_index is the index of the generator from which the
        # last (candidate) minimum value for this call of minptrix was obtained,
        # and cache_min is that value.

        did_examine = False  # looked at a value (so that minval is valid)
        minval = sys.maxint
        i = 0
        for gen in gens:
            curval = self.cache[i]
            if curval is None:
                if self.gen_empty[i]:
                    i = i + 1
                    continue
                else:
                    try:
                        self.cache[i] = gen.next()
                    except StopIteration as exc:
                        self.gen_empty[i] = True
                        i = i + 1
                        continue
                curval = self.cache[i]
            did_examine = True
            
            if minval > curval:
                self.cache[i] = None  # consume the value
                if self.cache_min_index is not None:  # unconsume if possible
                    self.cache[self.cache_min_index] = self.cache_min
                self.cache_min_index = i  # store for possible unconsumption
                self.cache_min = curval
                minval = curval
            
            i = i + 1

        self.cache_min_index = None  # keep consumed the minval for this call
        return did_examine, minval
    

def list_gen(a_list):
    """ Generates elements of a list.
    Args: a list.
    Returns: nothing.
    It would be possible to alter the code to consume output of an array of
    generators, for example producing values from a file, rather than a list
    of lists.
    """
    for element in a_list:
        yield element


def multimerge(lists):
    """ Multimerge, linear in k, the number of lists.
    Args: list of lists.
    Returns: a list.
    Each element of list must be a sorted list of int.  On return, output
    will be a sorted list containing all the values in all the elements of
    lists.
    """
    output = []
    gens = []

    for a_list in lists:
        gens.append (list_gen(a_list))
    mmerge = MMergeLinear(gens)
    minval_valid, minval = mmerge.minptrix(gens)
    while minval_valid:
        # if wanted a generator, could yield here instead of building output
        output.append(minval)
        minval_valid, minval = mmerge.minptrix(gens)
    return output


def multimerge_pq(lists):
    """ Multimerge using priority queue.
    Args: list of lists.
    Returns: a list.
    Each element of list must be a sorted list of int.  On return, output
    will be a sorted list containing all the values in all the elements of
    lists.
    """
    output = []
    hpq = []
    gens = []

    for a_list in lists:
        gens.append (list_gen(a_list))

    for gen in gens:
        try:
            curval = gen.next()
            if curval is not None:
                heapq.heappush(hpq, (curval, gen))
        except StopIteration as exc:
            pass

    while hpq:
        (curval, gen) = heapq.heappop(hpq)
        # if wanted a generator, could yield here instead of building output
        output.append(curval)
        try:
            curval = gen.next()
            if curval is not None:
                heapq.heappush(hpq, (curval, gen))
        except StopIteration as exc:
            pass
    
    return output


def main():
    """ mmerge main to test k-way merges; simply calls testmmerge.main().
    Args: none
    Returns: exit code
    """
    import testmmerge
    return testmmerge.main_func(sys.argv)


if __name__ == "__main__":
    main ()
