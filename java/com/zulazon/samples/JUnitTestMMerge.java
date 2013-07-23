// java/JUnitTestMMerge.java rev. 22 July 2013 by Stuart Ambler.
// Runs TestMMerge.main_func using JUnit4.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

package com.zulazon.samples;

import static org.junit.Assert.assertEquals;

import org.junit.Test;
import org.junit.Ignore;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/**
 * Runs TestMMerge.main using Junit4.
 */
public class JUnitTestMMerge {
  @Test
  public void testMain () {
    String args[] = {};
    org.junit.Assert.assertEquals("TestMMerge.main_func returned nonzero.",
                                  TestMMerge.main_func(args), 0);
  }
}