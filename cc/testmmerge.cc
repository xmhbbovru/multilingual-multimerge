// cc/testmmerge.cc rev. 23 July 2013 by Stuart Ambler.  Tests cc/mmerge.cc.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

// Tests multi-way merge of k sorted arrays of int, total length n, in memory,
// using either a simple algorithm linear in k, or instead an algorithm that
// uses a priority queue, logarithmic in k.  Either way, the dependence on n
// is linear.  No exception or other error handling.  streams are used in this
// test code despite discouragement for Google style.

#include <iostream>
#include <sstream>

#include <argtable2.h>
#include "./mmerge.h"
#include "./testmmerge.h"

namespace mm = ::com_zulazon_samples_cc_mmerge;

// Print program usage.

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
  std::cout << s;
}

// Get and process command-line arguments.  See usage().

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
    std::cout << "Insufficient memory to parse command-line arguments.\n"
              << std::endl;
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
      std::cout << "nr_inputs (" << *p_nr_inputs << ") and ave_input len ("
                << *p_ave_input_len << ") must be strictly positive,"
                << std::endl
                << "and their product "
                <<   static_cast<long>(*p_nr_inputs)
                   * static_cast<long>(*p_ave_input_len)
                << ", the total number of ints to merge," << std::endl
                << "no greater than   " << max_nr_input_ints << "."
                << std::endl;
      usage();
      *p_error = true;
      return;
    }
  } else {
    std::cout << "Incorrect usage.\n" << std::endl;
    usage();
    *p_error = true;
    return;
  }
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
}

// Prints a vector of ints.

void print_iv(std::string name, mm::IntVector v) {
  std::cout << name << " ";
  for (mm::IntVectorConstIterator i = v.begin(); i != v.end(); ++i)
    std::cout << *i << " ";
  std::cout << std::endl;
}

// Test data small enough to verify by hand.

bool verify_small_data() {
  int a1[] = {  2,  6, 88, 688 };
  int a2[] = {  1,  2,  3,   4, 5, 6, 7, 8 };
  int a3[] = {  5, 10, 15,  20 };
  int a123[] = {1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 10, 15, 20, 88, 688};
  mm::IntVector correctOutput(a123, a123 + sizeof(a123) / sizeof(a123[0]));
  mm::IntVector mixed_array[] = {
                            mm::IntVector(a1, a1 + sizeof(a1) / sizeof(a1[0])),
                            mm::IntVector(a2, a2 + sizeof(a2) / sizeof(a2[0])),
                            mm::IntVector(a3, a3 + sizeof(a3) / sizeof(a3[0]))};
  mm::IntVectorVector arrays(mixed_array, mixed_array + sizeof(mixed_array)
                                                      / sizeof(mm::IntVector));
  mm::IntVector output;
  bool retval = true;

  mm::multimerge_pq(arrays, &output);
  print_iv("multimerge pq     small data", output);
  if (output != correctOutput) {
    std::cout << "multimerge pq  differs from correctOutput" << std::endl;
    retval = false;
  }

  mm::multimerge(arrays, &output);
  print_iv("multimerge linear small data", output);
  if (output != correctOutput) {
    std::cout << "multimerge lin differs from correctOutput" << std::endl;
    retval = false;
  }

  return retval;
}

// Generates random integers in a range; used to generate lengths for
// input arrays of test data.

class int_rand_in_range {
 public:
  int_rand_in_range(int amin, int amax, bool do_seed = false) : rmin_(amin),
                      factor_(static_cast<double>(amax - amin) * kInvRandMax) {
    if (do_seed)
      srand(clock());
  }
  int_rand_in_range(const int_rand_in_range &irir) : rmin_(irir.rmin_),
                                                     factor_(irir.factor_) {}
  int_rand_in_range & operator= (const int_rand_in_range &) = delete;

  int operator()() {
    return rmin_ + int (factor_ * static_cast<double>(rand_r(&seed)) + 0.5);
  }

 private:
  constexpr static double kInvRandMax = 1.0 / RAND_MAX;

  int          rmin_;
  double       factor_;
  unsigned int seed;
};

// Sequential integer generator for std::generate, used to generate sequential
// overall input data.

class int_seq {
 public:
  explicit int_seq(int amin) : rmin_(amin) {}
  int_seq(const int_seq &is) : rmin_(is.rmin_) {}
  int_seq & operator= (const int_seq &) = delete;

  int operator()() {
    return rmin_++;  // post-increment
  }

 private:
  int rmin_;
};

// Calculate and print statistics for list of ints, interpreted as
// various lengths centered at aveInputLen; return total length.

