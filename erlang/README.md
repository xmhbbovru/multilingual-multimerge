erlang/README.md rev. 04 August 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Erlang

mmerge.erl provides two merge methods for a list of sorted lists, output one
sorted list.  testmmerge.erl provides several functions to test correctness
and timing.  getopt.erl and getopt.app.src are from
https://github.com/jcomellas/getopt under the new BSD license.  For the
priority queue, it's required to use the .beam file from compiling
skewbinheap.erl, from https://github.com/okeuday/skewbinheap ; I found no
license so am not including this here.  Tested with erlang 15B01 under lubuntu
12.10, intel processor, 8 GB RAM.

An earlier version used the pairing heap in heaps.erl from
https://gist.github.com/larsmans/1248317, under LGPL 2.1 license, but it was
slower; when using it, the delete_min runtime was apparently linear in k,
rather than logarithmic as desired.

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

For one thing, the method that for other languages is simpler (and linear in k)
is not so simple, and is pretty slow, in Erlang (though apparently still linear
in k).  For another, the generation of test data is slow.  Maybe these things
could be improved.

Also, using processes as iterators to the lists to merge makes it seem
natural to extend to multiple computers.