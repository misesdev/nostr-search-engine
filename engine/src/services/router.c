#ifndef ROUTER_C
#define ROUTER_C

#include "../types/types.c"
#include "./endpoints/up_users.c"
#include "./endpoints/up_search.c"
#include "./endpoints/up_friends.c"
#include "./endpoints/tree_save.c"
#include "./endpoints/get_user.c"
#include "./endpoints/get_friends.c"
#include "./endpoints/search_friends.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HttpResponse* router(char *request, struct TrieNode *root) 
{
    HttpResponse *response = calloc(1, sizeof(HttpResponse));

    char *jsonRequest = requestParams(request, response->Content);

    if(!jsonRequest) 
    {
        response->StatusCode = 403;
        return response;
    }

    // insert user on tree
    if(strstr(request, "/add_user") != NULL)
        return insertUser(jsonRequest, response, root);
    // get user from tree with pubkey
    if(strstr(request, "/get_user") != NULL)
        return getUserWithPubkey(jsonRequest, response, root);
    // get friends from user
    if(strstr(request, "/search_friends") != NULL)
        return searchFriends(jsonRequest, response, root);      // search on graph users
    if(strstr(request, "/search") != NULL)
        return searchUsers(jsonRequest, response, root);
    // add friends of users on graph user
    if(strstr(request, "/add_friends") != NULL)
        return insertFriends(jsonRequest, response, root);
    // get friends from user
    if(strstr(request, "/get_friends") != NULL)
        return getFriendsFromUser(jsonRequest, response, root);     
    // save the tree on disk
    if(strstr(request, "/save") != NULL)
        return saveTrieOnDisk(jsonRequest, response, root);

    responseMessage(response->Content, "Please use one of the following endpoints: "
        "/add_user, /add_friends, /get_user, /get_friends, /search and /search_friends");

    response->StatusCode = 403;

    return response;
}

#endif
