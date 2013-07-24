// c/testmmerge.c rev. 23 July 2013 by Stuart Ambler.  Tests c/mmerge.c
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Tests multi-way merge of k sorted arrays of int, total length n, in memory,
// using either a simple algorithm linear in k, or instead an algorithm that
// uses a priority queue, logarithmic in k.  Either way, the dependence on n
// is linear.  Minimal error handling.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <argtable2.h>

#include "./mmerge.h"

// Prints usage of program.

void usage() {
  const char *s = "Test mmerge.rb k-way merge.\n"
"\n"
"Usage:\n"
"  ./testmmerge.rb [-l]\n"
"  ./testmmerge.rb <nr_inputs> [-l]\n"
"  ./testmmerge.rb <nr_inputs> <ave_input_len> [-l]\n"
"  ./testmmerge.rb -h | --help\n"
"\n"
"Arguments:\n"
"  <nr_inputs>      Number of sorted input arrays to generate; must be\n"
"                   positive and its product with ave_input_len no more\n"
"                   than 500 million.  [default: 1000]\n"
"  <ave_input_len>  Desired average length of sorted input arrays to\n"
"                   generate; must be positive and its product with\n"
"                   nr_inputs no more than 500 million [default: 10000].\n"
"\n"
"Options:\n"
"  -h --help        Show this help message and exit.\n"
"  -l               Test slower linear method as well as priority queue method."
"\n";
  (void) printf("%s", s);
}

// Get and process command-line arguments.  See usage().  Set defaults before calling.

void get_cfg(int argc, char *argv[], int max_nr_input_ints,
             int *p_nr_inputs, int *p_ave_input_len,
             bool *p_do_multimerge_lin, bool *p_help_only, bool *p_error) {
  struct arg_lit *help = arg_lit0("h", "help",
                                  "Show help message and exit.");
  struct arg_lit *lin  = arg_lit0("l", NULL,
                                  "Test slower linear as well as priority queue method.");
  struct arg_int *nr   = arg_int0(NULL, NULL, "<nr_inputs>",
                                  "Number of sorted input arrays to generate.");
  struct arg_int *len  = arg_int0(NULL, NULL, "<ave_input_len>",
                                  "Desired averagel length of sorted input arrays.");
  struct arg_end *end  = arg_end(20);
  void *argtable[]     = { help, lin, nr, len, end };
  if (arg_nullcheck(argtable) != 0) {
    (void) printf("Insufficient memory to parse command-line arguments.\n");
    *p_error = true;
    return;
  }
  int nr_errors = arg_parse(argc, argv, argtable);
  if (nr_errors == 0) {
    if (help->count > 0) {
      usage();
      *p_help_only = true;
      return;
    }
    if (lin->count > 0)
      *p_do_multimerge_lin = true;
    if (nr->count > 0)
      *p_nr_inputs = nr->ival[0];
    if (len->count > 0)
      *p_ave_input_len = len->ival[0];
    if (   *p_nr_inputs <= 0 || *p_ave_input_len <= 0
           ||   (long) (*p_nr_inputs) * (long) (*p_ave_input_len)
              > (long) max_nr_input_ints) {
      (void) printf("nr_inputs (%d) and ave_input len (%d) must be strictly positive,\n"
                    "and their product %19ld, the total number of ints to merge,\n"
                    "no greater than   %19d.\n", *p_nr_inputs, *p_ave_input_len,
                    (long) (*p_nr_inputs) * (long) (*p_ave_input_len), max_nr_input_ints);
      usage();
      *p_error = true;
      return;
    }
  } else {
    (void) printf("Incorrect usage.\n");
    usage();
    *p_error = true;
    return;
  }
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
}

// The following three variables and three functions provide a simple way to
// keep track of memory allocations to make freeing them easier in error
// handling and otherwise.  The intent is to call handle_malloc in place of
// malloc, free_given_malloc in place of an individual free, and free_mallocs
// to free all the outstanding allocations.  The small, fixed number for
// MAX_NR_MALLOC_CALLS and some of the logic reflect the very small number of
// calls to malloc in this code.

