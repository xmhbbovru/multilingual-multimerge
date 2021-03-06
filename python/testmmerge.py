#!/usr/bin/env python
""" Unit and timing test for mmerge.py.
python/testmmerge.py rev. 29 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.
"""
import argparse
import heapq
import math
import random
import sys
import time
import unittest

import mmerge


def get_cfg(args, max_nr_input_ints):
    """ Parses command line arguments.
    Args: args command line arguments, total max_nr_input_ints to merge.
    Returns: nr_inputs number of sorted input arrays to generate,
             ave_input_len average length of such arrays,
             lin_multimerge use linear method in addition to priority queue.
    """
    parser = argparse.ArgumentParser(description="Test k-way merge.")
    parser.add_argument("nr_inputs", action="store", nargs="?",
                        default=1000, type=int,
                        help=("number of sorted input arrays to generate for"
                              " the test; must be positive, and its product"
                              " with ave_input_len no more than 500 million."
                              "  [default 1000]"))
    parser.add_argument("ave_input_len", action="store", nargs="?",
                        default=10000, type=int,
                        help=("desired average length of sorted input arrays"
                              " to generate for the test; must be positive, and"
                              " its product with ave_input_len no more than"
                              " 500 million.  [default 10000]"))
    parser.add_argument("-l", action="store_true", dest="lin_multimerge",
                        help="if present, test the linear method (slower)")
    parsed_args    = parser.parse_args(args[1:])
    nr_inputs      = parsed_args.nr_inputs
    ave_input_len  = parsed_args.ave_input_len
    lin_multimerge = parsed_args.lin_multimerge
    if nr_inputs <= 0 or ave_input_len <= 0:
        print("nr_inputs and ave_input_len must be positive.")
        parser.print_help()
        sys.exit(1)
    if nr_inputs * ave_input_len > max_nr_input_ints:
        print("The product of nr_inputs and ave_input_len, %d, is the total\n"
              "number of ints to merge, which must be no more than %d."
              % (nr_inputs * ave_input_len, max_nr_input_ints))
        parser.print_help()
        sys.exit(1)
    return (nr_inputs, ave_input_len, lin_multimerge)


def verify_small_data():
    """ Test data small enough to verify by hand.
    Args: none.
    Returns: True if merge output Ok
    """
    aint1 = [  2,  6, 88, 688 ]
    aint2 = [  1,  2,  3,   4, 5, 6, 7, 8 ]
    aint3 = [  5, 10, 15,  20 ]
    lists = [ aint1, aint2, aint3 ]
    correct_output = [ 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 10, 15, 20, 88, 688 ]

    retval = True
    output = mmerge.multimerge_pq(lists)
    print("multimerge pq     small data")
    print(output)
    if output != correct_output:
        print("multimerge pq  differs from correct_output")
        retval = False

    output = mmerge.multimerge(lists)
    print("multimerge linear small data")
    print(output)
    if output != correct_output:
        print("multimerge lin differs from correct_output")
        retval = False

    return retval


def calc_display_stats(lens, ave_input_len):
    """ Calculate and print statistics for list of ints.
    Args: lens list of ints, ave_input_len requested average of the ints.
    Returns: total of the ints.
    Variable names and print text assumes that the ints are lengths (lens).
    """
    nr_inputs = len(lens)
    tot_lens = 0
    max_len  = -sys.maxint - 1
    min_len  =  sys.maxint
    for length in lens:
        tot_lens = tot_lens + length
        if max_len < length:
            max_len = length
        if min_len > length:
            min_len = length

    mean_len = tot_lens / nr_inputs
    sum_sq_dev = 0
    for length in lens:
        diff = length - mean_len
        sum_sq_dev = sum_sq_dev + (diff * diff)

    print(  "nr_inputs " + str(nr_inputs)
          + ", requested ave_input_len " + str(ave_input_len))
    print(  "their product " + str(nr_inputs * ave_input_len)
          + ", actual tot_lens " + str(tot_lens))
    print(  "mean_len " + str(mean_len)
          + ", std dev " + str(int(round(math.sqrt(sum_sq_dev / nr_inputs))))
          + ", min_len " + str(min_len)
          + ", max_len " + str(max_len))

    return tot_lens


