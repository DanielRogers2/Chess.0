/*
 * net.c
 * 
 * 
 * 
 * @author Daniel Rogers
 * 
 */

#include "net.h"

void getStatus(char opponent_move[7], double * tleft, int gameid, int teamnum,
        const char * teamsecret)
{
    //json Result data
    json_t * gamestate;
    json_error_t json_error;

    //ready, time we have left, last move executed
    json_t *ready, *secondsleft, *lastmove;

    //flag for if it's our turn
    bool myturn;

    //Set up the URL
    char url[URL_SIZE];
    sprintf(url, URL_FORMAT, gameid, teamnum, teamsecret);

    //Response from server
    char * response;

#ifdef DEBUG
    puts("inside poll server");
    printf("url: %s\n", url);
#endif

    //Loop until it's our turn
    do
    {
        //Only poll once per second
        Sleep(1000);

        //Ask server for data
        response = pollServer(url);

#ifdef DEBUG
        puts("polled");
#endif

        //Read json data
        gamestate = json_loads(response, 0, &json_error);
        free(response);

#ifdef DEBUG
        puts("json loaded");
#endif

        //Get the state of the game
        ready = json_object_get(gamestate, "ready");

#ifdef DEBUG
        puts("accessed ready");
#endif

        //See if it's our turn
        myturn = json_is_true(ready);

#ifdef DEBUG
        printf("checked turn: %d\n", myturn);
#endif

    } while (!myturn);

    //It's our turn, get the time left and the lastmove
    secondsleft = json_object_get(gamestate, "secondsleft");
    lastmove = json_object_get(gamestate, "lastmove");

#ifdef DEBUG
    puts("got seconds/lastmove");
#endif

    const char * lmove_string;

    //Unpack them
    *tleft = json_real_value(secondsleft);
    lmove_string = json_string_value(lastmove);

#ifdef DEBUG
    printf("unpacked seconds/lastmove: %f, %s\n", tleft, lmove_string);
#endif

    for (uint8_t i = 0; i < 7; ++i)
    {
        opponent_move[i] = lmove_string[i];
    }

#ifdef DEBUG
    puts("copied lastmove");
#endif

    //free root
    json_decref(gamestate);
#ifdef DEBUG
    puts("freed root");
#endif

}

char * pollServer(char * url)
{
    //Open a connection to the server
    CURL * curl;
    CURLcode res;

    //allocate a buffer
    char * buffer = malloc(sizeof(char) * BUFFER_SIZE);

    //Initialize write-response
    write_response result;
    result.data = buffer;
    result.pos = 0;

    curl = curl_easy_init();
    if (curl)
    {
        //Point it to the chess server
        curl_easy_setopt(curl, CURLOPT_URL, url);
        //Use the writeData function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        //Write it to result
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        //Do the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            exit(1);
        }

        //Cleanup
        curl_easy_cleanup(curl);
        //null-terminate
        buffer[result.pos] = '\0';

    }
    else
    {
        puts("Fatal connection error");
        exit(1);
    }

    return (buffer);
}

void pushMove(int gameid, int teamnum, const char * teamsecret, char * move)
{
    //Open a connection to the server
    CURL * curl;
    CURLcode res;

    char url[URL_SIZE];
    sprintf(url, MOVE_URL_FORMAT, gameid, teamnum, teamsecret, move);

#ifdef DEBUG
    printf("moveurl: %s\n", url);
#endif

    curl = curl_easy_init();
    if (curl)
    {
        //Point it to the chess server
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //Do the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            exit(1);
        }

        //Cleanup
        curl_easy_cleanup(curl);
    }
    else
    {
        puts("Fatal connection error");
        exit(1);
    }
}

size_t writeData(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    //Cast for access
    write_response * res = (write_response *) userdata;

    //check for overflow
    if ((res->pos + size * nmemb) > BUFFER_SIZE)
    {
        puts("Buffer too small");
        return (0);
    }

    //Copy data into the struct
    memcpy(res->data + res->pos, ptr, size * nmemb);
    //Update the length tracking
    res->pos += size * nmemb;

    //Return size like curl expects
    return (size * nmemb);
}

