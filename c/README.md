c/README.md rev. 31 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in C

mmerge.h and mmerge.c provide two merge methods for an array of sorted arrays,
output one sorted array.  pqueue.h and pqueue.c implement a min heap priority
queue for use by mmerge.c.  testmmerge.c tests correctness of results and times
the merge.  Compiled with gcc 4.7.2 under lubuntu 12.10, intel processor, 8 GB
RAM.  Requires installation of argtable2, tested with version 12-1.  Ported
from the C++ equivalent, with the addition of a priority queue implementation.

To test, use scripts in the common subdirectory, which is on the same level as
the c directory containing this file: from the directory containing this file,

../common/runtests.py ./testmmergemain >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

testdata.txt and the second two lines in Rout.txt will contain data such as that
in timing.txt.  The graphs in pdf files will show the relation of actual data to
fitted formulas.

testmmergemain has a main function that calls testmmerge_main.
checktestmmerge defines a check testing framework test; it too has a main
function, that via check (version 0.9.8-2 installed) calls testmmerge_main.

To run runvalgrind, install valgrind 3.7.0

make all builds the executables, runs the timing tests, analysis, and
valgrind.  make clean deletes the results of all that.  Testing, not
extensive, was done with GNU Make 3.81.