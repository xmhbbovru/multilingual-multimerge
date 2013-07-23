#!/usr/bin/env ruby
# encoding: utf-8
# Functions to merge sorted arrays of integers.
# ruby/mmerge.rb rev. 22 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.
# Input is in the form of an array of k arrays.  The merge uses either a simple
# algorithm linear in k, or an algorithm that uses a priority queue and is
# logarithmic in k.  Either way, the dependence on n, the total length of all
# the arrays, is linear.  Not much exception or other error handling.
# Assumes PriorityQueue gem has been installed.

module MMerge

# Tried require 'algorithms' Container::PriorityQueue, which necessitated
# also using ValueEnumeratorPair, result over twice as slow as
# require 'priority_queue' without ValueEnumeratorPair.  Tried require 'ferret'
# Ferret::Utils::PriorityQueue, maybe fast since in C, but kept getting
# `insert': method `call' called on terminated object, the PriorityQueue object
# - which I gather may have something to do with a problem involving weak
# references.

require 'priority_queue'

attr_reader :MAXINT
attr_reader :MININT

MAXINT = 2**32 - 1    # to have some reasonable initial value for finding min
MININT = -MAXINT - 1  # to have some reasonable initial value for finding max

# Supports the linear method for k-way merge.
class MMergeLinear
  # Initialize instance variables.
  #   Args: enums array of enumerators (external iterators); used for minptrix.
  def initialize(enums)
    @cache = Array.new enums.size
    @enum_empty = Array.new(enums.size, false)
    i = 0
    enums.each do |enum|
      @cache[i] = enum.next
      i = i + 1
    end
    @cache_min_index = nil
    @cache_min = nil
  end

  # Finds minimum value for any of an array of enumerators,
  #   Args: enums array of enumerators of integers (side-effect: their next
  #         methods are called)
  #   Returns: bool minimum value valid, minimum value.
  # Execution time is linear in the total number of elements the enumerators
  # return.  In finding the minimum value, next is called.  minptrix is
  # designed to be called repeatedly, with the same array of enumerators,
  # by method multimerge, until calling next has raised a StopIteration
  # exception for all the enumerators.
  def minptrix(enums)
    # Since enumerators don't have a peek function, the instance variable
    # cache contains values obtained by calling next on each enumerator, or
    # None if a StopIteration exception was raised by the call.  Instance
    # variable cache_min_index is the index of the enumerator from which the
    # last (candidate) minimum value for this call of minptrix was obtained,
    # and cache_min is that value.

    did_examine = false  # looked at a value (so that minval is valid)
    minval = MMerge::MAXINT
    i = 0
    enums.each do |enum|
      curval = @cache[i]
      unless curval
        if @enum_empty[i]
          i = i + 1
          next
        else
          begin
            @cache[i] = enum.next
          rescue StopIteration
            @enum_empty[i] = true
            i = i + 1
            next
          end
        end
        curval = @cache[i]
      end
      did_examine = true

      if minval > curval
        @cache[i] = nil  # consume the value
        if @cache_min_index  # unconsume if possible
          @cache[@cache_min_index] = @cache_min
        end
        @cache_min_index = i  # store for possible unconsumption
        @cache_min = curval
        minval = curval
      end
      i = i + 1
    end
    @cache_min_index = nil  # keep consumed the minval for this call
    return did_examine, minval
  end
end

# Multimerge, linear in k, the number of arrays.
#   Args: array of arrays.
#   Returns: an array.
# Each element of array must be a sorted array of integers.  On return, output
# is a sorted array containing all the values in all the elements of arrays.
def self.multimerge(arrays)
  i = 0
  tot_nr = 0
  enums = Array.new arrays.size
  arrays.each do |an_array|
    enums[i] = an_array.to_enum
    i = i + 1
    tot_nr += an_array.size
  end

  i = 0
  output = Array.new tot_nr
  mmerge = MMergeLinear.new(enums)
  minval_valid, minval = mmerge.minptrix(enums)
  while minval_valid
    # if wanted a enumerator, could yield here instead of building output
    output[i] = minval
    i = i + 1
    minval_valid, minval = mmerge.minptrix(enums)
  end
  output
end

# Class for objects for priority queue for multimerge.
class ValueEnumeratorPair
  include Comparable

  attr_accessor :value
  attr_accessor :enumerator

  def initialize(val, enum)
    @value      = val
    @enumerator = enum
  end

  def <=>(other)
    return nil unless other.instance_of ValueEnumeratorPair
    @value <=> other.value
  end
end

# Multimerge using priority queue.
#   Args: array of arrays.
#   Returns: an array.
# Each element of array must be a sorted array of integers.  On return, output
# is a sorted array containing all the values in all the elements of arrays.
def self.multimerge_pq(arrays)
  tot_nr = 0
  pq = PriorityQueue.new
  arrays.each do |an_array|
    enum = an_array.to_enum
    curval = enum.next
    pq.push(enum, curval) if curval
    tot_nr += an_array.size
  end

  output = Array.new tot_nr
  i = 0
  until pq.empty?
    enum, curval = pq.delete_min
    # if wanted a enumerator, could yield here instead of building output
    output[i] = curval
    i = i + 1
    begin
      curval = enum.next
      pq.push(enum, curval) if curval
    rescue StopIteration
    end
  end
  output
end

def self.main
  require_relative 'testmmerge.rb'
  TestMMerge.main_func
end

if __FILE__ == $PROGRAM_NAME
  main
end

end
