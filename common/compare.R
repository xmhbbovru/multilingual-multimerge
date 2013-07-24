#!/usr/bin/env Rscript
# common/commonanalyze.R rev. 23 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

compare <- function(dirprefix1, dirprefix2, language, pqlin) {
  t1t <- read.table(paste(dirprefix1, "testdata.txt", sep=""),header=TRUE)
  t2t <- read.table(paste(dirprefix2, "testdata.txt", sep=""),header=TRUE)
  t1R <- read.table(paste(dirprefix1, "Rout.txt", sep=""),header=FALSE)
  t2R <- read.table(paste(dirprefix2, "Rout.txt", sep=""),header=FALSE)

  if (pqlin == "pq") {
    ratio <- t2t$pq / t1t$pq
    fullpartrow <- c(1,3)
  } else {
    ratio <- t2t$lin / t1t$lin
    fullpartrow <- c(2,4)
  }
  partnrpts = t2R[fullpartrow[2], 1]
  fullpartratio <- list(ratio, ratio[1:partnrpts])
  fullpartstr <- c("full data", sprintf("%d point", partnrpts))

  # Keep this out of the loop; seems like enough histograms is enough.
  pdf(sprintf("%s.%shist.pdf", language, pqlin))  # Fix filename here and in
                                                  # Makefile if move into loop.
  hist(fullpartratio[[1]],
       main=sprintf("Histogram of %s %s execution time ratio %s to c",
                    pqlin, fullpartstr[1], language))
  dev.off()

  for (fullpart in 1:2) {
    print(sprintf("%3s %s execution time ratio %s to c",
                  pqlin, fullpartstr[fullpart], language));
    print(summary(fullpartratio[[fullpart]]))
    print(sprintf("regression coeff. ratio %g",
                    t2R[fullpartrow[fullpart], 7]
                  / t1R[fullpartrow[fullpart], 7]))
    print("")
  }
  print("")
}

args <- commandArgs(trailingOnly=TRUE)
compare(args[1], args[2], args[3], args[4])
