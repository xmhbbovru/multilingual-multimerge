// java/TestMMerge.java rev. 22 July 2013 by Stuart Ambler.  Tests MMerge.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

package com.zulazon.samples;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.ListIterator;
import java.util.Random;

import joptsimple.NonOptionArgumentSpec;
import joptsimple.OptionException;
import joptsimple.OptionParser;
import joptsimple.OptionSet;
import joptsimple.OptionSpec;

import com.zulazon.samples.GenerateDataOutput;
import com.zulazon.samples.GetCfgOutput;
import com.zulazon.samples.MMerge;

// Test runs of TestMMerge.java using javac, java 1.7.0_21, times in seconds
// (these were made with version different only in this comment, in input being
// initialized starting with 0 instead of with 1, and in labels output):
//       k   each          n      pq     lin   heapq
//      10  10000      80139    0.06    0.04      NA
//      20  10000     173780    0.07    0.08      NA
//      30  10000     274899    0.08    0.21      NA
//      40  10000     432652    0.11    0.37      NA
//      50  10000     438984    0.11    0.43      NA
//      60  10000     574387    0.22    0.50      NA
//      70  10000     801194    0.24    0.79      NA
//      80  10000     844374    0.20    1.43      NA
//      90  10000     963635    0.22    1.81      NA
//     100  10000    1019296    0.24    2.11      NA
//     160  10000    1662089    0.43    7.15      NA
//    1000  10000   10509733    9.65      NA      NA
//     100  20000    2114335    0.40    7.14      NA
//     100  40000    3992275    3.02      NA      NA
//     100  60000    6348704    1.18      NA      NA
//     100  80000    7989592    5.35      NA      NA
//     100 100000   10304450    2.28      NA      NA
// Note: there was wide variability at times; for instance the 1000 10000 test
// took more often 3+ seconds, but the first time after a new version 9 seconds.
// Thinkpad SL-510 with 8GB memory, Lubuntu 12.10.

// The statistical language R gave a straight line with exceptions! plotting pq
// (priority queue version runtime) vs. log (k) * n, by removing comments from
// the above table with header row, and in R, source("commonanalyze.R").
// Similarly with lin vs. k * n.
// Or, the fitting and plots can be done with just the first 11 rows, resulting
// in smaller residuals and:
// pq  ~=   5.156e-02 +   4.432e-08 * log(k) * n
// lin ~=  -2.491e-01 +   2.658e-08 *     k  * n

// For the data here for k=20..160 and n=10000, the java version is two to four
// times as fast as the C++ version (and some times are faster than for C), but
// this is false for some other data here,// and the java times are quite
// variable.  For instance, the 9+ for k=1000,n=10000 often becomes 3+ after
// the first time it's run in a short time.  The plots also show variability.

// The sorted output checked Ok vs. original sorted input.

// Class for (non-JUnit) test framework.

/**
 * Tests MMerge multimerge functions.
 */
public class TestMMerge {
  static final int DECIMAL = 10;
  static final int MAX_NR_INPUT_INTS = 500000000;  // (Ok for 8GB RAM)

  // Prints program usage.
  
  static void usage() {
    String s = "Test MMerge.java k-way merge.\n"
+ "\n"
+ "Usage:\n"
+ "  ./runmmerge [-l]\n"
+ "  ./runmmerge <nr_inputs> [-l]\n"
+ "  ./runmmerge <nr_inputs> <ave_input_len> [-l]\n"
+ "  ./runmmerge -h | --help\n"
+ "\n"
+ "Arguments:\n"
+ "  <nr_inputs>      Number of sorted input arrays to generate; must be\n"
+ "                   positive and its product with ave_input_len no more\n"
+ "                   than 500 million.  [default: 1000]\n"
+ "  <ave_input_len>  Desired average length of sorted input arrays to\n"
+ "                   generate; must be positive and its product with\n"
+ "                   nr_inputs no more than 500 million [default: 10000].\n"
+ "\n"
+ "Options:\n"
+ "  -h --help        Show this help message and exit.\n"
+ "  -l               Test slower 'linear' as well as priority queue method.\n";
    System.out.print(s);
  }

