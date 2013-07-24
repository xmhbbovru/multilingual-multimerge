README.md rev. 23 July 2013 by Stuart Ambler.
Copyright (c) 2013 Stuart Ambler.
Distributed under the Boost License in the accompanying file LICENSE.

# Multimerge Code Samples in C, C++, Java, Python, and Ruby

I wrote this code first in C as an answer to a tech interview question about how
to merge k sorted arrays of ints into one sorted array.  It used the most
straightforward method, which is called linear here since it takes time linear
in k (times n, the total number of ints).  Then I rewrote it in C++ to use
an STL priority queue, making the time logarithmic in k.

When a helpful hiring manager in a Bay Area software company suggested that it
would be good to make some code samples publicly available on GitHub, this
code suggested itself to me.  About everything significant I've written has been
proprietary so I wouldn't want to publish it, but I have no attachment to this.
Whether looking at this would be useful to anyone other than as a code sample 
is a question, but if it is, that's great.

The intent is that each language version should be idiomatic.  In some cases
I was guided by style guides and/or used style checker or lint programs.  Each
version has a way to run timing tests with command line parameters for k and
the average array length desired.  There's also a way to run a timing test, for
most languages just with the default parameters, via a commonly used testing
framework for the language.  The code uses command line argument parsing
libraries, and various priority queue implementations, except in C, where one
is built from scratch.

There are Python and R scripts usable in common for all the language versions
to gather tables of timing data, analyze and plot, to see how fast the
various versions run and how well the fitted formulas for time match the data.

make all runs make all for all the individual languages; likewise for
make clean.  Testing, not extensive, was done with GNU Make 3.81.

Tested on an IBM Thinkpad SL510, 8 GB RAM, 64 bit Lubuntu 12.10.

I'd be happy to hear from you.  My [website](http://www.zulazon.com) has a
contact form.
