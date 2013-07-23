# common/analyze.R rev. 22 July 2013 by Stuart Ambler.  analyze function.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE

analyze <- function(t, ixs, use.log) {
  sel <- t[ixs,]
  len <- length(ixs)
  if (use.log) {
    lmresult <- lm(sel$pq ~ I(log(sel$k) * sel$n))
    pdf(sprintf("pq%g.pdf", len))
    plot(lmresult$coeff[1] + lmresult$coeff[2] * log(sel$k) * sel$n, sel$pq,
         xlab=sprintf("%g points fitted a + b * log(k) * n", len),
         ylab="pq", main="priority queue timings vs. fitted formula")
    dev.off()
    cat(sprintf("%g points pq  ~= %11.3e + %11.3e * log(k) * n\n",
                len, lmresult$coeff[1], lmresult$coeff[2]))
  } else {
    lmresult <- lm(sel$lin~I(sel$k*sel$n))
    pdf(sprintf("lin%g.pdf", len))
    plot(lmresult$coeff[1] + lmresult$coeff[2] * sel$k * sel$n, sel$lin,
         xlab=sprintf("%g points fitted a + b * k * n", len),
         ylab="lin", main="'linear' method timings vs. fitted formula")
    dev.off()
    cat(sprintf("%g points lin ~= %11.3e + %11.3e *     k  * n\n",
                len, lmresult$coeff[1], lmresult$coeff[2]))
  }
} 