  static GetCfgOutput getCfg(String args[], int MAX_NR_INPUT_INT,
                             int nrInputs, int aveInputLen) {
    // rely on default initialization to false for the boolean fields
    GetCfgOutput cfgOutput = new GetCfgOutput();
    cfgOutput.nrInputs = nrInputs;
    cfgOutput.aveInputLen = aveInputLen;

    OptionParser parser = new OptionParser();
    parser.accepts("help").forHelp();
    parser.accepts("l");
    OptionSpec<Integer> integerNonOptions =
        parser.nonOptions("nr_inputs then ave_input_len").ofType(Integer.class);
    OptionSet options;
    try {
      options = parser.parse (args);
    } catch (OptionException e) {
      System.out.println("TestMMerge can't understand its command line.");
      usage();
      cfgOutput.error = true;
      return cfgOutput;
    }
    cfgOutput.helpOnly = options.has("help");
    if (cfgOutput.helpOnly) {
      usage();
      return cfgOutput;
    }
    cfgOutput.doMultimergeLin = options.has("l");
    List<Integer> nonOptionsList;
    try {
      nonOptionsList = integerNonOptions.values(options);
    } catch (OptionException e) {
      System.out.println("Incorrect usage.");
      usage();
      cfgOutput.error = true;
      return cfgOutput;
    }
    if (nonOptionsList.size() > 0)
      cfgOutput.nrInputs = nonOptionsList.get(0);
    if (nonOptionsList.size() > 1)
      cfgOutput.aveInputLen = nonOptionsList.get(1);
    if (   cfgOutput.nrInputs <= 0 || cfgOutput.aveInputLen <= 0
        ||   (long) cfgOutput.nrInputs * (long) cfgOutput.aveInputLen
           > (long) MAX_NR_INPUT_INTS) {
      System.out.println(  "nr_inputs (" + cfgOutput.nrInputs
                         + ") and ave_input_len (" + cfgOutput.aveInputLen
                         + ") must be strictly positive.\nTheir product ("
                         + (  (long) cfgOutput.nrInputs
                            * (long) cfgOutput.aveInputLen)
                         + ") must be no greater than " + MAX_NR_INPUT_INTS
                         + ".");
      usage();
      cfgOutput.error = true;
      return cfgOutput;
    }

    return cfgOutput;
  }

  // Prints a List<Integer>.

  static void printListInt (String name, List<Integer> list) {
    System.out.print(name + " ");
    for (Integer i : list)
      System.out.print(i.toString() + " ");
    System.out.println("");
  }

  // Test data small enough to verify by hand.
  
  static boolean verifySmallData () {
    int aint1[] = {  2,  6, 88, 688 };
    int aint2[] = {  1,  2,  3,   4, 5, 6, 7, 8 };
    int aint3[] = {  5, 10, 15,  20 };
    ArrayList<Integer> correctOutput = new ArrayList<Integer>
          (Arrays.asList(1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 10, 15, 20, 88, 688));
    
    ArrayList<Integer> a1 = new ArrayList<Integer>(aint1.length);
    for (int i = 0; i <  aint1.length; i++)
      a1.add(aint1[i]);
    ArrayList<Integer> a2 = new ArrayList<Integer>(aint2.length);
    for (int i = 0; i <  aint2.length; i++)
      a2.add(aint2[i]);
    ArrayList<Integer> a3 = new ArrayList<Integer>(aint3.length);
    for (int i = 0; i <  aint3.length; i++)
      a3.add(aint3[i]);

    ArrayList<ArrayList<Integer> > arrays =
        new ArrayList<ArrayList<Integer> >(3);
    arrays.add(a1);
    arrays.add(a2);
    arrays.add(a3);

    MMerge mmerge = new MMerge();

    boolean retval = true;
    ArrayList<Integer> output = mmerge.multimergePQ(arrays);
    printListInt("multimerge pq     small data", output);
    if (!output.equals(correctOutput)) {
      System.out.println("multimerge pq differs from correctOutput");
      retval = false;
    }

    output = mmerge.multimerge(arrays);
    printListInt("multimerge linear small data", output);
    if (!output.equals(correctOutput)) {
      System.out.println("multimerge lin differs from correctOutput");
      retval = false;
    }

    return retval;
  }

  // Calculate and print statistics for list of ints, interpreted as
  // various lengths centered at aveInputLen; return total length.

  static int calcDisplayStats(ArrayList<Integer> lens, int aveInputLen) {
    int nrInputs = lens.size();
    int totLens  = 0;
    int maxLen   = Integer.MIN_VALUE;
    int minLen   = Integer.MAX_VALUE;
    for (int len : lens) {
      totLens += len;
      if (maxLen < len)
        maxLen = len;
      if (minLen > len)
        minLen = len;
    }
    double meanLen  = (double) totLens / (double) nrInputs;
    double variance = 0;
    for (int len : lens) {
      double  diff = (double) len - meanLen;
      variance += (diff * diff);
    }
    System.out.println(  "nrInputs " + nrInputs
                         + ", requested aveInputLen " + aveInputLen);
    System.out.println(  "their product " + nrInputs * aveInputLen
                         + ", actual totLens " + totLens);
    System.out.printf("meanLen %d, std dev %d, minLen %d, maxLen %d\n",
                      Math.round(meanLen),
                      Math.round(Math.sqrt(variance / (double) nrInputs)),
                      minLen, maxLen);
    return totLens;
  }

