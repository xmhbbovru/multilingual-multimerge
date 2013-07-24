# java/Makefile rev. 24 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

SHELL		= /bin/sh
JAVA		= java
JAVAC		= javac
JAVACDEBUG	= -g
JAVAFLAGS	= -Xlint:all
JAVADOC		= javadoc
DIR		= com/zulazon/samples
PKG		= com.zulazon.samples
CLASSPATH     = .:/usr/share/java/junit4.jar:/usr/share/java/jopt-simple-4.5.jar
SOURCEPATH	= .
TIMETEST	= $(DIR)/TestMMerge
TESTTEST	= $(DIR)/JUnitTestMMerge
TESTCLASSES	= $(TIMETEST).class $(TESTTEST).class
JAVASRC		= $(DIR)/MMerge.java \
		  $(DIR)/GetCfgOutput.java \
		  $(DIR)/GenerateDataOutput.java \
		  $(TIMETEST).java $(TESTTEST).java
# Note that the list of classes could change if internal class structure changes
ALLCLASSES	= $(DIR)/MMerge.class \
		  $(DIR)/MMerge$$IteratorValuePair.class \
		  $(DIR)/MMerge$$ValueValidPair.class \
		  $(DIR)/GetCfgOutput.class \
		  $(DIR)/GenerateDataOutput.class \
		  $(TESTCLASSES)
RUN		= $(JAVA) -cp $(CLASSPATH)
RUNTESTS	= ../common/runtests.py
ANALYZE		= ../common/commonanalyze.R
ANALYSIS	= Rout.txt lin11.pdf lin12.pdf pq11.pdf pq17.pdf
ALLBUTDOC	= $(ALLCLASSES) testdata.txt $(ANALYSIS)

.PHONY:		all
all:		$(ALLBUTDOC) doc

$(ALLCLASSES):	intermediate-0

.INTERMEDIATE:	intermediate-0
intermediate-0:	$(JAVASRC)
		$(JAVAC) $(JAVAFLAGS) -sourcepath . -cp $(CLASSPATH) \
		$(JAVASRC)

testdata.txt:	$(ALLCLASSES)
		$(RUNTESTS) "$(RUN) $(TIMETEST)" >$@

$(ANALYSIS):	intermediate-1

.INTERMEDIATE:	intermediate-1
intermediate-1:	testdata.txt
		$(ANALYZE) > Rout.txt

doc:		$(ALLBUTDOC)
		rm -f -r doc; \
		$(JAVADOC) -classpath $(CLASSPATH) -d $@ $(PKG)

.PHONY:		clean
clean:
		rm -f $(DIR)/*.class testdata.txt $(ANALYSIS); \
		rm -f -r doc

