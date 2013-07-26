ruby/README.md rev. 25 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Ruby

mmerge.rb provides two merge methods for a list of sorted lists, output one
sorted list, and a main function to test correctness of results and time
the merge.  Tested with ruby 1.9.3p194 under lubuntu 12.10, intel processor,
8 GB RAM.  Requires PriorityQueue, tested with version 0.1.2; docopt, tested
with 0.5.0; 

To test, use scripts in the common subdirectory, which is on the same level as
the ruby directory containing this file.  From the directory containing this
file,

../common/runtests.py ./testmmerge.rb >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

RSpec can be used to test testmmerge.rb with default parameters (blank
command line) via ./runrspec    Version 2.14.1 of rspec was installed.

testdata.txt and the second two lines in Rout.txt contain data that with
slight reformatting, makes the time table in the comments in testmmerge.c.
The graphs in pdf files show the relation of actual data to fitted formulas.

make all runs the timing tests, analysis, and builds the rdoc.
make clean deletes the results of all that.  Testing, not extensive,
was done with GNU Make 3.81.