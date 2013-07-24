# Makefile rev. 24 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
DIRS		= c cc common java python ruby
ALLS		= $(addprefix all-,$(DIRS))
CLEANS		= $(addprefix clean-,$(DIRS))

.PHONY:		all
all:		$(ALLS)
$(ALLS):
		$(MAKE) -C $(subst all-,,$@) all

all-common:	all-c all-cc all-java all-python all-ruby

.PHONY:		clean
clean:		$(CLEANS)
$(CLEANS):
		$(MAKE) -C $(subst clean-,,$@) clean
