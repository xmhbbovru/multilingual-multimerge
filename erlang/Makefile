# erlang/Makefile rev. 31 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
ERLCOMPILE      = erl -compile
EDOC		= erl -noshell -run edoc_run
TIMETEST	= escript testmmerge.beam
TESTTEST	= test_testmmerge.erl
RUNTESTS	= ../common/runtests.py
ANALYZE		= ../common/commonanalyze.R
ANALYSIS	= Rout.txt lin11.pdf lin12.pdf pq11.pdf pq17.pdf
ERLSRC          = list_iter.erl mmerge.erl testmmerge.erl \
		  getopt.erl getopt.app.src heaps.erl
ERLSRCLIST      = '["list_iter.erl", "mmerge.erl", "testmmerge.erl"]'
ERLBEAM         = list_iter.beam mmerge.beam testmmerge.beam \
		  getopt.beam heaps.beam
ALLBUTDOC	= $(ERLBEAM) testdata.txt $(ANALYSIS)
ERLDOCDIR       = '[{dir,"./doc"}]'

.PHONY:		all
all:		$(ALLBUTDOC) doc

testdata.txt:	$(ERLBEAM)
		$(RUNTESTS) "$(TIMETEST)" >$@

$(ERLBEAM):	$(ERLSRC)

.SUFFIXES:	.erl .beam

.erl.beam:
		$(ERLCOMPILE) $<

$(ANALYSIS):	intermediate-1

.INTERMEDIATE:	intermediate-1
intermediate-1:	testdata.txt
		$(ANALYZE) > Rout.txt

doc:		$(ALLBUTDOC)
		rm -f -r doc; \
		$(EDOC) files $(ERLSRCLIST) $(ERLDOCDIR)

.PHONY:		clean
clean:
		rm -f $(ALLBUTDOC); \
		rm -f -r doc
