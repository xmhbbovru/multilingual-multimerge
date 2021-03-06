cc/README.md rev. 31 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in C++

mmerge.h and mmerge.cc provide two merge methods for an STL vector of
sorted vectors, output one sorted vector.  testmmerge.cc tests correctness
of results and times the merge.  Compiled with g++ 4.7.2 under lubuntu
12.10, intel processor, 8 GB RAM.  Requires installation of argtable2,
tested with version 12-1.

Original intent was to make a more solid version of a program written for a
phone tech interview question; adapted to fit the Google C++ style guide to a
fair degree, for a code sample.

To test, use scripts in the common subdirectory, which is on the same level as
the cc directory containing this file: from the directory containing this file,

../common/runtests.py ./testmmergemain >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

testdata.txt and the second two lines in Rout.txt will contain data such as that
in timing.txt.  The graphs in pdf files will show the relation of actual data to
fitted formulas.

testmmergemain has a main function that calls testmmerge_main.
cppunittestmmerge defines a CppUnit test, and it too has a main function,
that via CppUnit (version 1.12.1 installed) calls testmmerge_main with default
(no) args.

To run runvalgrind, install valgrind 3.7.0.

make all builds the executables, runs the timing tests, analysis, and
valgrind.  make clean deletes the results of all that.  Testing, not
extensive, was done with GNU Make 3.81.

Google's C++ style guide is available at
http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
which gives a link to download its cpplint.py.
