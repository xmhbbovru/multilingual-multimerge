% erlang/testmmerge.erl rev. 31 July 2013 by Stuart Ambler.
% Tests erlang/mmerge.erl.
% Copyright (c) 2013 Stuart Ambler.
% Distributed under the Boost License in the accompanying file LICENSE.

% @doc Tests erlang/mmerge.erl.
% Tests multi-way merge of k sorted arrays of int, total length n, in memory,
% using either a simple algorithm linear in k, or instead an algorithm that
% uses a priority queue, logarithmic in k.  Either way, the dependence on n
% is linear.  Minimal error handling.

-module(testmmerge).

-include_lib("eunit/include/eunit.hrl").

-export([testmmerge/3, main/1]).
-import(fprof).
-import(init).
-import(lists).
-import(list_iter).
-import(pqueue).
-import(proplists).
-import(random).
-import(timer).

-import(getopt).
-import(mmerge).

% Prints usage of program.
usage() ->
    S = "Test mmerge.erl k-way merge.~n"
"~n"
"Usage:~n"
"  ./runtestmmerge [-l]~n"
"  ./runtestmmerge <nr_inputs> [-l]~n"
"  ./runtestmmerge <nr_inputs> <ave_input_len> [-l]~n"
"  ./runtestmmerge -h | --help~n"
"~n"
"Arguments:~n"
"  <nr_inputs>      Number of sorted input arrays to generate; must be~n"
"                   positive and its product with ave_input_len no more~n"
"                   than 500 million.  [default: 1000]~n"
"  <ave_input_len>  Desired average length of sorted input arrays to~n"
"                   generate; must be positive and its product with~n"
"                   nr_inputs no more than 500 million [default: 10000].~n"
"~n"
"Options:~n"
"  -h --help        Show this help message and exit.~n"
"  -l               Test slower linear method as well as priority queue method."
"~n",
    io:format(S).

% Prints a string and a list.
print_int_list(S, List) ->
    io:format("~s ", [S]),
    lists:foreach(fun(X) -> io:format("~w ", [X]) end, List),
    io:format("~n"). 

% Test data small enough to verify by hand.
verify_small_data() ->
    L1  = [2,6,88,688],
    L2  = [1,2,3,4,5,6,7,8],
    L3  = [5,10,15,20],
    It1 = list_iter:iterator(L1),
    It2 = list_iter:iterator(L2),
    It3 = list_iter:iterator(L3),
    Out = mmerge:multimerge_pq([It1, It2, It3]),
    print_int_list("multimerge pq     small data", Out),
    It1_lin = list_iter:iterator(L1),
    It2_lin = list_iter:iterator(L2),
    It3_lin = list_iter:iterator(L3),
    Out_lin = mmerge:multimerge([It1_lin, It2_lin, It3_lin]),
    print_int_list("multimerge linear small data", Out_lin),
    Correct = [1,2,2,3,4,5,5,6,6,7,8,10,15,20,88,688],
    if
        (Out == Correct) and (Out_lin == Correct) ->
            true;
        true ->
            if
                Out /= Correct ->
                    io:format("Error in multimerge_pq  with small data~n");
                true ->
                    nil
            end,
            if
                Out_lin /= Correct ->
                    io:format("Error in multimerge lin with small data~n");
                true ->
                    nil
            end,
            false
    end.

% Calculate and print statistics for list of ints, interpreted as
% various lengths centered at aveInputLen; return total length.
calc_display_stats(Nr_inputs, Ave_input_len, Lens) ->
    {Nr, Tot_lens, Tot_sum_sq, Max_len, Min_len}
        = lists:foldl(fun(Len, {N, Sum, Sum_sq, Max, Min})
                         ->{N + 1, Sum + Len, Sum_sq + Len * Len,
                            max(Max, Len), min(Min, Len)}
                      end,
                      {0, 0, 0,
                       -trunc(math:pow(2,31)), trunc(math:pow(2,31)) - 1},
                      Lens),
    io:format("nr_inputs ~w, requested ave_input_len ~w~n"
              "their product ~w, actual tot_lens ~w~n"
              "mean_len ~w, std dev ~.2f, min_len ~w, max_len ~w\~n",
              [Nr_inputs, Ave_input_len, Nr_inputs * Ave_input_len,
              Tot_lens, trunc(Tot_lens / Nr),
              math:sqrt(Tot_sum_sq / Nr - Tot_lens * Tot_lens / (Nr * Nr)),
              Min_len, Max_len]),
    Tot_lens.

% Shuffles a list.
shuffle(List) ->
    [Y || {_, Y} <- lists:sort([{random:uniform(), X} || X <- List])].

% test3 run (from main) with N_arrays 10, 20 40, 100, 1000, Ave_len 10000,
% resulting in multimerge_pq times 0.12, 0.27, 0.81, 3.72, TODO sec.

