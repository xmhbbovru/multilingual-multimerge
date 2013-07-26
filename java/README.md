java/README.md rev. 25 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Sample in Java

MMerge.java provides two merge methods for an array of sorted arrays, output
one sorted array.  TestMMerge.java tests correctness of results and times
the merge.  Compiled and test with java 1.7.0_25, OpenJDK Runtime Environment
(IcedTea 2.3.10) (7u25-2.3.10-0ubunto0.12.10.2), OpenJDK 64-Bit Server VM build
23.7-b01, mixed mode, under under lubuntu 12.10, intel processor, 8 GB RAM.
Requires installation of jopt-simple (see below).

To build, first ensure that jopt-simple-4.5.jar is in /usr/share/java; or if
it's somewhere else, edit runmmerge to put the new location in the javac
command classpath (cp) option.

To run, first ensure that jopt-simple-4.5.jar is in /usr/share/java; or if it's
somewhere else, edit runmmerge to put the new location in the java command
classpath (cp) option.

To test, use scripts in the common subdirectory, which is on the same level as
the java directory containing this file.  From the directory containing this
file,

../common/runtests.py ./runmmerge >testdata.txt
Rscript ../common/commonanalyze.R >Rout.txt

JUnit4 can be used to run TestMMerge with default parameters (blank
command line) via ./runjunittest   Version 4.10 of JUnit was installed.

testdata.txt and the second two lines in Rout.txt contain data that with
slight reformatting, makes the time table in the comments in testmmerge.java.
The graphs in pdf files show the relation of actual data to fitted formulas.

make all builds the class files, runs the timing tests, analysis, and
builds the javadoc.  make clean deletes the results of all that.  Testing,
not extensive, was done with GNU Make 3.81.