#define MAX_NR_MALLOC_CALLS (16)  // Watch this with code revisions.
static int nr_malloc_calls = 0;
static void *(malloc_list[MAX_NR_MALLOC_CALLS]);

// Free the memory pointed to by p, removing that entry from malloc_list if it
// is there.  Though the intent is to use free_given_malloc for memory obtained
// via handle_malloc, the code provides for freeing untracked memory as well.

void free_malloc(void *p) {
  if (p == NULL)
    return;
  free(p);

  if (nr_malloc_calls <= 0)
    return;

  for (int j = nr_malloc_calls - 1; j >= 0; --j) {
    if (p == malloc_list[j]) {
      --nr_malloc_calls;
      for (int k = j; k < nr_malloc_calls; k++)
        malloc_list[k] = malloc_list[k + 1];
      break;
    }
  }
}

// Free all outstanding tracked allocated memory.

void free_mallocs() {
  for (int j = nr_malloc_calls - 1; j >= 0; --j)
    free (malloc_list[j]);
  nr_malloc_calls = 0;
}

// To replace malloc to track allocated memory, and to free all tracked memory
// and display a string s in case malloc fails.  len is argument to malloc.

void *handle_malloc (int len, const char *s) {
  if (nr_malloc_calls >= MAX_NR_MALLOC_CALLS) {
    free_mallocs();
    return NULL;
  }
  
  void *retval = malloc(len);
  if (retval == NULL) {
    (void) printf ("%s\n", s);
    free_mallocs();
  } else {
    malloc_list[nr_malloc_calls] = retval;
    ++nr_malloc_calls;
  }

  return retval;
}

// Test that two integer arrays are equal.

bool int_arrays_equal(int n0, int array0[n0], int n1, int array1[n1]) {
  if (n0 != n1)
    return false;

  for (int i = 0; i < n0; ++i) {
    if (array0[i] != array1[i])
      return false;
  }

  return true;
}

// Prints an array of ints.

void print_iv(const char *name, int n, int array[n]) {
  (void) printf("%s ", name);
  for (int i = 0; i < n; ++i, ++array)
    (void) printf("%d ", *array);
  (void) printf("\n");
}

// Test data small enough to verify by hand.

bool verify_small_data () {
  int a1[] = {  2,  6, 88, 688 };
  int a2[] = {  1,  2,  3,   4, 5, 6, 7, 8 };
  int a3[] = {  5, 10, 15,  20 };
  int a123[]   = {1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 10, 15, 20, 88, 688};
  int output[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,   0};
  int nr_inputs = 3;
  int small_lens[] = { sizeof(a1) / sizeof(int),
                       sizeof(a2) / sizeof(int),
                       sizeof(a3) / sizeof(int) };
  int *small_arrays[] = { a1, a2, a3 };
  int tot_lens = 0;
  for (int i = 0; i < nr_inputs; ++i)
    tot_lens += small_lens[i];

  bool retval = true;
  
  if (!multimerge_pq(nr_inputs, small_lens, small_arrays, tot_lens, output))
    (void) printf ("Error in multimerge_pq with small data\n");
  print_iv("multimerge pq     small data", tot_lens, output);
  if (!int_arrays_equal(tot_lens, output,
                        sizeof(a123) / sizeof(a123[0]), a123)) {
    (void) printf("multimerge pq  differs from correctOutput\n");
    retval = false;
  }

  if (!multimerge(nr_inputs, small_lens, small_arrays, tot_lens, output))
    (void) printf ("Error in multimerge (linear) with small data\n");
  print_iv("multimerge linear small data", tot_lens, output);
  if (!int_arrays_equal(tot_lens, output,
                        sizeof(a123) / sizeof(a123[0]), a123)) {
    (void) printf("multimerge lin differs from correctOutput\n");
    retval = false;
  }
      
  return retval;
}

// Generates random integers in a range; used to generate lengths for
// input arrays of test data.

