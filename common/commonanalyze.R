# common/commonanalyze.R rev. 22 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

source('../common/analyze.R')
t<-read.table('testdata.txt',header=TRUE)
analyze(t, c(seq(1,17)), TRUE)
analyze(t, c(seq(1,11),13), FALSE)
analyze(t, seq(1,11), TRUE)
analyze(t, seq(1,11), FALSE)
