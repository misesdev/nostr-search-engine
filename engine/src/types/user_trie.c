#ifndef USERS_TRIE_C  
#define USERS_TRIE_C

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "./types.c"
#include "./user_trie_list.c"
#include "../utils/utils.c"

struct TrieNode* createTrieNode(uint8_t key) {
    struct TrieNode *node = malloc(sizeof(struct TrieNode));
    node->isEndOfKey = false;
    node->childrens = NULL;
    node->user = NULL;
    node->key = key;
    return node;
}

struct TrieNode* insertTrieNode(struct TrieNode *root, User *user)
{
    uint8_t key[ADDRESS_LENGTH];
    compressPubkey(user->pubkey, key);
    struct TrieNode *current = root;

    for(uint8_t i = 0; i < ADDRESS_LENGTH; i++)
    {     
        struct TrieNode *nextNode = NULL;
        struct TrieList *list = current->childrens;

        while(list) {
            if(list->node->key == key[i]) {
                nextNode = list->node;
                break;
            }
            list = list->next;
        }

        if(nextNode == NULL) {
            nextNode = createTrieNode(key[i]);
            struct TrieList *newChildren = createNode(nextNode);
            newChildren->next = current->childrens;
            current->childrens = newChildren;
        }
        
        current = nextNode;
    }
    
    if(current->user) {
        strcpy(current->user->name, user->name);
        strcpy(current->user->displayName, user->displayName);
        strcpy(current->user->pubkey, user->pubkey);
        strcpy(current->user->profile, user->profile);
        free(user);
    } else {
        current->isEndOfKey = true;
        current->user = user;
    }

    return current;
}

bool deleteTrieNode(struct TrieNode *current, uint8_t *key)
{
    if(*key) 
    {
        struct TrieList *prev = NULL;
        struct TrieList *list = current->childrens;
        // finds the next node by key
        while(list) 
        {
            if(list->node->key == *key) {
                if(deleteTrieNode(list->node, key + 1)) {
                    // if the node not have childrens, remove the link
                    if(list->node->isEndOfKey && list->node->childrens == NULL) {
                        free(list->node);
                        if(prev != NULL) {
                            prev->next = list->next;
                        } else {
                            current->childrens = list->next;
                        }
                        free(list);
                        return true;
                    }
                }
                return false;
            }
            prev = list;
            list = list->next;
        }
    } 
    else if(current->isEndOfKey) 
    {
        current->isEndOfKey = false;
        if(current->childrens == NULL) 
            return true;
    }

    return false;
}

void destroyTrieNode(struct TrieNode *root) 
{
    struct TrieList *list = root->childrens;

    while(list) {
        struct TrieList *temp = list;
        destroyTrieNode(list->node);
        list = list->next;
        free(temp);
    }

    if(!hasChildren(root) && root->isEndOfKey)
        free(root->user);

    free(root);
}

struct TrieNode* getTrieNode(struct TrieNode *root, uint8_t *key)
{
    struct TrieNode* current = root;
   
    for(uint8_t i = 0; i < ADDRESS_LENGTH; i++) 
    {
        struct TrieNode *nextNode = NULL;
        struct TrieList *list = current->childrens;

        while(list) 
        {
            if(list->node->key == key[i]) {
                nextNode = list->node;
                break;
            }
            list = list->next;
        }

        if(!nextNode) return NULL;
                
        current = nextNode;   
    }

    if(current->isEndOfKey) 
        return current;

    return NULL;
}

struct TrieNode* getTrieNodeFromPubkey(struct TrieNode *root, char *pubkey)
{
    uint8_t address[ADDRESS_LENGTH];
    
    compressPubkey(pubkey, address);
    
    return getTrieNode(root, address);
}


#endif

