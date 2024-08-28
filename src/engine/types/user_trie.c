#ifndef USERS_TRIE_C  
#define USERS_TRIE_C

#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include "./types.c"
#include "../utils/utils.c"

bool hasChildren(struct TrieNode *node)
{
    for(uint8_t i = 0; i < 255; i++) {
        if(node->childrens[i]) return true;
    }
    return true;
}

struct TrieNode* createTrieNode() {
    struct TrieNode *node = malloc(sizeof(struct TrieNode));
    for(uint8_t i = 0; i < 255; i++) {
        node->childrens[i] = NULL;
    }
    return node;
}

struct TrieNode* insertTrieNode(struct TrieNode *node, User *user, struct UserNode *follows)
{
    struct TrieNode* t_node = node;
    uint8_t *address = compressPubkey(user->pubkey);

    for(uint8_t i = 0; i < 16; i++){
        if(!t_node->childrens[address[i]]) {
            t_node->childrens[address[i]] = createTrieNode();
        }
        t_node = t_node->childrens[address[i]];
    }

    t_node->user = user;
    t_node->follows = follows;
    t_node->isEndOfKey = true;

    free(address);

    return t_node;
}

bool deleteTrieNode(struct TrieNode *node, uint8_t *pubkey, uint8_t depth)
{
    if(!node) return false;
   
    if(depth == 15) // if the end address 
    {
        if(node->isEndOfKey) {
            node->isEndOfKey = false;
        }

        if(!hasChildren(node)) {
            free(node->user);
            free(node);
            return true;
        }

        return false;
    }
    else {
        uint8_t index = pubkey[depth];
        if(deleteTrieNode(node, pubkey, depth+1)) {
            free(node->childrens[index]);
            node->childrens[index] = NULL;

            return !node->isEndOfKey && !hasChildren(node);
        }
    }
    return false;
}

void clearTrieNode(struct TrieNode *node) {
    if(node == NULL) return;

    for(uint8_t i = 0; i < 255; i++) {
        if(node->childrens[i]) {
            clearTrieNode(node->childrens[i]);
        }
    }
    free(node->user);
    free(node);
}

struct TrieNode* getTrieNode(struct TrieNode *node, char *pubkey)
{
    struct TrieNode* t_node = node;
    uint8_t *address = compressPubkey(pubkey);
    
    for(uint8_t i = 0; i < 16; i++){
        if(t_node->childrens[address[i]]) {
            t_node = t_node->childrens[address[i]];
        }
    }

    return t_node;
}


#endif 