int calc_display_stats(mm::IntVector lens, int ave_input_len) {
  int nr_inputs = lens.size();
  int tot_lens  = 0;
  int max_len   = INT_MIN;
  int min_len   = INT_MAX;

  mm::IntVectorIterator lens_it;
  for (lens_it = lens.begin(); lens_it != lens.end(); ++lens_it) {
    int len   = *lens_it;
    tot_lens += len;
    if (max_len < len)
      max_len = len;
    if (min_len > len)
      min_len = len;
  }
  double mean_len =   static_cast<double>(tot_lens)
                    / static_cast<double>(nr_inputs);
  double variance = 0;
  for (lens_it = lens.begin(); lens_it != lens.end(); ++lens_it) {
    double  diff = static_cast<double>(*lens_it) - mean_len;
    variance += (diff * diff);
  }
  std::cout << "nr_inputs "                 << nr_inputs
            << ", requested ave_input_len " << ave_input_len << std::endl
            << "their product "             << nr_inputs * ave_input_len
            << ", actual tot_lens "         << tot_lens << std::endl
            << "mean_len "                  << round(mean_len)
            << ", std dev "                 << round(sqrt(variance
                                              / static_cast<double>(nr_inputs)))
            << ", min_len "                 << min_len
            << ", max_len "                 << max_len << std::endl;

  return tot_lens;
}

// Generate test data: an IntVectorVector of nr_input IntVectors of random
// lengths uniformly distributed from about ave_input_len / 10 to
// 2 * ave_input_len minus that; a range centered at ave_input_len.

void generate_data(int nr_inputs,
                   int ave_input_len,
                   mm::IntVector *p_input_copy,
                   mm::IntVectorVector *p_arrays) {
  int amin = (ave_input_len + 5) / 10;
  if (amin < 1)
    amin = 1;
  int amax = 2 * ave_input_len - amin;

  mm::IntVector lens(nr_inputs, 0);
  std::generate(lens.begin(), lens.end(), int_rand_in_range(amin, amax));

  int tot_lens = calc_display_stats(lens, ave_input_len);

  // Generate the input data by initializing an overall input array
  // sequentially, shuffling it, piecing it out into the input arrays,
  // and sorting each input array.

  mm::IntVector input(tot_lens, 0);
  std::generate(input.begin(), input.end(), int_seq(1));
  *p_input_copy = input;  // for later comparison with merge result
  std::random_shuffle(input.begin(), input.end());

  p_arrays->clear();
  p_arrays->reserve(nr_inputs);
  mm::IntVectorIterator input_it = input.begin();
  for (int i = 0; i < nr_inputs; ++i) {
    p_arrays->push_back(mm::IntVector(lens[i], 0));
    for (mm::IntVectorIterator j = (*p_arrays)[i].begin();
         j != (*p_arrays)[i].end(); ++j) {
      *j = *input_it;
      ++input_it;
    }
    std::stable_sort((*p_arrays)[i].begin(), (*p_arrays)[i].end());
  }
}

// For timing merges; in actual usage s is nonempty for start == false only.

void stopwatch(bool start = true, std::string s = std::string()) {
  static clock_t t_start;
  if (start) {
    t_start = clock();
    if (s.size() > 0)
      std::cout << s << " stopwatch start" << std::endl;
  } else {
    clock_t t_end = clock();
    std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    std::cout.precision(2);
    std::cout << s << " stopwatch end" << std::endl
              << " elapsed " << (t_end - t_start)
              << " clocks at " << CLOCKS_PER_SEC << " per sec, or "
              <<   static_cast<double>(t_end - t_start)
                 / static_cast<double>(CLOCKS_PER_SEC)
              << " sec" << std::endl;
  }
}

// Test program for mmerge.cc.

int testmmerge_main(int argc, char *argv[]) {
  // Obtain parameters for more voluminous test data from the command line,
  // or use the following defaults.

  constexpr int max_nr_input_ints = 500000000;  // 500 million (Ok for 8GB RAM)
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

  mm::IntVector input_copy;
  mm::IntVectorVector arrays;
  generate_data(nr_inputs, ave_input_len, &input_copy, &arrays);

  mm::IntVector output_pq;
  std::cout << "multimerge priority queue" << std::endl;
  stopwatch();
  mm::multimerge_pq(arrays, &output_pq);
  stopwatch(false, "multimerge pq ");
  bool cmp_ok = (output_pq == input_copy);
  if (!cmp_ok)
    retval = false;
  std::cout << "multimerge_pq       "
            << (cmp_ok ? "matches     " : "differs from")
            << " input_copy" << std::endl;

  if (do_multimerge_lin) {
    mm::IntVector output_lin;
    std::cout << "multimerge linear" << std::endl;
    stopwatch();
    mm::multimerge(arrays, &output_lin);
    stopwatch(false, "multimerge lin");
    cmp_ok = (output_pq == input_copy);
    if (!cmp_ok)
      retval = false;
    std::cout << "multimerge (linear) "
              << (cmp_ok ? "matches     " : "differs from")
              << " input_copy" << std::endl;
  }

  return retval ? 0 : -1;
}