int int_rand_in_range(bool init, bool do_seed, int amin, int amax) {
  const static double kInvRandMax = 1.0 / RAND_MAX;
  static int rmin;
  static double factor;
  if (init) {
    rmin   = amin;
    factor = (double) ((amax - amin) * kInvRandMax);
    if (do_seed)
      srand(clock());
  }
  return rmin + (int) (factor * (double) (rand ()) + 0.5);
}

// Calculate and print statistics for list of ints, interpreted as
// various lengths centered at aveInputLen; return total length.

int calc_display_stats(int ave_input_len, int nr_inputs, int lens[nr_inputs]) {
  int tot_lens  = 0;
  int len;
  int max_len  = INT_MIN;
  int min_len  = INT_MAX;

  for (int i = 0; i < nr_inputs; ++i) {
    len = lens[i];
    tot_lens += len;
    if (max_len < len)
      max_len = len;
    if (min_len > len)
      min_len = len;
  }
  
  double mean_len =   (double) (tot_lens)
                    / (double) (nr_inputs);
  double variance = 0;
  for (int i = 0; i < nr_inputs; ++i) {
    double diff = (double) (lens[i] - mean_len);
    variance += (diff * diff);
  }

  (void) printf("nr_inputs %d, requested ave_input_len %d\n"
                "their product %d, actual tot_lens %d\n"
                "mean_len %d, std dev %d, min_len %d, max_len %d\n",
                nr_inputs, ave_input_len, nr_inputs * ave_input_len,
                tot_lens, (int) (round(mean_len)),
                (int) (round(sqrt(variance / (double) (nr_inputs)))),
                min_len, max_len);

  return tot_lens;
}

// Swap of two elements of indices i, j, of an int array.

static inline void shuffle_swap (int *array, int i, int j) {
  int temp = array[i];
  array[i] = array[j];
  array[j] = temp;
}

// Shuffles array of ints, equivalent to constructing a random permutation
// of 0 through n-1.  Use the algorithm of Introduction to Algorithms, 3rd
// edition, by Cormen, Leiserson, Rivest, and Stein, MIT, 2009, pp. 126-127,
// which produces uniform random permutations.

void random_shuffle (int n, int array[n]) {
  for (int i = 0; i < n; i++) {
    shuffle_swap (array, i, int_rand_in_range(true, false, i, n - 1));
  }
}

// Compare function for use in qsort of integer array.

int compare_int (const void *p0, const void *p1) {
  int i0 = *((const int *) p0);
  int i1 = *((const int *) p1);

  if (i0 < i1)
    return -1;
  else if (i0 == i1)
    return 0;
  else
    return 1;
}

// Generate test data: an array of nr_input arrays of random lengths uniformly
// distributed from about ave_input_len / 10 to 2 * ave_input_len minus that;
// a range centered at ave_input_len.  Return false for failure.

bool generate_data(int    nr_inputs,
                   int    ave_input_len,
                   int  **p_lens,
                   int   *p_tot_lens,
                   int  **p_input_copy,
                   int ***p_arrays) {
  int amin = (ave_input_len + 5) / 10;
  if (amin < 1)
    amin = 1;
  int amax = 2 * ave_input_len - amin;
  int_rand_in_range(true, false, amin, amax);
  int *lens = handle_malloc (nr_inputs * sizeof (int),
                             "Unable to allocate memory for \"lens\".");
  if (lens == NULL)
    return false;
  *p_lens = lens;

  for (int i = 0; i < nr_inputs; ++i)
    lens[i] = int_rand_in_range(false, false, amin, amax);

  int tot_lens = calc_display_stats(ave_input_len, nr_inputs, lens);
  *p_tot_lens  = tot_lens;

  // Generate the input data by initializing an overall input array
  // sequentially, shuffling it, piecing it out into the individual input
  // arrays, and sorting each input array.

  int *input = handle_malloc (tot_lens * sizeof (int),
                              "Unable to allocate memory for input.");
  if (input == NULL)
    return false;

  *p_arrays = (int **) handle_malloc (tot_lens * sizeof (int *),
                                      "Unable to allocate memory for arrays.");
  if (*p_arrays == NULL)
    return false;

  // Allocate memory for input_copy and the individual arrays in arrays
  // in one piece.

  *p_input_copy = handle_malloc (2 * tot_lens * sizeof (int),
                                 "Unable to allocate memory for input.");
  if (*p_input_copy == NULL)
    return false;

  for (int i = 0, j = 1; i < tot_lens; ++i, ++j) {
    input[i] = j;
    (*p_input_copy)[i] = j;  // for later comparison with merge result
  }

  random_shuffle (tot_lens, input);

  int k = 0;
  int *array = *p_input_copy + tot_lens;
  for (int i = 0; i < nr_inputs; ++i) {
    (*p_arrays)[i] = array;
    for (int j = 0; j < lens[i]; ++j, ++k)
      array[j] = input[k];
    qsort(array, lens[i], sizeof (int), compare_int);
    array += lens[i];
  }

  free_malloc(input);
  return true;
}

