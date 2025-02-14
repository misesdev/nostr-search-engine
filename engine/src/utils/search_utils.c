#ifndef SEARCH_UTILS_C 
#define SEARCH_UTILS_C

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "./string_utils.c"
#include "./http_utils.c"
#include "../types/types.c"

void destroyResultNode(struct ResultNode *node);

Search* jsonToSearchParams(char *json, char *error)
{
    cJSON *jsonParams = cJSON_Parse(json);

    if(!jsonParams) {
        responseMessage(error, "Error when parsing json, expected properties 'pubkey', 'limit' and 'searchTerm'");
        return NULL;
    }

    Search *searchParams = malloc(sizeof(Search));
    
    cJSON *search = cJSON_GetObjectItemCaseSensitive(jsonParams, "searchTerm");
    cJSON *pubkey = cJSON_GetObjectItemCaseSensitive(jsonParams, "pubkey");
    cJSON *limit = cJSON_GetObjectItem(jsonParams, "limit");
    if(cJSON_IsString(search) && (search->valuestring != NULL) && 
        cJSON_IsString(pubkey) && (pubkey->valuestring != NULL) 
    ) 
    {
        snprintf(searchParams->pubkey, 65, "%s", pubkey->valuestring);
        snprintf(searchParams->search, 100, "%s", search->valuestring);  
        if(cJSON_IsNumber(limit) && limit->valueint > 0)
            searchParams->limit = limit->valueint > MAX_LIMIT_RESULTS ? MAX_LIMIT_RESULTS : limit->valueint;
        else
            searchParams->limit = 100;
    } else {
        responseMessage(error, "Error when parsing search, expected properties 'pubkey', 'limit' and 'searchTerm'");
        cJSON_Delete(jsonParams);
        free(searchParams);
        return NULL;
    }

    cJSON_Delete(jsonParams);

    return searchParams;
}

struct ResultNode* createResultNode(User *user, float similarity)
{
    struct ResultNode *node = malloc(sizeof(struct ResultNode));
    node->similarity = similarity;
    node->user = user;
    node->next = NULL;

    return node;
}

void insertResultNode(struct ResultNode *rootUsers, User *user, float similarity)
{
    if(!rootUsers->user)
    {
        rootUsers->similarity = similarity;
        rootUsers->user = user;
        return;
    }

    struct ResultNode *current = rootUsers;
    while (current)
    {
        if(strcmp(current->user->pubkey, user->pubkey) == 0) return;

        if(!current->next) {
            current->next = createResultNode(user, similarity);
            return;
        }

        current = current->next;
    }
}

cJSON* resultUserToCJSON(User *user, float similarity)
{
    cJSON *userJson = cJSON_CreateObject();

    cJSON_AddStringToObject(userJson, "name", user->name);
    cJSON_AddStringToObject(userJson, "displayName", user->displayName);
    cJSON_AddStringToObject(userJson, "pubkey", user->pubkey);
    cJSON_AddStringToObject(userJson, "profile", user->profile);
    cJSON_AddNumberToObject(userJson, "similarity", (int)(similarity * 100));

    return userJson;
}

void resultToJson(struct ResultNode *rootUsers, char *response)
{
    cJSON *jsonList = cJSON_CreateArray();

    struct ResultNode *current = rootUsers;
    while(current) 
    {
        if(current->user)
            cJSON_AddItemToArray(jsonList, resultUserToCJSON(current->user, current->similarity));

        current = current->next;
    }

    snprintf(response, MAX_RESPONSE_LENGTH, "%s", cJSON_Print(jsonList));

    destroyResultNode(rootUsers);

    cJSON_Delete(jsonList);
}

struct ResultNode* searchOnFriends(User *user, char *searchTerm, int limit)
{
    struct ResultNode *rootUsers = createResultNode(NULL, 0);

    int foundCount = 0;
    struct UserNode *current = user->friends;
    while(current)
    {
        if(foundCount >= limit) break;

        float similarity = textSimilarity(current->user->displayName, searchTerm);
        
        if(similarity >= MIN_SIMILARITY_TERM) {
            insertResultNode(rootUsers, current->user, similarity);
            foundCount++;
        }

        if(foundCount >= limit) break;
        
        current = current->next;
    }

    return rootUsers;
}

void destroyResultNode(struct ResultNode *node) 
{
    struct ResultNode *delete;

    while(node->next) 
    {
        delete = node->next;
        node->next = delete->next;
        free(delete);
    }

    free(node);
}


#endif


