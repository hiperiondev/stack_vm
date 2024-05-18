/*
 * @hashtable.h
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

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdint.h>
#include <stdbool.h>

#include "linked_list.h"

/**
 * @struct hashtable_s
 * @brief Chained hash table
 *
 */
typedef struct hashtable_s {
       list_t *ptrList;
       node_t* *ptrTable;
    uint32_t iSize;
    uint32_t (*fDataLen)(void*);
} hashtable_t;

/**
 * @fn hashtable_t* InitHashTable(void* (*fCreateData)(void*), bool (*fDeleteData)(void*), uint32_t (*fDuplicatedNode)(node_t*, node_t*), uint32_t (*fNodeDataCmp)(void*, void*), uint32_t iTableSize, uint32_t (*fDataLen)(void*))
 * @brief
 *
 * @param fCreateData Creates data
 * @param fDeleteData Delete data
 * @param fDuplicatedNode Duplicate data
 * @param fNodeDataCmp Compare data
 * @param iTableSize Table size
 * @param fDataLen Data size
 * @return Table pointer
 */
hashtable_t* InitHashTable(void* (*fCreateData)(void*), bool (*fDeleteData)(void*), uint32_t (*fDuplicatedNode)(node_t*, node_t*), uint32_t (*fNodeDataCmp)(void*, void*), uint32_t iTableSize, uint32_t (*fDataLen)(void*));

/**
 * @fn bool AddDataToTable(hashtable_t*, void*)
 * @brief
 *
 * @param pHT
 * @param pData
 * @return
 */
bool AddDataToTable(hashtable_t *pHT, void *pData);

/**
 * @fn node_t FindDataInTable*(hashtable_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
node_t* FindDataInTable(hashtable_t*, void*);

/**
 * @fn bool DelDataInTable(hashtable_t*, void*)
 * @brief
 *
 * @param pHT
 * @param pData
 * @return
 */
bool DelDataInTable(hashtable_t *pHT, void *pData);

/**
 * @fn bool EndHashTable(hashtable_t*)
 * @brief
 *
 * @param pHT
 * @return
 */
bool EndHashTable(hashtable_t *pHT);

#endif  /* __HASHTABLE_H__ */