  // Generate test data: an ArrayList of nrInput ArrayLists of random lengths
  // uniformly distributed from about aveInputLen / 10 to 2 * aveInputLen minus
  // that; a range centered at aveInputLen.

  static GenerateDataOutput generateData(int nrInputs, int aveInputLen) {
    int amin = (aveInputLen + 5) / 10;
    if (amin < 1)
      amin = 1;
    int amax = 2 * aveInputLen - amin;

    Random random = new Random ();
    ArrayList<Integer> lens = new ArrayList<Integer>(nrInputs);
    for (int i = 0; i < nrInputs; i++)
      lens.add(amin + random.nextInt(amax + 1));

    int totLens = calcDisplayStats(lens, aveInputLen);

    // Generate the input data by initializing an overall input array
    // sequentially, shuffling it, piecing it out into the input arrays,
    // and sorting each input array.
  
    ArrayList<Integer> input = new ArrayList<Integer>(totLens);
    for (int i = 0, j = 1; i < totLens; i++, j++)
      input.add(j);
    ArrayList<Integer> inputCopy = new ArrayList<Integer>(totLens);
    ListIterator<Integer> it = input.listIterator();
    for (int i = 0; it.hasNext(); i++)
      inputCopy.add(it.next());
    Collections.shuffle(input);

    ArrayList<ArrayList<Integer> > arrays =
        new ArrayList<ArrayList<Integer> >(nrInputs);
    ListIterator<Integer> lensIt  = lens.listIterator();
    ListIterator<Integer> inputIt = input.listIterator();
    while (lensIt.hasNext()) {  // presumed unnecessary to check others
      int len = lensIt.next();
      ArrayList<Integer> array = new ArrayList<Integer>(len);
      for (int i = 0; i < len; i++)
        array.add (inputIt.next());
      Collections.sort(array);
      arrays.add(array);
    }

    GenerateDataOutput output = new GenerateDataOutput();
    output.inputCopy = inputCopy;
    output.arrays    = arrays;
    return output;
  }

  // For timing merges; in actual usage s is nonempty for start == false only.

  static long startTime = 0;

  static void stopwatch(boolean start, String s) {
    if (start) {
      startTime = System.nanoTime();
      if (s.length() > 0)
        System.out.println(s + " stopwatch start");
    } else {
      long endTime = System.nanoTime();
      System.out.println(s + " stopwatch end");
      System.out.printf(" elapsed %.2f sec\n",
                        ((double) (endTime - startTime) * 1.0e-09));
    }
  }

 /**
  * Tests MMerge multimerge functions.
  */
  public static int main_func(String[] args) {
    // Obtain parameters for more voluminous test data from the command line,
    // or use the following defaults.

    // Product of the two following variables must be <= MAX_NR_INPUT_INTS.
    int nrInputs    = 1000;
    int aveInputLen = 10000;

    GetCfgOutput cfgOutput = getCfg(args, MAX_NR_INPUT_INTS,
                                    nrInputs, aveInputLen);
    if (cfgOutput.helpOnly)
      return 0;
    else if (cfgOutput.error)
      return -1;
    nrInputs = cfgOutput.nrInputs;
    aveInputLen = cfgOutput.aveInputLen;
    boolean doMultimergeLin = cfgOutput.doMultimergeLin;

    int retval = 0;  // set to -1 if any errors in merge output
    if (!verifySmallData())
      retval = -1;

    // Do the larger tests.

    GenerateDataOutput gdo = generateData(nrInputs, aveInputLen);
    ArrayList<Integer> inputCopy = gdo.inputCopy;
    ArrayList<ArrayList<Integer> > arrays = gdo.arrays;   
    
    MMerge mmerge = new MMerge();

    System.out.println("multimerge priority queue");
    stopwatch(true, "");
    ArrayList<Integer> output = mmerge.multimergePQ(arrays);
    stopwatch(false, "multimerge pq ");
    boolean cmpOk = output.equals(inputCopy);
    if (!cmpOk)
      retval = -1;
    System.out.println(  "multimerge pq  "
                       + (cmpOk ?
                          "matches     " : "differs from")
                       + " inputCopy");

    if (doMultimergeLin) {
      System.out.println("multimerge linear");
      stopwatch(true, "");
      output = mmerge.multimerge(arrays);
      stopwatch(false, "multimerge lin");
      cmpOk = output.equals(inputCopy);
      if (!cmpOk)
        retval = -1;
      System.out.println(  "multimerge lin "
                         + (cmpOk ?
                            "matches     " : "differs from")
                         + " inputCopy");
    }

    return retval;
  }

 /**
  * main function calls main_func, which has a non-void return value.
  */
  public static void main(String[] args) {
    System.exit(main_func(args));
  }
}