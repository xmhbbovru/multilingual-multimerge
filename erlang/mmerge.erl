% erlang/mmerge.erl rev. 31 July 2013 by Stuart Ambler.
% Functions to merge sorted lists of integers.
% Copyright (c) 2013 Stuart Ambler.
% Distributed under the Boost License in the accompanying file LICENSE.

% @doc Functions to merge sorted lists of integers.
% Input is in the form of a list of k lists.  The merge uses either a simple
% algorithm linear in k, or an algorithm that uses a priority queue and is
% logarithmic in k.  Either way, the dependence on n, the total length of all
% the lists, is linear.  Not much exception or other error handling.

-module(mmerge).
-export([multimerge/1, multimerge_pq/1]).
-import(lists).
-import(list_iter).
-import(heaps).

% @doc Multimerge using straightforward "linear" method to merge a list of
% iterators to sorted (increasing) lists of integers into one sorted list of
% integers.
%
% @spec multimerge(It_list) -> Out_list
%                  It_list  = [list_iter:list_iterator()]
%                  Out_list = [integer()]
multimerge(It_list) ->
    multimerge(It_list, []).

multimerge(It_list, Out) ->
    % First find the minimum value from all the iterators in It_list.
    % We omit from It_list_new those iterators that are empty.
    {_, End_min_n, End_min_val, It_list_new}
        = lists:foldl(fun(It, {N, Min_n, Min_val, List})
                         -> {V, It_new} = list_iter:next(It, false),
                            if
                                V == iterator_none ->
                                    {N, Min_n, Min_val, List};
                                true ->
                                    List_new = [It_new | List],
                                    if
                                        (Min_n == 0) or (V < Min_val) ->
                                            {N + 1, N + 1, V, List_new};
                                        true ->
                                            {N + 1, Min_n, Min_val, List_new}
                                    end
                            end
                      end,
                      {0, 0, trunc(math:pow(2,31)) - 1, []}, It_list),
    if
        % If the minimum value was never set, all the iterators are empty
        % and we return the result of the recursion.
        End_min_n == 0 ->
            lists:reverse(Out);
        % Otherwise we consume the minimum value (call next(_, true) for its
        % iterator), prepend it to the result, and make a recursive call.
        true ->
            % next has already been called for all iterators in It_list_new,
            % so next(It, false) will return {_, It}.
            {_, It_list_consumed_min}
                = lists:foldl(fun(It, {N, List})
                                 -> {V, _} = list_iter:next(It, false), 
                                    if
                                        (V == iterator_none)
                                        or (N /= End_min_n) ->
                                            {N + 1, [It | List]};
                                        true ->  % consume
                                            {_, It_new}
                                                = list_iter:next(It, true), 
                                            {N + 1, [It_new | List]}
                                    end
                              end,
                              % Need to reverse to get original order, which
                              % is needed for use of End_min_n.
                              {1, []}, lists:reverse(It_list_new)),
            % No need to reverse returned list; will traverse in opposite
            % order with each recursive call, but that will do no harm.
            multimerge(It_list_consumed_min, [End_min_val | Out])
    end.

% @doc Multimerge using a priority queue to merge a list of iterators to
% sorted (increasing) lists of integers into one sorted list of integers.
%
% @spec multimerge_pq(It_list) -> Out_list
%                     It_list  = [list_iter:list_iterator()]
%                     Out_list = [integer()]
multimerge_pq(It_list) ->
    {Q, Nall} = lists:foldl(fun(It, {Q, N}) ->
                                    {V, It_next} = list_iter:next(It, true),
                                    if
                                        V /= iterator_none ->
                                            {heaps:add({V, N, It_next}, Q),
                                             N + 1};
                                        true ->
                                            nil
                                    end
                            end,
                            {heaps:new(), 0}, It_list),
    multimerge_pq({Q, Nall}, []).

% It would reduce dependency on heaps implementation to use heaps:new()
% instead of {prioq, nil, 0} in the next line; but erlang won't allow it.
multimerge_pq({{prioq, nil, 0}, _}, Out) ->
    lists:reverse(Out);
multimerge_pq({Q, Nall}, Out) ->
    {Val, N, It} = heaps:min(Q),
    Q1 = heaps:delete_min(Q),
    {V, It_next} = list_iter:next(It, true),
    if
        V == iterator_none ->
            Q2 = Q1;
        true ->
            Q2 = heaps:add({V, N, It_next}, Q1)
    end,
    multimerge_pq({Q2, Nall}, [Val | Out]).
