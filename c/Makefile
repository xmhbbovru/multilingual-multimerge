# c/Makefile rev. 24 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
C		= gcc
CDEBUG		= -g
CFLAGS		= -x c -std=c99
CLIBS		= -largtable2 -lm
CTESTLIBS	= -lcheck
CMERGESRC	= pqueue.c mmerge.c testmmerge.c
CMERGEHDR	= pqueue.h mmerge.h testmmerge.h
TIMETEST	= testmmergemain
TESTTEST	= checktestmmerge
RUNTESTS	= ../common/runtests.py
ANALYZE		= ../common/commonanalyze.R
ANALYSIS	= Rout.txt lin11.pdf lin12.pdf pq11.pdf pq17.pdf
VALGRIND	= valgrind
VALSUPP		= vgsupp
VALOPTS		= --suppressions=$(VALSUPP)
SHORTARGS	= 100 100 -l

.PHONY:		all
all:		$(TIMETEST) $(TESTTEST) testdata.txt $(ANALYSIS) \
		valgrindout.txt

$(TIMETEST):	$(TIMETEST).c $(CMERGESRC) $(CMERGEHDR)
		$(C) $(CFLAGS) $(CMERGESRC) $@.c $(CLIBS) -o $@

$(TESTTEST):	$(TESTTEST).c $(CMERGESRC) $(CMERGEHDR)
		$(C) $(CFLAGS) $(CMERGESRC) $@.c $(CLIBS) \
		$(CTESTLIBS) -o $@

testdata.txt:	$(TIMETEST)
		$(RUNTESTS) ./$(TIMETEST) >$@

$(ANALYSIS):	intermediate

.INTERMEDIATE:	intermediate
intermediate:	testdata.txt
		$(ANALYZE) > Rout.txt

valgrindout.txt:	$(TIMETEST) $(VALSUPP)
		$(VALGRIND) $(VALOPTS) ./$(TIMETEST) \
		$(SHORTARGS) > $@ 2>&1

.PHONY:		clean
clean:
		rm -f $(TIMETEST) $(TESTTEST) \
		testdata.txt $(ANALYSIS) valgrindout.txt
