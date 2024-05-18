/*
 * @llgen.c
 *
 * @brief Stack VM
 * @details
 * This is based on other projects:
 *   https://github.com/r-bauer/hashTable
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

#define LLHead          (L->slkHead)
#define LLTail          (L->slkTail)
#define NodeCount       (L->uiCount)

#define CreateData      (*(L->fCreateData))
#define DeleteData      (*(L->fDeleteData))
#define DuplicatedNode  (*(L->fDuplicatedNode))
#define NodeDataCmp     (*(L->fNodeDataCmp))

bool AddNodeAtHead(list_t *L, void *nd) {
    node_t *slkPn;

    slkPn = CreateNode(L, nd);
    if (slkPn == NULL)
        return false;

    if (LLHead == NULL) {
        LLHead = LLTail = slkPn;
    } else {
        LLHead->prior = slkPn;
        slkPn->next = LLHead;
        LLHead = slkPn;
    }

    ++NodeCount;
    return true;
}

bool AddNodeAscend(list_t *L, void *nd) {
    node_t *slkPn;
    node_t *slkPrev;
    node_t *slkCurr;
    node_t snDummy;
    int iCompare = 0;
    int iAction = 0;

    slkPn = CreateNode(L, nd);
    if (slkPn == NULL)
        return false;

    snDummy.next = LLHead;
    snDummy.prior = NULL;
    if (snDummy.next != NULL)
        snDummy.next->prior = &snDummy;

    slkPrev = &snDummy;
    slkCurr = snDummy.next;

    for (; slkCurr != NULL; slkPrev = slkCurr, slkCurr = slkCurr->next) {
        iCompare = NodeDataCmp(slkPn->pdata, slkCurr->pdata);
        if (iCompare <= 0) {
            break;
        }
    }

    if ((slkCurr != NULL) && (iCompare == 0)) {
        iAction = DuplicatedNode(slkPn, slkCurr);
        if (iAction == 2) {
        } else {
            LLHead = snDummy.next;
            LLHead->prior = NULL;

            if (iAction == 1) {
                DeleteData(slkPn->pdata);
                free(slkPn);
            }

            return true;
        }
    }

    slkPrev->next = slkPn;
    slkPn->prior = slkPrev;
    slkPn->next = slkCurr;

    if (slkCurr != NULL) {
        slkCurr->prior = slkPn;
    } else {
        LLTail = slkPn;
    }

    ++NodeCount;

    LLHead = snDummy.next;
    LLHead->prior = NULL;

    return true;
}

list_t* CreateLList(void* (*fCreateData)(void*), bool (*fDeleteData)(void*), uint32_t (*fDuplicatedNode)(node_t*, node_t*),
        uint32_t (*fNodeDataCmp)(void*, void*)) {
    list_t *pL;

    pL = (list_t*) malloc(sizeof(list_t));
    if (pL == NULL)
        return NULL;

    pL->slkHead = NULL;
    pL->slkTail = NULL;
    pL->uiCount = 0;
    
    pL->fCreateData = fCreateData;
    pL->fDeleteData = fDeleteData;
    pL->fDuplicatedNode = fDuplicatedNode;
    pL->fNodeDataCmp = fNodeDataCmp;

    return (pL);
}

node_t* CreateNode(list_t *L, void *data) {
    node_t *slkNewNode;

    slkNewNode = (node_t*) malloc(sizeof(node_t));
    if (slkNewNode == NULL)
        return (NULL);

    slkNewNode->prior = NULL;
    slkNewNode->next = NULL;
    slkNewNode->pdata = CreateData(data);

    if (slkNewNode->pdata == NULL) {
        free(slkNewNode);
        return (NULL);
    }

    else {
        return (slkNewNode);
    }
}

bool DeleteNode(list_t *L, node_t *slkToDelete) {
    node_t *slkPn;

    if (slkToDelete == NULL)
        return false;

    if (slkToDelete == LLHead && slkToDelete == LLTail) {
        LLHead = NULL;
        LLTail = NULL;
    } else if (slkToDelete->prior == NULL) {
        LLHead = slkToDelete->next;
        LLHead->prior = NULL;
    } else if (slkToDelete->next == NULL) {
        LLTail = slkToDelete->prior;
        LLTail->next = NULL;
    } else {
        slkPn = slkToDelete->prior;
        slkPn->next = slkToDelete->next;

        slkPn = slkToDelete->next;
        slkPn->prior = slkToDelete->prior;
    }

    DeleteData(slkToDelete->pdata);
    free(slkToDelete);

    --NodeCount;

    return true;
}

node_t* FindNode(list_t *L, void *nd) {
    node_t *slkPCurr;

    if (LLHead == NULL) {
        return (NULL);
    }

    for (slkPCurr = LLHead; slkPCurr != NULL; slkPCurr = slkPCurr->next) {
        if (NodeDataCmp(nd, slkPCurr->pdata) == 0) {
            return (slkPCurr);
        }
    }

    return (NULL);
}

node_t* FindNodeAscend(list_t *L, void *nd) {
    node_t *slkPCurr;
    int iCmpResult;

    if (LLHead == NULL) {
        return (NULL);
    }

    for (slkPCurr = LLHead; slkPCurr != NULL; slkPCurr = slkPCurr->next) {
        iCmpResult = NodeDataCmp(nd, slkPCurr->pdata);

        if (iCmpResult < 0) {
            return (NULL);
        }

        if (iCmpResult == 0) {
            return (slkPCurr);
        }
    }

    return (NULL);
}

bool DestroyLList(list_t *L) {
    node_t *slk1, *slk2;

    if (L == NULL)
        return false;

    for (slk1 = LLHead; slk1 != NULL;) {
        slk2 = slk1;
        slk1 = slk1->next;
        DeleteNode(L, slk2);
    }
    
    free(L);

    return true;
}
