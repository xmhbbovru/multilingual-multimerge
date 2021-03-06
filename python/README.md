python/README.md rev. 31 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Python

mmerge.py provides two merge methods for a list of sorted lists, output one
sorted list, and a main function to test correctness of results and time
the merge.  Tested with python 2.7.3 under lubuntu 12.10, intel processor,
8 GB RAM.

To test, use scripts in the common subdirectory, which is on the same level as
the python directory containing this file.  From the directory containing this
file,

../common/runtests.py ./mmerge.py >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

testdata.txt and the second two lines in Rout.txt will contain data such as that
in timing.txt.  The graphs in pdf files will show the relation of actual data to
fitted formulas.

or to use unittest or nose (1.1.2 installed) as selected by code in
testmmerge.py, but not produce testdata.txt,

./testmmerge.py [command line parameters as described by ./mmerge.py -h]

The desire of the test frameworks to consume the command line conflicted with
the convention in these samples to use the command line to choose timing test
parameters.  This accounts for the two ways above to run the tests.

make all runs the timing tests, analysis, and builds the pydoc. 
make clean deletes the results of all that.  Testing, not extensive,
was done with GNU Make 3.81.

Google's Python style guide is available at
http://google-styleguide.googlecode.com/svn/trunk/pyguide.html
Used pychecker 0.8.19 and pylint version 0.28.0.