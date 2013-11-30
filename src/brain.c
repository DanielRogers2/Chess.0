/*
 * brain.c
 * 
 * Contains the implementations of the functions in brain.h
 * 
 * @author Daniel Rogers
 * 
 */

#include <stdbool.h>

#ifdef PARALLEL_NEGAMAX
#include <omp.h>
#endif

#include "brain.h"

/*
 * Does a search for and returns the best move for the board
 *
 * @owner Daniel Rogers
 *
 * @param self_white true if we are white
 * @param initial A pointer to the initial board state to use
 * @param result A pointer that will be filled with the new board state based
 *               on the function's selected best move. last_piece and last_move
 *               will be set to the value of the piece & location to move it to
 * @param depth The depth to search to
 */
void selectBestMove(bool self_white, chessboard * const initial,
        chessboard * result, uint8_t depth)
{
#ifdef DEBUG
    puts("setting up storage");
#endif

    //Make the expansion store
#ifndef PARALLEL_NEGAMAX
    boardset * store = calloc(depth, sizeof(boardset));
#else
    int thread;

#ifdef USE_MAX_THREADS
    int threadcount = omp_get_max_threads();
#ifdef DEBUG
    printf("MAX_THREADS: %d\n", threadcount);
#endif
#else
    int threadcount = 4;
#endif

    omp_set_num_threads(threadcount);
    boardset * threadstore[threadcount];
    for (uint8_t i = 0; i < threadcount; ++i)
    {
        threadstore[i] = calloc(depth - 1, sizeof(boardset));
    }

    boardset store;
    store.count = 0;
    store.data = NULL;
#endif

#ifdef DEBUG
    puts("doing initial expansion");
#endif

    //Do the first expansion
#ifndef PARALLEL_NEGAMAX
    uint8_t states = expandStates(initial, &store[0], self_white);
#else
    uint8_t states = expandStates(initial, &store, self_white);
#endif

    //Best value seen
#ifndef PARALLEL_NEGAMAX
    int best = INT_MIN;
    uint8_t best_indx = 0;
#else
    int best[threadcount];
    uint8_t best_indx[threadcount];
    for (uint8_t i = 0; i < threadcount; ++i)
    {
        best[i] = INT_MAX;
        best_indx[i] = 0;
    }
#endif

    //Currently seen value
    int cur;

#ifdef DEBUG
    puts("staring search");
#endif

    //Do the search
#ifdef PARALLEL_NEGAMAX
#pragma omp parallel for private(cur, thread) \
    shared(best_indx, best, states, store, threadstore, self_white, depth)
#endif
    for (uint8_t i = 0; i < states; ++i)
    {
        //Already did depth 0, so do the rest
        //  depth 0 values won't be over-written, so we can just look up
        //  the data we want after the loop ends
#ifndef PARALLEL_NEGAMAX
        cur = -negamax(&store[0].data[i], !self_white, &store[1], depth - 1);
        if (cur > best)
        {
            best = cur;
            best_indx = i;
        }
#else
        thread = omp_get_thread_num();
        cur = -negamax(&store.data[i], !self_white, threadstore[thread],
                depth - 1);
        if (cur > best[thread])
        {
            best[thread] = cur;
            best_indx[thread] = i;
        }
#endif
#ifdef DEBUG
        printf("tl @ %d of %d\n", i + 1, states);
#endif
    }

#ifdef DEBUG
    puts("search complete");
#endif

#ifndef PARALLEL_NEGAMAX
    //Get best board state
    memcpy(result, &store[0].data[best_indx], sizeof(chessboard));
    //Free used memory
    for (uint8_t i = 0; i < depth; ++i)
    {
        free(store[i].data);
    }
    free(store);
#else
    //Get thread best
    int th_best = best[0];
    uint8_t th_best_i = best_indx[0];

    for (uint8_t i = 1; i < threadcount; ++i)
    {
        if (best[i] > th_best)
        {
            th_best = best[i];
            th_best_i = best_indx[i];
        }
    }

    //copy into result
    memcpy(result, &store.data[th_best_i], sizeof(chessboard));

#ifdef DEBUG
    puts("freeing thread mem");
#endif
    //Free used memory
    for (uint8_t j = 0; j < threadcount; ++j)
    {
        for (uint8_t i = 0; i < depth - 1; ++i)
        {
            free(threadstore[j][i].data);
        }
        free(threadstore[j]);
    }
#endif
}

/*
 * Performs a standard negamax search
 *
 * The caller is responsible for doing the first expansion & call to negamax
 *  for each root node, as well as determining which root node to use.
 *
 * @owner Daniel Rogers
 *
 * @param state A pointer to the start state for the layer
 * @param white If the current layer of the search is from white or black's
 *          perspective
 * @param expansionStore An array of boardsets to use for storing expanded
 *                       states. These may be realloc'd during execution of
 *                       the search.
 *                       !!! It is assumed to contain at least as many
 *                       pointers as the maximum depth of the search. Supplying
 *                       less will result in out of bounds memory access. !!!
 * @param depth The depth to traverse to
 *
 * @return The best score resulting from the negamax search
 */
int negamax(chessboard * const state, bool white, boardset * expansionStore,
        uint8_t depth)
{
    if (depth)
    {
        //Best value seen
        int best = INT_MIN;
        //Currently seen value
        int cur;
        //Storage of expanded states
        boardset * storage = &expansionStore[depth - 1];

        //Do expansion, store result
        uint8_t states = expandStates(state, storage, white);

        //recurse negamax for each state expanded
        for (uint8_t i = 0; i < states; ++i)
        {
            cur = -negamax(&storage->data[i], !white, expansionStore,
                    depth - 1);
            if (cur > best)
            {
                best = cur;
            }
        }

        //Return the best value
        return (best);
    }
    else
    {
        //Return value of state
        return (evaluateState(state, white));
    }
}
