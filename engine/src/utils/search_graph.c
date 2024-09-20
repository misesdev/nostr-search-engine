#ifndef SEARCH_GRAPH_C 
#define SEARCH_GRAPH_C

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "./uthash.h"
#include "./utils.c"
#include "../types/types.c"
#include "../types/user_list.c"

typedef struct {
    User *user;
    UT_hash_handle hh; // Handle para a tabela hash da biblioteca uthash
} VisitedUser;

void markVisited(VisitedUser **visitedSet, User *user) {
    VisitedUser *newVisit = malloc(sizeof(VisitedUser));
    newVisit->user = user;
    HASH_ADD_PTR(*visitedSet, user, newVisit);
}

bool isVisited(VisitedUser *visitedSet, User *user) {
    VisitedUser *visit;
    HASH_FIND_PTR(visitedSet, &user, visit);
    return visit != NULL;
}

void freeVisitedSet(VisitedUser *visitedSet) {
    VisitedUser *current, *tmp;
    HASH_ITER(hh, visitedSet, current, tmp) {
        HASH_DEL(visitedSet, current);
        free(current);
    }
}

void enqueue(struct UserNode ***queue, int *queueSize, int *capacity, User *user) {
    if (*queueSize == *capacity) {
        *capacity += 100; // Aumenta a capacidade em blocos de 100
        *queue = realloc(*queue, sizeof(struct UserNode*) * (*capacity));
    }
    (*queue)[*queueSize] = (struct UserNode*) malloc(sizeof(struct UserNode));
    (*queue)[*queueSize]->user = user;
    (*queue)[*queueSize]->next = NULL;
    (*queueSize)++;
}

struct UserNode* searchOnGraph(User *rootUser, char *searchTerm, int limit) {
    int foundCount = 0, visitedCount = 0;
    struct UserNode *resultList = createUserNode(NULL);
    limit = limit > MAX_LIMIT_RESULTS ? MAX_LIMIT_RESULTS : limit;

    // Fila de busca (implementada como um array dinâmico de UserNode*)
    struct UserNode **queue = NULL;
    int queueSize = 0, queueCapacity = 100;
    queue = malloc(queueCapacity * sizeof(struct UserNode*));

    // Hash set para verificar usuários visitados
    VisitedUser *visitedSet = NULL;

    // Adiciona os amigos do usuário root à fila de busca
    struct UserNode *current = rootUser->friends;
    while (current != NULL) {
        enqueue(&queue, &queueSize, &queueCapacity, current->user);
        markVisited(&visitedSet, current->user); // Marca como visitado
        current = current->next;
    }

    // Inicia a busca em largura (BFS)
    while (queueSize > 0 && foundCount < limit && visitedCount < MAX_USERS_TO_VISIT) {
        // Remove o primeiro elemento da fila
        struct UserNode *currentNode = queue[0];
        for (int i = 0; i < queueSize - 1; i++) {
            queue[i] = queue[i + 1];
        }
        queueSize--;

        visitedCount++;

        // Verifica se o usuário é semelhante ao termo de busca
        if (textSimilarity(currentNode->user->name, searchTerm) >= MIN_SIMILARITY_TERM) {
            insertUserNode(resultList, currentNode->user);
            foundCount++;
        }

        // Adiciona os amigos do nó atual à fila
        struct UserNode *friendList = currentNode->user->friends;
        while (friendList != NULL && visitedCount < MAX_USERS_TO_VISIT) {
            if (!isVisited(visitedSet, friendList->user)) {
                enqueue(&queue, &queueSize, &queueCapacity, friendList->user);
                markVisited(&visitedSet, friendList->user); // Marca como visitado
            }
            friendList = friendList->next;
        }

        // Libera a memória do nó atual da fila
        free(currentNode);

        // Se atingir o limite de resultados ou usuários visitados, a busca termina
        if (foundCount >= limit || visitedCount >= MAX_USERS_TO_VISIT) {
            break;
        }
    }

    // Libera a memória da fila
    for (int i = 0; i < queueSize; i++) {
        free(queue[i]);
    }
    free(queue);

    // Libera o conjunto de usuários visitados
    freeVisitedSet(visitedSet);

    return resultList;
}


#endif