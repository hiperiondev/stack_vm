/*
 * @hashtable.c
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
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"
#include "hashtable.h"

/////// HASHING  //////

static uint16_t HashDiv(uint32_t dwKey, int iSize) {
    return (uint16_t) (dwKey % iSize);
}

static uint16_t HashMul(uint32_t dwKey, int iSize) {
    double dFrac;

    dFrac = fmod(dwKey * 0.618, 1);

    return (uint16_t) (iSize * dFrac);
}

static uint32_t HashElf(const uint8_t *ptrData, int iLen) {
    uint32_t dwKey = 0, dwTmp;

    for (; iLen > 0; --iLen) {
        dwKey = (dwKey << 4) + *ptrData++;
        if ((dwTmp = dwKey & 0xF0000000))
            dwKey ^= dwTmp >> 24;
        dwKey &= ~dwTmp;
    }

    return dwKey;
}

uint16_t HashData(hashtable_t *pHT, void *pData) {
    uint32_t dwKey;
    uint16_t wIndex;
    uint16_t (*fHash)(uint32_t, int);

    dwKey = HashElf(pData, pHT->fDataLen(pData));
    fHash = (pHT->iSize & 1) ? HashDiv : HashMul;
    wIndex = fHash(dwKey, pHT->iSize);

    return wIndex;
}

static bool CreateTable(node_t* **ptrTable, int iSize) {
    *ptrTable = calloc(iSize, sizeof(node_t*));

    return (*ptrTable == NULL ? false : true);
}

////////////////////

hashtable_t* InitHashTable(void* (*fCreateData)(void*), bool (*fDeleteData)(void*), uint32_t (*fDuplicatedNode)(node_t*, node_t*),
        uint32_t (*fNodeDataCmp)(void*, void*), uint32_t iTableSize, uint32_t (*fDataLen)(void*)) {
    hashtable_t *pHT = NULL;

    pHT = (hashtable_t*) malloc(sizeof(hashtable_t));

    if (pHT) {
        pHT->iSize = iTableSize;
        pHT->fDataLen = fDataLen;

        if (CreateTable(&pHT->ptrTable, pHT->iSize)) {
            pHT->ptrList = CreateLList(fCreateData, fDeleteData, fDuplicatedNode, fNodeDataCmp);

            if (pHT->ptrList) {
                return (pHT);
            } else {
                free(pHT->ptrTable);
            }
        }
    }

    free(pHT);

    return NULL;
}

bool AddDataToTable(hashtable_t *pHT, void *pData) {
    uint16_t wIndex;

    if (pHT == NULL)
        return false;

    wIndex = HashData(pHT, pData);
    pHT->ptrList->slkHead = pHT->ptrTable[wIndex];

    if (AddNodeAscend(pHT->ptrList, pData) == false)
        return false;

    pHT->ptrTable[wIndex] = pHT->ptrList->slkHead;

    return true;
}

node_t* FindDataInTable(hashtable_t *pHT, void *pData) {
    uint16_t wIndex;
    void *pInfo;
    node_t* slk;

    if (pHT == NULL)
        return NULL;

    wIndex = HashData(pHT, pData);
    pHT->ptrList->slkHead = pHT->ptrTable[wIndex];
    pInfo = pHT->ptrList->fCreateData(pData);
    slk = FindNodeAscend(pHT->ptrList, pInfo);
    pHT->ptrList->fDeleteData(pInfo);

    return slk;
}

bool DelDataInTable(hashtable_t *pHT, void *pData) {
    uint16_t wIndex;
    void *pInfo;
    node_t* slk;
    bool bOk;

    wIndex = HashData(pHT, pData);
    pHT->ptrList->slkHead = pHT->ptrTable[wIndex];
    pInfo = pHT->ptrList->fCreateData(pData);
    slk = FindNodeAscend(pHT->ptrList, pInfo);
    pHT->ptrList->fDeleteData(pInfo);

    if (slk) {
        if (slk->prior == NULL)
            slk = pHT->ptrList->slkHead;

        if (slk->next == NULL)
            slk = pHT->ptrList->slkTail;

        bOk = DeleteNode(pHT->ptrList, slk);
        pHT->ptrTable[wIndex] = pHT->ptrList->slkHead;
    } else
        bOk = false;

    return bOk;
}

bool EndHashTable(hashtable_t *pHT) {
    int iCnt;
    node_t* pCurr, *pBak;

    if (pHT == NULL)
        return false;

    for (iCnt = 0; iCnt < pHT->iSize; ++iCnt) {
        pCurr = pHT->ptrList->slkHead = pHT->ptrTable[iCnt];

        if (pCurr != NULL) {
            while (pCurr != NULL) {
                pBak = pCurr;
                pCurr = pCurr->next;
                if (pCurr == NULL)
                    pHT->ptrList->slkTail = pBak;
                DeleteNode(pHT->ptrList, pBak);
            }
        }
    }

    free(pHT->ptrList);
    free(pHT);

    return true;
}
