#ifndef USER_LIST_C
#define USER_LIST_C

#include <stdlib.h>
#include <string.h>
#include "./types.c"

struct UserNode* createUserNode(User *user) 
{
    struct UserNode *node = malloc(sizeof(struct UserNode));
    node->user = user;
    node->next = NULL;
    return node;
}

void insertUserNode(struct UserNode *node, User *user)
{
    if(!node->user) 
    {
        node->user = user;
        return;
    }

    struct UserNode *current = node;
    while (current) 
    {
        if(strcmp(current->user->pubkey, user->pubkey) == 0)
            return;

        if(!current->next)
        {
            current->next = createUserNode(user);
            return;
        }

        current = current->next;
    }
}

void deleteUserNode(struct UserNode *node, char *pubkey)
{
    struct UserNode *previous = node;
    struct UserNode *current = node->next;

    while (current) 
    {
        if(strcmp(pubkey, current->user->pubkey)) 
        {
            previous->next = current->next;
            free(current->user);
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
}

void destroyUserNode(struct UserNode *node) 
{
    struct UserNode *delete;

    while(node->next) 
    {
        delete = node->next;
        node->next = delete->next;
        if(delete->user) free(delete->user);
        free(delete);
    }
    
    if(node->user) free(node->user);
    free(node);
}

User* getUser(struct UserNode *node, char *pubkey)
{
    struct UserNode* current = node;

    while(current) 
    {
        if(strcmp(current->user->pubkey, pubkey)) return current->user;
        current = current->next;
    }

    return NULL;
}

#endif


