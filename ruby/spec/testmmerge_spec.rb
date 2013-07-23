# ruby/spec/testmmerge_spec.rb rev. 22 July 2013 by Stuart Ambler.
# Copyright (c) 2013 Stuart Ambler.
# Distributed under the Boost License in the accompanying file LICENSE.

require_relative '../testmmerge.rb'

describe 'testmmerge' do
  it 'main with empty ARGV should return 0' do
    TestMMerge.main_func.should eql(0)
  end
end
