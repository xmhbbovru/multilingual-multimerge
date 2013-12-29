# cc/Makefile rev. 28 December 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
CC		= g++
CCDEBUG		= -g
CCFLAGS		= --std=c++11
CCLIBS		= -largtable2 -lpthread
CCTESTLIBS	= -lcppunit
CCMERGESRC	= mmerge.cc testmmerge.cc
CCMERGEHDR	= mmerge.h testmmerge.h
TIMETEST	= testmmergemain
TESTTEST	= cppunittestmmerge
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

$(TIMETEST):	$(TIMETEST).cc $(CCMERGESRC) $(CCMERGEHDR)
		$(CC) $(CCFLAGS) $(CCMERGESRC) $@.cc $(CCLIBS) -o $@

$(TESTTEST):	$(TESTTEST).cc $(CCMERGESRC) $(CCMERGEHDR)
		$(CC) $(CCFLAGS) $(CCMERGESRC) $@.cc $(CCLIBS) \
		$(CCTESTLIBS) -o $@

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
