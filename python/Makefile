# python/Makefile rev. 24 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
TIMETEST	= mmerge.py
TESTTEST	= testmmerge.py
RUNTESTS	= ../common/runtests.py
ANALYZE		= ../common/commonanalyze.R
ANALYSIS	= Rout.txt lin11.pdf lin12.pdf pq11.pdf pq17.pdf
ALLBUTDOC	= testdata.txt $(ANALYSIS)
DOC		= mmerge.html testmmerge.html
PYTHONDOC	= pydoc -w

.PHONY:		all
all:		$(ALLBUTDOC) $(DOC)

testdata.txt:	$(TIMETEST) $(TESTTEST)
		$(RUNTESTS) ./$(TIMETEST) >$@

$(ANALYSIS):	intermediate-1

.INTERMEDIATE:	intermediate-1
intermediate-1:	testdata.txt
		$(ANALYZE) > Rout.txt

%.html:		%.py
		$(PYTHONDOC) $(basename $@)

.PHONY:		clean
clean:
		rm -f $(ALLBUTDOC) $(DOC)
