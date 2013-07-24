common/README.md rev. 23 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Samples Common Files

runtest.py runs a set of timing tests for a given language

commonanalyze.R, which calls analyze.R, analyzes the results for a given
language (see individual language README.md files for details).

compare.R compares the results for a given language with those for C.

make all runs compare.R for all non-C languages; make clean deletes the
results.    Testing, not extensive, was done with GNU Make 3.81.

Tested with Python 2.7.3 and R 2.15.1.