def generate_data(nr_inputs, ave_input_len):
    """ Generate test data.
    Args: nr_inputs number of sorted input arrays to generate,
          ave_input_len average length of such arrays.
    Returns: inlist_copy list containing expected output,
             lists a list of lists to use as input to the merge functions.
    Generate array of random lengths for input lists uniformly distributed
    from about ave_input_len / 10 to 2 * ave_input_len minus that a range
    centered at ave_input_len.  Display length statistics.
    """
    amin = (ave_input_len + 5) / 10
    if amin < 1:
        amin = 1
    amax = 2 * ave_input_len - amin

    lens = []
    for i in xrange(0, nr_inputs):
        lens.append(random.randint(amin, amax))

    tot_lens = calc_display_stats(lens, ave_input_len)

    # Generate the input data by initializing an overall input list
    # sequentially, shuffling it, piecing it out into the input lists,
    # and sorting each input list.

    inlist = []
    for i in xrange(0, tot_lens):
        inlist.append(i + 1)
    inlist_copy = [element for element in inlist]
    random.shuffle(inlist)

    lists = []
    i = 0
    for length in lens:
        a_list = inlist[i:i + length]
        i = i + length
        a_list.sort()
        lists.append(a_list)

    return (inlist_copy, lists)


def stopwatch(start_time, start, label):
    """ Times intervals.
    Args: start_time is a list to allow modification as well as retrieval,
          bool start True to start timer, False to stop
          label to display.
    Returns: nothing (side effect sets start_time if start).
    For timing merges in actual usage s is nonempty only if not start.
    """
    if start:
        print("%s stopwatch start" % label)
        start_time[0] = time.clock()
    else:
        end_time = time.clock()
        print("%s stopwatch end elapsed %.2f sec" % (label,
                                                     end_time - start_time[0]))


def main_func (args):
    """ Test k-way merge; called by main.
    Args: args, set to sys.argv by main or set directly for testing.
    Returns: nothing.
    """
    max_nr_input_ints = 500000000  # (Ok for 8GB RAM)
    nr_inputs, ave_input_len, lin_multimerge = get_cfg(args, max_nr_input_ints)

    retval = True  # set to False if any errors in merge output

    if not verify_small_data():
        retval = False

    # Do the larger tests.

    (inlist_copy, lists) = generate_data(nr_inputs, ave_input_len)

    start_time = [0]  # a list to provide for setting inside stopwatch

    stopwatch(start_time, True, "multimerge pq")
    output = mmerge.multimerge_pq(lists)
    stopwatch(start_time, False, "multimerge pq")
    mmerge_output_ok = (output == inlist_copy)
    if not mmerge_output_ok:
        retval = False
    print(  "multimerge pq  "
          + ("matches     " if mmerge_output_ok else "differs from")
          + " inlist_copy")

    if lin_multimerge:
        stopwatch(start_time, True, "multimerge lin")
        output = mmerge.multimerge(lists)
        stopwatch(start_time, False, "multimerge lin")
        mmerge_output_ok = (output == inlist_copy)
        if not mmerge_output_ok:
            retval = False
        print(  "multimerge lin "
              + ("matches     " if mmerge_output_ok else "differs from")
              + " inlist_copy")

    # Try heapq.merge.  But don't change retval if its output is bad;
    # the intent is to test mmerge, not heapq.merge.

    stopwatch(start_time, True, "heapq       ")
    outputgen = heapq.merge(*lists)
    output = [e for e in outputgen]
    stopwatch(start_time, False, "heapq       ")
    heapq_merge_output_ok = (output == inlist_copy)
#    if not heapq_merge_output_ok:
#        retval = False
    print(  "heapq       "
          + ("matches     " if heapq_merge_output_ok else "differs from")
          + " inlist_copy")

    return 0 if retval else -1


class TestMMerge(unittest.TestCase):
    """ unittest derivative for test frameworks.
    """
    def setUp(self):
        pass
    def tearDown(self):
        pass    
    def test_overall(self):
        """ To test via test frameworks.
        """
        self.assertEqual(0, main_func(sys.argv))


def main():
    """ testmmerge main to test k-way merges.
    Args: none
    Returns: exit code
    Usage:  python -m testmmerge [args to be processed by main_func]
    or      ./testmmerge.py [args to be processed by main_func]
    """
    # argv=[sys.argv[0] etc.] is used to keep unittest/nose from using the rest
    # of sys.argv.  unittest and nose give about the same results here, except
    # that one can add, e.g. before "testmmerge", "--with-coverage" to the
    # nose argument list, to see lines not covered - but do this only if run
    # one test individually, not if using runtests.py.  Note that README.md
    # says to run mmerge.py, not testmmerge.py, except to use unittest/nose.
    # return unittest.main(argv=[sys.argv[0]])
    import nose
    # return nose.run(argv=[sys.argv[0], "-s", "--with-coverage", "testmmerge"])
    return nose.run(argv=[sys.argv[0], "-s", "testmmerge"])


if __name__ == "__main__":
    main ()
