# common/analyze.R rev. 31 July 2013 by Stuart Ambler.  analyze function.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

# Fits a choice of log(k) * n or k * n (governed by use.log TRUE or FALSE)
# to table t (read from testdata.txt) with rows selected by ixs.  method
# is text inserted into filenames and graph labels ("pq" or "lin" for now,
# and this also selects the dependent variable column in the table).
analyze <- function(t, ixs, method, use.log) {
  sel  <- t[ixs,]
  len  <- length(ixs)
  ktxt <- "k"
  kfun <- function(k) { return(k) }
  if (use.log) {
    ktxt <- "log(k)"
    kfun <- function(k) { return(log(k)) }
  }
  lmresult <- lm(sel[,method] ~ I(kfun(sel$k) * sel$n))
  pdf(sprintf("%s%g.pdf", method, len))
  plot(lmresult$coeff[1] + lmresult$coeff[2] * kfun(sel$k) * sel$n,
       sel[,method], xlab=sprintf("%g points fitted a + b * %s * n", len, ktxt),
       ylab=method, main=sprintf("%s timings vs. fitted formula", method))
  dev.off()
  cat(sprintf("%g points %s  ~= %11.3e + %11.3e * %s * n\n",
              len, method, lmresult$coeff[1], lmresult$coeff[2], ktxt))
} 
