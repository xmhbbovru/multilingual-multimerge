# common/Makefile rev. 31 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
COMPARE		= ./compare.R
NONCDIRS	= cc erlang java python ruby
COMPARISONS	= $(foreach dir,$(NONCDIRS),$(dir).pqhist.pdf) \
		  $(foreach dir,$(NONCDIRS),$(dir).linhist.pdf)

.PHONY:		all
all:		comparetoc.txt $(COMPARISONS)

comparetoc.txt:
		rm -f *hist.pdf

%.pqhist.pdf:	../c/testdata.txt ../%/testdata.txt \
		../c/Rout.txt     ../%/Rout.txt
		$(COMPARE) ../c/ ../$(word 1,$(subst ., ,$@))/ \
		$(word 1,$(subst ., ,$@)) pq  >> comparetoc.txt

%.linhist.pdf:	../c/testdata.txt ../%/testdata.txt \
		../c/Rout.txt     ../%/Rout.txt
		$(COMPARE) ../c/ ../$(word 1,$(subst ., ,$@))/ \
		$(word 1,$(subst ., ,$@)) lin  >> comparetoc.txt

.PHONY:		clean
clean:
		rm -f comparetoc.txt $(COMPARISONS)


