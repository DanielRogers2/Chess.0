/*
 * net.h
 * 
 * Network code definitions
 * 
 * @author Daniel Rogers
 * 
 */

#ifndef NET_H_
#define NET_H_

#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <stdbool.h>
#include <windows.h>
#include <inttypes.h>

#define URL_FORMAT "http://www.bencarle.com/chess/poll/%d/%d/%s/"
#define MOVE_URL_FORMAT "http://www.bencarle.com/chess/move/%d/%d/%s/%s/"
#define URL_SIZE 256
#define BUFFER_SIZE 256

typedef struct
{
    char * data;
    int pos;
} write_response;

/**
 * Polls the server for updates
 *
 */
void getStatus(char opponent_move[7], double * tleft, int gameid, int teamnum,
        const char * teamsecret);

/**
 * Polls the server for an update
 */
char * pollServer(char * url);

/**
 * Pushes a move to the server
 */
void pushMove(int gameid, int teamnum, const char * teamsecret, char * move);

/**
 * Writes data to buffer
 *
 * based on the github_commits.c example from jansson
 */
size_t writeData(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif /* NET_H_ */
