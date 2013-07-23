#!/usr/bin/env python
""" Runs a set of mmerge.py timing tests.
common/runtests.py rev. 22 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE
"""
import os
import re
import sys

tot_lens_str      = r'(?:tot_lens|totLens) (\d+)'
pq_elapsed_str    = r'(?:pq.+|pq\s+stopwatch end\s+)elapsed.* (\d+\.\d+) sec'
lin_elapsed_str   = r'(?:lin.+|lin\s+stopwatch end\s+)elapsed.* (\d+\.\d+) sec'
heapq_elapsed_str = r'heapq.+elapsed.* (\d+\.\d+) sec'
differ_str        = r'(differ)'
results_reo       = re.compile(         tot_lens_str        # 0
                               + r'|' + pq_elapsed_str      # 1
                               + r'|' + lin_elapsed_str     # 2
                               + r'|' + heapq_elapsed_str   # 3
                               + r'|' + differ_str,         # 4
                               re.IGNORECASE)
python_str = r'\.py$'
python_reo = re.compile(python_str, re.IGNORECASE)
ruby_str   = r'\.rb$'
ruby_reo   = re.compile(ruby_str, re.IGNORECASE)

def run_a_test(cmd, nr_input_arrays, ave_input_len, do_pq, do_lin):
    """ Run a timing test on some language version of the multimerge samples.
    Args: command cmd to run a test, or "header" to print header line,
    nr_input_arrays, ave_input _len as named,
    do_pq should always be True unless want to skip the test altogether,
    do_lin True to set the third, l, argument for the command,
    """
    nr_input_arrays_str = str(nr_input_arrays)
    ave_input_len_str   = str(ave_input_len)
    tot_lens            = "NA"
    pq_elapsed          = "NA"
    lin_elapsed         = "NA"
    heapq_elapsed       = "NA"
    if cmd == "header":
        nr_input_arrays_str = "k"
        ave_input_len_str   = "each"
        tot_lens            = "n"
        pq_elapsed          = "pq"
        lin_elapsed         = "lin"
        heapq_elapsed       = "heapq"
    elif do_pq or do_lin:
        lin_str = "-l"
        p = os.popen("%s %d %d %s" % (cmd, nr_input_arrays, ave_input_len,
                                      lin_str if do_lin else ""), "r")
        results = p.readlines()
        p.close
        match_list = results_reo.findall("".join(results))
        tot_lens = "NA"
        pq_elapsed = "NA"
        lin_elapsed = "NA"
        heapq_elapsed = "NA"
        for match in match_list:
            if match[0]:
                tot_lens = match[0]
            elif match[1]:
                pq_elapsed = match[1]
            elif match[2]:
                lin_elapsed = match[2]
            elif match[3]:
                heapq_elapsed = match[3]
            if match[4]:
                sys.stderr.write("\nerror:\n%s\n" % results)
                sys.stderr.flush()
    print("    %6s %6s %10s %7s %7s %7s" % (nr_input_arrays_str,
                                            ave_input_len_str, tot_lens,
                                            pq_elapsed, lin_elapsed,
                                            heapq_elapsed))


def main ():
    """ main function to run set of mmerge.py timing tests.
    Args: command line argument, the command to run the test executble:
    for c,      ./testmmergemain
    for cc,     ./testmmergemain
    for java,   ./runmmerge
    for python, ./mmerge.py
    for ruby,   ./testmmerge.rb
    Returns: nothing
    """
    cmd = sys.argv[1]
    run_a_test("header", 0, 0, False, False)
    run_a_test(cmd,    10,  10000, True,  True)
    run_a_test(cmd,    20,  10000, True,  True)
    run_a_test(cmd,    30,  10000, True,  True)
    run_a_test(cmd,    40,  10000, True,  True)
    run_a_test(cmd,    50,  10000, True,  True)
    run_a_test(cmd,    60,  10000, True,  True)
    run_a_test(cmd,    70,  10000, True,  True)
    run_a_test(cmd,    80,  10000, True,  True)
    run_a_test(cmd,    90,  10000, True,  True)
    run_a_test(cmd,   100,  10000, True,  True)
    run_a_test(cmd,   160,  10000, True,  True)
    run_a_test(cmd,  1000,  10000, True,  False)
    run_a_test(cmd,   100,  20000, True,  True)
    run_a_test(cmd,   100,  40000, True,  False)
    run_a_test(cmd,   100,  60000, True,  False)
    run_a_test(cmd,   100,  80000, True,  False)
    run_a_test(cmd,   100, 100000, True,  False)

if __name__ == "__main__":
    main ()