// For timing merges; in actual usage s is nonempty for start == false only.

void stopwatch(bool start, const char *s) {
  static clock_t t_start;
  if (start) {
    t_start = clock();
    if (strlen(s) > 0)
      (void) printf("%s stopwatch start\n", s);
  } else {
    clock_t t_end = clock();
    (void) printf("%s stopwatch end\n elapsed %ju clocks at %ju per sec, or "
                  "%.2f sec\n", s,
                  (uintmax_t) (t_end - t_start),
                  (uintmax_t) CLOCKS_PER_SEC,
                    (double) (t_end - t_start)
                  / (double) (CLOCKS_PER_SEC));
  }
}

// Test program for mmerge.c.

int testmmerge_main(int argc, char *argv[]) {
  // Obtain parameters for more voluminous test data from the command line,
  // or use the following defaults.
  
  const int max_nr_input_ints = 500000000;  // 500 million (Ok for 8GB RAM)
                                            // >= product of two following:
  int  nr_inputs         = 1000;
  int  ave_input_len     = 10000;
  bool do_multimerge_lin = false;
  bool help_only         = false;
  bool error             = false;

  get_cfg(argc, argv, max_nr_input_ints, &nr_inputs, &ave_input_len,
          &do_multimerge_lin, &help_only, &error);
  if (help_only)
    return 0;
  else if (error)
    return -1;

  bool retval = true;  // set to false if any errors in merge output
  if (!verify_small_data())
    retval = false;

  // Do the larger tests.

  int  *lens;
  int   tot_lens;
  int  *input_copy;
  int **arrays;
  if (!generate_data(nr_inputs, ave_input_len, &lens, &tot_lens,
                     &input_copy, &arrays))
    return -1;

  int *output = handle_malloc (tot_lens * sizeof (int),
                               "Unable to allocate memory for output.");
  if (output == NULL)
    return -1;

  (void) printf ("multimerge priority queue\n");
  stopwatch(true, "");
  if (!multimerge_pq(nr_inputs, lens, arrays, tot_lens, output))
    (void) printf ("Error in multimerge_pq with large data\n");
  stopwatch(false, "multimerge pq ");
  bool cmp_ok = int_arrays_equal (tot_lens, output, tot_lens, input_copy);
  if (!cmp_ok)
    retval = false;
  (void) printf ("multimerge pq       %s input_copy\n",
                 cmp_ok ? "matches     " : "differs from");

  if (do_multimerge_lin) {
    (void) printf ("multimerge (linear)\n");
    stopwatch(true, "");
    if (!multimerge(nr_inputs, lens, arrays, tot_lens, output))
      (void) printf ("Error in multimerge (linear) with small data\n");
    stopwatch(false, "multimerge lin");
    cmp_ok = int_arrays_equal (tot_lens, output, tot_lens, input_copy);
    if (!cmp_ok)
    retval = false;
    (void) printf ("multimerge lin      %s input_copy\n",
                   cmp_ok ? "matches     " : "differs from");
  }

  free_mallocs();

  return retval ? 0 : -1;
}
