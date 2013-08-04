erlang/README.md rev. 03 August 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Erlang

mmerge.erl provides two merge methods for a list of sorted lists, output one
sorted list.  testmmerge.erl provides several functions to test correctness
and timing.  getopt.erl and getopt.app.src are from
https://github.com/jcomellas/getopt under the new BSD license.

heaps.erl is modified from https://gist.github.com/larsmans/1248317, under LGPL
2.1 license; it's only necessary to link to its object code heaps.beam file and
the original version works just as well except slower.  See priority_queue.txt
for a description of the change and reason for it.  The previous GitHub commit
of mmerge used skewbinheap but I could find no license for that and thus didn't
distribute it, plus it was somewhat slower.  Tested with erlang 15B01 under
lubuntu 12.10, intel processor, 8 GB RAM.

To test, use scripts in the common subdirectory, which is on the same level as
the erlang directory containing this file.  From the directory containing this
file,

../common/runtests.py "escript testmmerge.beam" >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

testdata.txt and the second two lines in Rout.txt will contain data such as that
in timing.txt.  The graphs in pdf files will show the relation of actual data to
fitted formulas.

test\_testmmerge.erl uses eunit to run testmmerge:main\_quick\_test, which in
turn gives a command line to testmmerge:main that won't take long to execute.

./runprof.erl uses escript and profiles a testmmerge:testmmerge run to stdout.

make all runs the timing tests, analysis, and builds the edoc.
make clean deletes the results of all that.  Testing, not extensive,
was done with GNU Make 3.81.

## More to come is planned for this sample.

Using processes as iterators to the lists to merge makes it seem
natural to extend to multiple computers.

There are some things that possibly could be made faster.  Possibly gbtrees
in place of heaps could improve the runtime for the priority queue method,
though for the time being I'm satisfied with this.  The method that for other
languages is simpler and linear in k, is not so simple, and is pretty slow
though still linear, in Erlang.  Maybe this could be improved, perhaps by
using gbtrees, though that might not be in the spirit of the simple method
in other languages.  The generation of test data is slow.  In one profile run
examined, two thirds of this time was from lists:sublist.  Perhaps a different
method of data generation would be faster.  However, for now I'm not motivated
to work on these timing issues.

