# ruby/Makefile rev. 24 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
TIMETEST	= testmmerge.rb
TESTTEST	= rspec                   # don't prepend ./
RUNTESTS	= ../common/runtests.py
ANALYZE		= ../common/commonanalyze.R
ANALYSIS	= Rout.txt lin11.pdf lin12.pdf pq11.pdf pq17.pdf
ALLBUTDOC	= testdata.txt $(ANALYSIS)
RUBYDOC		= rdoc

.PHONY:		all
all:		$(ALLBUTDOC) doc

testdata.txt:	mmerge.rb testmmerge.rb
		$(RUNTESTS) ./$(TIMETEST) >$@

$(ANALYSIS):	intermediate-1

.INTERMEDIATE:	intermediate-1
intermediate-1:	testdata.txt
		$(ANALYZE) > Rout.txt

doc:		$(ALLBUTDOC)
		rm -f -r doc; \
		$(RUBYDOC)

.PHONY:		clean
clean:
		rm -f $(ALLBUTDOC); \
		rm -f -r doc
