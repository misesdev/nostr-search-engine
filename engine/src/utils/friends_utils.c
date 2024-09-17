
#ifndef FRIENDS_UTILS_C
#define FRIENDS_UTILS_C

#include "./utils.c"
#include "./http_utils.c"
#include "../types/types.c"
#include "../types/user_list.c"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct FriendNode* createFriendNode(char *pubkey)
{
    struct FriendNode* node = malloc(sizeof(struct FriendNode));
    
    compressPubkey(pubkey, node->address);

    node->next = NULL;

    return node;
}

struct FriendNode* invalidPubkey(cJSON *json, struct FriendNode *friends, char *error) 
{
    strcpy(error, "{ \"message\": \"One invalid pubkey\" }");
    cJSON_Delete(json);
    free(friends);
    
    return NULL;
}

struct FriendNode* jsonToFriends(char *request, char *error)
{
    char *jsonParams = requestParams(request, error);
    
    if(!jsonParams) return NULL;

    cJSON *jsonFriends = cJSON_Parse(jsonParams);

    if(!jsonFriends) {
        strcpy(error, "{ \"message\": \"Error when parsing json, invalid json properties\" }");
        return NULL;
    }
    
    cJSON *pubkey = cJSON_GetObjectItem(jsonFriends, "pubkey");
    cJSON *friends = cJSON_GetObjectItem(jsonFriends, "friends");

    if(!cJSON_IsString(pubkey)) 
    {
        strcpy(error, "{ \"message\": \"Expectd propertie 'pubkey' \" }");
        cJSON_Delete(jsonFriends);
        return NULL;
    }

    if(!cJSON_IsArray(friends)) {
        strcpy(error, "{ \"message\": \"Expectd propertie 'friends', an array with pubkeys\" }");
        cJSON_Delete(jsonFriends);
        return NULL;
    }

    struct FriendNode *userFriends = createFriendNode(pubkey->valuestring);

    int friendsCount = cJSON_GetArraySize(friends);

    struct FriendNode *current = userFriends;

    for(int i = 0; i < friendsCount - 1; i++) 
    {
        cJSON *npub = cJSON_GetArrayItem(friends, i);

        if(!cJSON_IsString(npub))
            return invalidPubkey(jsonFriends, userFriends, error);   

        if(strlen(npub->valuestring) < 64)
            return invalidPubkey(jsonFriends, userFriends, error);   

        current->next = createFriendNode(npub->valuestring);
        current = current->next;
    }

    cJSON_Delete(jsonFriends);

    return userFriends;
}

void insertFriendIfNotExist(User *user, User *friend)
{
    if(!user->friends) {
        user->friends = createUserNode(friend);
        return;
    }

    struct UserNode *current = user->friends;
    while(current)
    {
        if(strcmp(current->user->pubkey, friend->pubkey) == 0)
            break;

        if(!current->next)
            current->next = createUserNode(friend);
            
        current = current->next;
    }
}

#endif