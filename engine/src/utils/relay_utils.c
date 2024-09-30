#ifndef RELAY_UTILS_C
#define RELAY_UTILS_C

#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>

#include "../types/types.c"
#include "http_utils.c"
#include "string_utils.c"
#include "../types/relay_list.c"

char* jsonToRelay(char *jsonRequest, char *error)
{
    cJSON *json = cJSON_Parse(jsonRequest);

    if(!json) {
        responseMessage(error, "Errr expected a json params");
        return NULL;
    }

    cJSON *relay = cJSON_GetObjectItem(json, "relay");
    
    if(!cJSON_IsString(relay)) {
        responseMessage(error, "Invalid json, expected property 'relay'");
        cJSON_Delete(json);
        return NULL;
    }

    if(strlen(relay->valuestring) <= 5) {
        responseMessage(error, "Invalid relay address");
        cJSON_Delete(json);
        return NULL;
    }

    char *relay_address = calloc(RELAY_SIZE, sizeof(char));
    snprintf(relay_address, RELAY_SIZE, "%s", relay->valuestring);

    cJSON_Delete(json);

    return relay_address;
}

char* jsonToSearchTerm(char *jsonRequest, char *error)
{
    cJSON *json = cJSON_Parse(jsonRequest);

    if(!json) {
        responseMessage(error, "Error expected json params with property 'searchTerm'");
        return "";
    }

    cJSON *searchTerm = cJSON_GetObjectItemCaseSensitive(json, "searchTerm");
    
    if(!cJSON_IsString(searchTerm)) {
        responseMessage(error, "Invalid json, expected property 'searchTerm'");
        cJSON_Delete(json);
        return "";
    }

    if(strlen(searchTerm->valuestring) <= MIN_SEARCH_TERM) {
        responseMessage(error, "Please the searchTerm <= 2");
        cJSON_Delete(json);
        return "";
    }

    char *search = calloc(MAX_SEARCH_TERM, sizeof(char));
    snprintf(search, MAX_SEARCH_TERM, "%s", searchTerm->valuestring);

    cJSON_Delete(json);

    return search;
}

struct RelayNode* searchRelays(struct RelayNode *root, char *searchTerm)
{
    struct RelayNode *resultList = createRelayNode("");

    struct RelayNode *current = root;    
    while (current) 
    {
        if(textSimilarity(current->address, searchTerm) >= MIN_SIMILARITY_TERM_RELAY) {
            insertRelayNode(resultList, current->address);
        }

        current = current->next;
    }

    return resultList;
}

void serializeRelayNode(struct RelayNode *root, char *response)
{
    cJSON *arrayList = cJSON_CreateArray();

    if(!arrayList) {
        responseMessage(response, "Memory error");
        destroyRelayNode(root);
        return;
    }

    struct RelayNode *current = root;
    while(current)
    {
        if(strlen(current->address) > 0) 
        {
            cJSON *object = cJSON_CreateObject();
            cJSON *relay_address = cJSON_CreateString(current->address);
            cJSON_AddItemToObject(object, "relay", relay_address);

            cJSON_AddItemToArray(arrayList, object);
        }

        current = current->next;
    }

    snprintf(response, MAX_RESPONSE_LENGTH, "%s", cJSON_Print(arrayList));
    
    cJSON_Delete(arrayList);
    
    destroyRelayNode(root);
}

#endif

