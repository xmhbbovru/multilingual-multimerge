% erlang/list_iter.erl rev. 31 July 2013 by Stuart Ambler.
% Functions for an iterator to a list.
% Copyright (c) 2013 Stuart Ambler.
% Distributed under the Boost License in the accompanying file LICENSE.

% @doc Functions for an iterator to a list.
% Implements an iterator to a list that must not contain the atom
% iterator_none.  Because an iterator has state, and the erlang tuples
% used to represent the iterator are immutable, it may abuse language
% to say "an iterator" when it is represented by tuples returned from the
% functions iterator and next (which returns a new tuple in general,
% though not after having been called once, and used to peek).
%
% The function next, if its Consume argument is false, peeks at the next
% value available from the original list via its iterator argument, which if
% next has been previously called for this iterator, is returned unaltered along
% with the value. If this is the first call to next, it returns a new iterator.
% If next is called with its Consume argument true, it returns an 'incremented'
% iterator along with the value, consuming that element of the list.  If there
% is no value available (if the iterator is empty), it returns the atom
% iterator_none in place of a value from the list.
%
% iter_server/0 is exported only so that a process executing it can be spawned
% for use internal to this module.

% Implementation note extensions to the above comments:

% The various representations of "an iterator" are tied together with the pid
% of the server process used to implement the iterator; but this is supposed
% to be opaque to users of this module.

% The function iterator returns an object not defined to the outside but
% internally implemented as a 4-tuple consisting of the pid for the server
% process created for the iterator, a boolean flag indicating whether the
% iterator is empty (i.e. all of the list has been consumed), a boolean flag
% indicating whether the cache has been initialized, and a list used
% as a cache in the client process for the data of the original list.  The
% cache allows the server to send data to the client in chunks.  In one test,
% sending in each message a 128 element cache resulted in about a 300 times
% speedup compared to sending one item of the original list per message.

% The different behavior (returning an equal vs. a new iterator) of the function
% next when its Consume argument is false, depending on whether there has been
% a previous call to next, allows filling the cache at that time rather than
% during the call to the function iterator.  An alternative implementation
% could call init_cache in the call to iterator, eliminating the need
% subsequently to check whether the cache is initialized and possibly
% initialize it.

% The cache contains list values except that its last value, reached when
% the iterator is empty, will be the iterator_none atom.

-module(list_iter).
-export([iterator/1, next/2, iter_server/0]).
-import(lists).

-define(UNINITIALIZED_CACHE_ITERATOR(Server), {Server, false, false, []}).
-define(INITIALIZED_CACHE_EMPTY_ITERATOR(Server),
                                              {Server, true,  true,  []}).
-define(INITIALIZED_CACHE_NONEMPTY_ITERATOR(Server, Cache),
                                              {Server, false, true,  Cache}).

% @doc iterator returns an iterator to a list.
%
% @type list_iterator()
% @spec iterator(List) -> list_iterator()
%                
iterator(List) ->
    Server = spawn(list_iter, iter_server, []),
    Server ! {self(), init, List},
    ?UNINITIALIZED_CACHE_ITERATOR(Server).

% Initializes (attempts to fill) the cache for an interator, returning a
% new iterator either empty, or with a nonempty Cache.  Must be called
% exactly once for Server from an iterator returned by the function iterator,
% before using the iterator's Cache.
init_cache(Server) ->
    receive
        {Server, Msg} ->
            if
                Msg == [iterator_none] ->
                    ?INITIALIZED_CACHE_EMPTY_ITERATOR(Server);
                true ->
                    ?INITIALIZED_CACHE_NONEMPTY_ITERATOR(Server, Msg)
            end
    end.

% Returns either a value from the cache or iterator_none, in case either
% the cache is empty or the iterator is empty.  It also returns the remainder
% of the cache or an empty list, and flags indicating respectively whether
% the cache is empty (was empty before the call to get_from_cache) and whether
% the iterator is empty (it's possible that when the cache is empty,
% get_from_cache may say the iterator is not empty even though when the cache
% is refilled the next call to get_from_cache may say the iterator is empty).
% We rely on the server sending an iterator_none value after, and only after
% all the list data.
get_from_cache([]) ->
    {iterator_none, [], true, false};  % _, _, Cache_empty, It_empty
get_from_cache([C | Rest]) ->
    if
        C == iterator_none ->
            {C, Rest, true, true};
        true ->
            {C, Rest, false, false}
    end.

% Just to make less nested if and reduce redundancy in next.
consume_or_not(Consume, X, Server, Cache, Rest) ->
    if
        Consume ->
            {X, ?INITIALIZED_CACHE_NONEMPTY_ITERATOR(Server, Rest)};
        true ->
            {X, ?INITIALIZED_CACHE_NONEMPTY_ITERATOR(Server, Cache)}
    end.

% @doc Given an iterator, next returns a value and a new iterator.  The value
% is from the list from which the iterator was constructed, except that it may
% be iterator_none, indicating that there is no list element available; the
% iterator is empty.  If Consume is false, the returned iterator equals the one
% given as an argument; else the returned iterator has been 'incremented' to
% the next list element if any.
%
% @spec next(It::list_iterator(), Consume::boolean) -> {any(), list_iterator()}
next({Server, true, _, _}, _) ->
    {iterator_none, ?INITIALIZED_CACHE_EMPTY_ITERATOR(Server)};
next({Server, _, Cache_initialized, Cache}, Consume) ->
    if
        not Cache_initialized ->
            {_, It_empty, _, New_cache} = init_cache(Server);
        true->
            It_empty = false,
            New_cache = Cache
    end,
    {X, Rest, Cache_empty, It_empty} = get_from_cache(New_cache),
    if
        It_empty ->
            {iterator_none, ?INITIALIZED_CACHE_EMPTY_ITERATOR(Server)};
        Cache_empty ->
            receive
                {Server, Msg} ->
                    {X1, Rest1, _, It_empty1}
                        = get_from_cache(Msg),
                    if
                        It_empty1 ->
                            {iterator_none,
                             ?INITIALIZED_CACHE_EMPTY_ITERATOR(Server)}; 
                        true ->
                            consume_or_not(Consume, X1, Server,
                                           Msg, Rest1)
                    end
            end;
        true ->
            consume_or_not(Consume, X, Server, New_cache, Rest)
    end.

% Sends the elements of List to Client in chunks of at most Chunk_size.
send_chunks(Client, List, Chunk_size) ->
    send_chunk(Client, List, Chunk_size, 1, length(List)).

send_chunk(_, _, _, _, 0) ->
    nil;
send_chunk(Client, List, Chunk_size, Start, Nr_left)
  when Chunk_size =< Nr_left ->
    Client ! {self(), lists:sublist(List, Start, Chunk_size)},
    send_chunk(Client, List,
               Chunk_size, Start + Chunk_size, Nr_left - Chunk_size);
send_chunk(Client, List, _, Start, Nr_left) ->
    Client ! {self(), lists:sublist(List, Start, Nr_left)}.

% @doc iter_server/0 is exported but is only for internal use by this module.
%
% @spec iter_server() -> {pid(), [atom()]}
iter_server() ->
    receive
        {Client, init, List} ->
            nil
    end,
    Chunk_size = 512,
    send_chunks(Client, List, Chunk_size),
    Client ! {self(), [iterator_none]}.
