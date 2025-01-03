#ifndef TYPES_C
#define TYPES_C

#include "../constants/index.c"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char name[45]; // 45 bytes + byte null end
    char pubkey[65]; // 64 bytes + byte null end
    char profile[150];
    char displayName[45];
    struct UserNode *friends;
    uint32_t index;
} User;

struct UserNode {
    User *user;
    struct UserNode *next;
};

struct TrieNode {
    User *user;
    uint8_t key;
    bool isEndOfKey;
    struct TrieList *childrens;
};

struct TrieList {
    struct TrieNode *node;
    struct TrieList *next;
};

typedef struct {
    char pubkey[65];
    char search[100];
    int limit;
} Search;

typedef struct {
    int StatusCode;
    char Content[MAX_RESPONSE_LENGTH];
} HttpResponse;

struct FriendNode {
    uint8_t address[ADDRESS_LENGTH];
    struct FriendNode *next;
};

typedef struct {
    char pubkey[65];
} UserIdentity;


struct ResultNode {
    User *user;
    float similarity;
    struct ResultNode *next;
};

typedef struct {
    char address[RELAY_SIZE];
} Relay;

struct RelayNode {
    float similarity;
    char address[RELAY_SIZE];
    struct RelayNode *next;
};

typedef struct {
    uint32_t size;
    uint32_t count;
    User *users;
} LinkedUsers;

typedef struct {
    uint32_t size;
    uint32_t count;
    Relay *relays;
} LinkedRelays;

typedef struct {
    LinkedUsers *users;
    LinkedRelays *relays;
    struct TrieNode *tree;
} Database;

typedef struct {
    char searchTerm[100];
    int limit;
} RelaySearch;

#endif



