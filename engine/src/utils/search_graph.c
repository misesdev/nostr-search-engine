#ifndef SEARCH_GRAPH_C 
#define SEARCH_GRAPH_C

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "./string_utils.c"
#include "../types/types.c"
#include "../types/user_queue.c"
#include "../types/user_bitset.c"
#include "../utils/search_utils.c"

struct ResultNode* searchOnGraph(User *rootUser, char *searchTerm, int limit) 
{
    int foundCount = 0, visitedCount = 0;

    struct ResultNode *resultList = createResultNode(NULL, 0);

    if(!rootUser->friends) return resultList;

    // Fila de busca (implementada como um array dinâmico de User*)
    UserQueue *userQueue = createUserQueue();

    // Bit set para verificar usuários visitados
    uint_fast64_t *userBitset = createUserBitset();

    // Adiciona os amigos do usuário root à fila de busca
    struct UserNode *current = rootUser->friends;
    while (current) 
    {
        if(current->user) 
        {
            markVisitedUser(userBitset, current->user); 
            enQueueUser(userQueue, current->user);
        }

        current = current->next;
    }

    // Inicia a busca em largura (BFS)
    while (foundCount < limit || visitedCount < MAX_USERS_TO_VISIT) 
    {
        if(visitedCount >= userQueue->size) break;

        // take the element from queue
        User *currentUser = userQueue->queue[visitedCount];
        visitedCount++;

        if(!currentUser) continue;

        // Verifica se o usuário é semelhante ao termo de busca
        float similarity = textSimilarity(currentUser->displayName, searchTerm);
        if (similarity >= MIN_SIMILARITY_TERM) 
        {
            insertResultNode(resultList, currentUser, similarity);
            foundCount++;
        }

        // if the current user not have friends, go to next users of queue
        if(!currentUser->friends) continue;

        // Add friends of current node on queue
        struct UserNode *friendList = currentUser->friends;
        while (friendList != NULL && visitedCount < MAX_USERS_TO_VISIT) 
        {
            if (!isVisitedUser(userBitset, friendList->user)) 
            {
                markVisitedUser(userBitset, friendList->user); 
                enQueueUser(userQueue, friendList->user);
            }
            friendList = friendList->next;
        }

        if (foundCount >= limit || visitedCount >= MAX_USERS_TO_VISIT) {
            break;
        }
    }

    freeUserBitset(userBitset);

    freeQueueUser(userQueue);
  
    return resultList;
}


#endif
