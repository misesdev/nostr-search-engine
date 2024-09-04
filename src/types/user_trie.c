#ifndef USERS_TRIE_C  
#define USERS_TRIE_C

#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include "./types.c"
#include "../utils/utils.c"

bool hasChildren(struct TrieNode *node)
{
    for(uint8_t i = 0; i < TRIE_CHILDREN_LENGTH; i++) {
        if(node->children[i]) return true;
    }
    return true;
}

struct TrieNode* createTrieNode() {
    struct TrieNode *node = malloc(sizeof(struct TrieNode));
    for(uint8_t i = 0; i < TRIE_CHILDREN_LENGTH; i++) {
        node->children[i] = NULL;
    }
    node->isEndOfKey = false;
    node->user = NULL;
    return node;
}

struct TrieNode* insertTrieNode(struct TrieNode *node, User *user)
{
    struct TrieNode* t_node = node;
    uint8_t address[PUBKEY_ADDRESS_LENGTH];
    compressPubkey(user->pubkey, address);

    for(uint8_t i = 0; i < PUBKEY_ADDRESS_LENGTH; i++){
        if(!t_node->children[address[i]]) {
            t_node->children[address[i]] = createTrieNode();
        }
        t_node = t_node->children[address[i]];
    }

    t_node->user = user;
    t_node->isEndOfKey = true;

    return t_node;
}

bool deleteTrieNode(struct TrieNode *node, uint8_t *pubkey, uint8_t depth)
{
    if(!node) return false;
   
    if(depth == PUBKEY_ADDRESS_LENGTH - 1) // if the end address 
    {
        if(node->isEndOfKey) node->isEndOfKey = false;

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
            free(node->children[index]);
            node->children[index] = NULL;

            return !node->isEndOfKey && !hasChildren(node);
        }
    }
    return false;
}

void destroyTrieNode(struct TrieNode *node) {
    if(node == NULL) return;

    for(uint8_t i = 0; i < TRIE_CHILDREN_LENGTH; i++) {
        if(node->children[i]) {
            destroyTrieNode(node->children[i]);
        }
    }
    if(!hasChildren(node)) {
        if(node->isEndOfKey) free(node->user);
        free(node);
    }
}

struct TrieNode* getTrieNode(struct TrieNode *node, uint8_t address[PUBKEY_ADDRESS_LENGTH])
{
    struct TrieNode* t_node = node;
    
    for(uint8_t i = 0; i < PUBKEY_ADDRESS_LENGTH; i++){
        if(t_node->children[address[i]]) {
            t_node = t_node->children[address[i]];
        }
    }

    if(t_node->isEndOfKey) return t_node;

    return NULL;
}

struct TrieNode* getTrieNodeFromPubkey(struct TrieNode *node, char *pubkey)
{
    struct TrieNode* t_node = node;
    uint8_t address[PUBKEY_ADDRESS_LENGTH];
    compressPubkey(pubkey, address);
    
    for(uint8_t i = 0; i < PUBKEY_ADDRESS_LENGTH; i++){
        if(t_node->children[address[i]]) {
            t_node = t_node->children[address[i]];
        }
    }

    return t_node;
}



#endif

