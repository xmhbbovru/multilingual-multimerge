#!/usr/bin/env Rscript
# common/commonanalyze.R rev. 31 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

source('../common/analyze.R')
pq  = "pq"
lin = "lin"
use.log=list()
use.log[pq] = TRUE
if (basename(getwd()) == "erlang")
  use.log[pq] = FALSE
use.log[lin] = FALSE
t<-read.table('testdata.txt',header=TRUE)
t$k = 1.0 * t$k  # to avoid integer overflow of product
t$n = 1.0 * t$n  # to avoid integer overflow of product
analyze(t, c(seq(1,17)), pq, use.log[pq][[1]])
analyze(t, c(seq(1,11),13), lin, use.log[lin][[1]])
analyze(t, seq(1,11), pq, use.log[pq][[1]])
analyze(t, seq(1,11), lin, use.log[lin][[1]])