% Generate test data: an array of nr_input arrays of random lengths uniformly
% distributed from about ave_input_len / 10 to 2 * ave_input_len minus that;
% a range centered at ave_input_len.
generate_data(Nr_inputs, Ave_input_len, Lin) ->
    Amin = trunc(max((Ave_input_len + 5) / 10, 1)),
    Amax = 2 * Ave_input_len - Amin,
    Lens = lists:foldl(fun(_, List)
                          -> [random:uniform(Amax + 1 - Amin) - 1 + Amin | List]
                       end, [], lists:seq(1, Nr_inputs)),
    Tot_lens = calc_display_stats(Nr_inputs, Ave_input_len, Lens),
    Correct_out = lists:seq(1, Tot_lens),
    Input = shuffle(Correct_out),
    {_, Start_lens} = lists:foldl(fun(Len, {Start, List})
                                     -> {Start + Len,
                                         [{Start, Len} | List]}
                                  end,
                                  {1, []}, Lens),
    Temp = lists:foldl(fun({Start, Len}, List)
                          -> [list_iter:iterator(lists:sort(lists:sublist(Input,
                                                                          Start,
                                                                          Len)))
                              | List]
                       end,
                       [], lists:reverse(Start_lens)),
    Arrays = lists:reverse(Temp),
    if
        Lin ->
            Temp_lin = lists:foldl(fun({Start, Len}, List)
                          -> [list_iter:iterator(lists:sort(lists:sublist(Input,
                                                                          Start,
                                                                          Len)))
                              | List]
                                   end,
                                   [], lists:reverse(Start_lens)),
            Arrays_lin = lists:reverse(Temp_lin);
        true ->
            Arrays_lin = []
    end,
    {Arrays, Arrays_lin, Correct_out}.

% @doc testmmerge tests mmerge.erl with specified number and desired average
% input length of input lists to generate for tests.  It always tests
% multimerge_pq, but aside from the small data verification, only tests the
% slower linear method if Lin is true.  It returns a success flag.
%
% @spec testmmerge(Nr_inputs::integer(), Ave_input_len::integer(), 
%                  Lin::boolean()) -> boolean()
testmmerge(Nr_inputs, Ave_input_len, Lin) ->
    Small_retval = verify_small_data(),
    {Arrays, Arrays_lin, Correct_out} = generate_data(Nr_inputs, Ave_input_len,
                                                      Lin),
    io:format("Start multimerge_pq~n"),
    Start_time = erlang:now(),
    Out = mmerge:multimerge_pq(Arrays),
    End_time = erlang:now(),
    io:format("End   multimerge_pq  elapsed ~.2f sec~n",
              [timer:now_diff(End_time, Start_time) * 0.000001]),
    if
        Out == Correct_out ->
            Pq_retval = true,
            Match_str = "matches     ";
        true ->
            Pq_retval = false,
            Match_str = "differs from"
    end,
    io:format("multimerge_pq output ~s input copy~n", [Match_str]),

    if
        Lin ->
            io:format("Start multimerge lin~n"),
            Start_time_lin = erlang:now(),
            Out_lin = mmerge:multimerge(Arrays_lin),
            End_time_lin = erlang:now(),
            io:format("End   multimerge_lin elapsed ~.2f sec~n",
                      [timer:now_diff(End_time_lin, Start_time_lin)
                       * 0.000001]),
            if
                Out_lin == Correct_out ->
                    Lin_retval = true,
                    Match_str_lin = "matches     ";
                true ->
                    Lin_retval = false,
                    Match_str_lin = "differs from"
            end,
            io:format("multimerge lin output ~s input copy~n", [Match_str_lin]);
        true ->
            Lin_retval = true
    end,
    Small_retval and Pq_retval and Lin_retval.

% @doc main function for testmmerge for use via escript, which gives it its
% command line argument.  It returns a success flag.
%
% @spec main(Cmdline::string()) -> boolean()
main(Cmdline) ->
    Def_nr_inputs = 1000,
    Def_ave_input_len = 10000,
    Max_nr_input_ints = 500000000,
    Spec = [{help_only, $h, "help", boolean,
             "Display usage and exit."},
            {nr_inputs, undefined, undefined, {integer, Def_nr_inputs},
             "Number of sorted input arrays to generate."},
            {ave_input_len, undefined, undefined, {integer, Def_ave_input_len},
             "Desired average length of sorted input arrays to generate."},
            {do_multimerge_lin, $l, "do_multimerge_lin", boolean,
             "Test slower linear method in addition to priority queue."}],

    case getopt:parse(Spec, Cmdline) of
        {ok, {Opts, _}} ->
            case proplists:get_bool(help_only, Opts) of
                true ->
                    usage(),
                    true;
                false ->
                    Lin = proplists:get_bool(do_multimerge_lin, Opts),
                    Nr_inputs = proplists:get_value(nr_inputs, Opts),
                    Ave_input_len = proplists:get_value(ave_input_len, Opts),
                    if
                        (Nr_inputs =< 0) or (Ave_input_len =< 0)
                         or (Nr_inputs * Ave_input_len > Max_nr_input_ints) ->
                            io:format("Incorrect usage~n"),
                            usage(),
                            false;
                        true ->
                            % TODO: perhaps do something with return value:
                            testmmerge(Nr_inputs, Ave_input_len, Lin)
                    end
            end;
        {error, _} ->
            io:format("Incorrect usage~n"),
            usage(),
            false
    end.

main_quick_test() ->
    ?assert(main("500 500") == true).

