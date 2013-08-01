erlang/README.md rev. 31 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Erlang

mmerge.erl provides two merge methods for a list of sorted lists, output one
sorted list.  testmmerge.erl provides several functions to test correctness
and timing.  heaps.erl is from https://gist.github.com/larsmans/1248317,
under LGPL 2.1 license; it is only required to link to its compiled code. 
getopt.erl and getopt.app.src are from https://github.com/jcomellas/getopt
under the new BSD license.  Tested with erlang 15B01 under lubuntu 12.10,
intel processor, 8 GB RAM.

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

make all runs the timing tests, analysis, and builds the edoc.
make clean deletes the results of all that.  Testing, not extensive,
was done with GNU Make 3.81.

## More to come is planned for this sample.

For one thing, the priority queue method runtime that for the other languages
is logarithmic in k, the number of arrays to merge, seems to be linear in k
for the Erlang version.  Hopefully it might be possible to improve this.  Also
the method that for other languages is simpler (and linear in k) is not so
simple, and is pretty slow, in Erlang (though apparently still linear in k).
Maybe this could be improved too.

Also, using processes as iterators to the lists to merge makes it seem
natural to extend to multiple computers.