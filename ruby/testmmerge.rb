#!/usr/bin/env ruby
# encoding: utf-8
# Unit and timing test for ruby/mmerge.rb.
# ruby/testmmerge.rb rev. 23 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

module TestMMerge

require 'docopt'
require_relative 'mmerge.rb'

# Process command-line arguments.
#   Args: max_nr_input_ints maximum total number of elements to be merged.
#   Returns: nr_inputs number of sorted input arrays to be merged
#            ave_input_len requested average length of such
#            lin_multimerge additionally use the linear merge method.
def self.get_cfg(max_nr_input_ints, default_nr_inputs,
                 default_ave_input_len)
  doc = <<DOCOPT
Test mmerge.rb k-way merge.

Usage:
  #{__FILE__} [-l]
  #{__FILE__} <nr_inputs> [-l]
  #{__FILE__} <nr_inputs> <ave_input_len> [-l]
  #{__FILE__} -h | --help

Arguments:
  <nr_inputs>      Number of sorted input arrays to generate; must be
                   positive and its product with ave_input_len no more
                   than 500 million.  [default: 1000]
  <ave_input_len>  Desired average length of sorted input arrays to
                   generate; must be positive, and its product
                   with nr_inputs no more than 500 million.  [default: 10000]

Options:
  -h --help        Show this help message and exit.
  -l               Test slower linear method as well as priority queue method.

DOCOPT
  begin
    dict = Docopt.docopt(doc)

    nr_inputs = dict['<nr_inputs>']
    nr_inputs = default_nr_inputs.to_s unless nr_inputs
    nr_inputs = Integer(nr_inputs) rescue nil

    ave_input_len = dict['<ave_input_len>']
    ave_input_len = default_ave_input_len.to_s unless ave_input_len
    ave_input_len = Integer(ave_input_len) rescue nil

    lin_multimerge = dict['-l']

    if !nr_inputs || nr_inputs <= 0 || !ave_input_len || ave_input_len <= 0
      if nr_inputs && ave_input_len
        if (calc_tot = nr_inputs * ave_input_len) > max_nr_input_ints
          puts "The product of nr_inputs and ave_input_len, #{calc_tot},"
    puts "the total number of ints to merge, can't exceed #{max_nr_input_ints}"
        end
      end
      puts doc
      exit 1
    end
    return nr_inputs, ave_input_len, lin_multimerge
  rescue Docopt::Exit  # => e
    puts doc  # could just puts e
    exit 1
  end
end

# Test data small enough to verify by hand.
#   Args: none.
#   Returns: true if merge output Ok
def self.verify_small_data
  aint1 = [2,  6, 88, 688]
  aint2 = [1,  2,  3,   4, 5, 6, 7, 8]
  aint3 = [5, 10, 15,  20]
  arrays = [aint1, aint2, aint3]
  correct_output = [1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 10, 15, 20, 88, 688]

  retval = true
  output = MMerge.multimerge_pq(arrays)
  puts 'multimerge pq     small data'
  p output
  if output != correct_output
    puts 'multimerge pq  differs from correct_output'
    retval = false
  end

  output = MMerge.multimerge(arrays)
  puts 'multimerge linear small data'
  p output
  if output != correct_output
    puts 'multimerge lin differs from correct_output'
    retval = false
  end

  retval
end

# Calculate and print statistics for array of ints.
#   Args: lens array of ints
#         ave_input_len requested average of the ints.
#   Returns: total of the ints.
# Variable names and print text assumes that the ints are lengths (lens).
def self.calc_display_stats(lens, ave_input_len)
  nr_inputs = lens.size
  tot_lens = 0
  max_len  = MMerge::MININT
  min_len  = MMerge::MAXINT
  lens.each do |length|
    tot_lens = tot_lens + length
    max_len = length if max_len < length
    min_len = length if min_len > length
  end

  mean_len = tot_lens / nr_inputs
  variance = 0
  lens.each do |length|
    diff = length - mean_len
    variance = variance + (diff * diff)
  end

  puts "nr_inputs #{nr_inputs}, requested ave_input_len #{ave_input_len}"
  puts "product #{nr_inputs * ave_input_len}, actual tot_lens #{tot_lens}"
  printf("mean_len %d, std dev %d, min_len %d, max_len %d\n",
         mean_len, Math.sqrt(variance / nr_inputs).round, min_len, max_len)

  tot_lens
