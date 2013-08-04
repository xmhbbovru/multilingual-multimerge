# common/analyze.R rev. 04 August 2013 by Stuart Ambler.  analyze function.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

# Fits a choice of log(k) * n or k * n (governed by use.log TRUE or FALSE)
# to table t (read from testdata.txt) with rows selected by ixs.  method
# is text inserted into filenames and graph labels ("pq" or "lin" for now,
# and this also selects the dependent variable column in the table).  dir
# is a string prepended to the title; commonanalyze sets it to the language
# directory where the data is being analyzed.
analyze <- function(t, ixs, method, use.log, dir) {
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
  pred <- function(k,n) { return(  lmresult$coeff[1]
                                 + lmresult$coeff[2] * kfun(k) * n) }
  x = pred(sel$k, sel$n)
  y = sel[,method]
  lim = c(min(min(x), min(y)), max(max(x), max(y)))
  mineach = min(sel$each)
  maxeach = max(sel$each)
  if (mineach == maxeach)
    plotchars = 79
  else
    plotchars = 79 + 11 * (sel$each - mineach) / (maxeach - mineach)
  plot(x, y, xlim=lim, ylim=lim, pch=plotchars,
       xlab=sprintf("%g points fitted a + b * %s * n, %s",
                    len, ktxt, "O..Z for ave input len range"),
       ylab=method,
       main=sprintf("%s %s timings vs. fitted formula", dir, method))
  dev.off()
  cat(sprintf("%g points %s  ~= %11.3e + %11.3e * %s * n\n",
              len, method, lmresult$coeff[1], lmresult$coeff[2], ktxt))
} 
