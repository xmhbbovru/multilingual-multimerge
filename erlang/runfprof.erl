#!/usr/bin/env escript
% erlang/test_testmmerge.erl rev. 03 August 2013 by Stuart Ambler.
% Copyright (c) 2013 Stuart Ambler.
% Distributed under the Boost License in the accompanying file LICENSE.

main(_) ->
    fprof:apply(testmmerge, testmmerge, [100, 300, false]).
    fprof:profile(),
    fprof:analyse().