end

# Generate test data.
#   Args: nr_inputs number of sorted input arrays to generate  ,
#         ave_input_len average length of such arrays.
#   Returns: inarray_copy array containing expected output,
#            arrays a array of arrays to use as input to the merge functions.
# Generate array of random lengths for input arrays uniformly distributed
# from about ave_input_len / 10 to 2 * ave_input_len minus that a range
# centered at ave_input_len.  Display length statistics.
def self.generate_data(nr_inputs, ave_input_len)
  amin = (ave_input_len + 5) / 10
  if amin < 1
    amin = 1
  end
  amax = 2 * ave_input_len - amin

  lens = Array.new nr_inputs
  (0..(nr_inputs - 1)).each { |i| lens[i] = amin + rand(amax + 1 - amin) }

  tot_lens = self.calc_display_stats(lens, ave_input_len)

  # Generate the input data by initializing an overall input array
  # sequentially, shuffling it, piecing it out into the input arrays,
  # and sorting each input array.

  inarray = Array.new tot_lens
  (1..tot_lens).each { |i| inarray[i - 1] = i }
  inarray_copy = inarray.clone
  inarray.shuffle

  arrays = Array.new nr_inputs
  in_ix = 0
  ar_ix = 0
  lens.each do |length|
    a_array = inarray[in_ix...(in_ix + length)]
    in_ix = in_ix + length
    a_array.sort
    arrays[ar_ix] = a_array
    ar_ix = ar_ix + 1
  end

  return inarray_copy, arrays
end

# Times intervals.
#   Args: start_time is a array to allow modification as well as retrieval,
#         bool start true to start timer, false to stop
#         label to display.
#   Returns: nothing (side effect sets start_time if start).
# For timing merges in actual usage s is nonempty only if not start.
def self.stopwatch(start_time, start, label)
  if start
    puts "#{label} stopwatch start"
    start_time[0] = Time.now.to_f
  else
    end_time = Time.now.to_f
    printf("%s stopwatch end elapsed %.2f sec\n", label,
           end_time - start_time[0])
  end
end

# Test k-way merge; called by main.
#   Args: none.
#   Returns: nothing.
def self.main_func
  max_nr_input_ints = 500_000_000  # (Ok for 8GB RAM)
  default_nr_inputs = 1000
  default_ave_input_len = 10000
  nr_inputs, ave_input_len, lin_multimerge = self.get_cfg(
                                                         max_nr_input_ints,
                                                         default_nr_inputs,
                                                         default_ave_input_len)

  retval = self.verify_small_data  # set false if error in merge here or later

  # Do the larger tests.

  inarray_copy, arrays = self.generate_data(nr_inputs, ave_input_len)

  start_time = [0]  # a array to provide for setting inside stopwatch

  self.stopwatch(start_time, true, 'multimerge pq')
  output = MMerge.multimerge_pq(arrays)
  self.stopwatch(start_time, false, 'multimerge pq')
  mmerge_output_ok = (output == inarray_copy)
  retval = false unless mmerge_output_ok
  diff_txt = mmerge_output_ok ? 'matches     ' : 'differs from'
  puts "multimerge pq  #{diff_txt} inarray_copy"

  if lin_multimerge
    self.stopwatch(start_time, true, 'multimerge lin')
    output = MMerge.multimerge(arrays)
    self.stopwatch(start_time, false, 'multimerge lin')
    mmerge_output_ok = (output == inarray_copy)
    retval = false unless mmerge_output_ok
    diff_txt = mmerge_output_ok ? 'matches     ' : 'differs from'
    puts "multimerge pq  #{diff_txt} inarray_copy"
  end

  retval ? 0 : -1
end

# testmmerge main to test k-way merges.
# Args: none
# Returns: exit code
# Usage:  ./testmmerge.rb [args to be processed by main_func]
def self.main
  main_func
end

if __FILE__ == $PROGRAM_NAME
  main
end

end
