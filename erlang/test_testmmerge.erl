#!/usr/bin/env escript
% erlang/test_testmmerge.erl rev. 31 July 2013 by Stuart Ambler.
% Copyright (c) 2013 Stuart Ambler.
% Distributed under the Boost License in the accompanying file LICENSE.

-import(eunit).

main(_) ->
    eunit:test({timeout, 20, testmmerge}).
