/*
 * @llgen.h
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

#ifndef __LLGEN_H__
#define __LLGEN_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @struct node_s
 * @brief
 *
 */
typedef struct node_s {
    struct node_s *prior;
    struct node_s *next;
             void *pdata;
} node_t;

/**
 * @struct list_s
 * @brief
 *
 */
typedef struct list_s {
      node_t *slkHead;
      node_t *slkTail;
    uint32_t uiCount;

       void* (*fCreateData)(void*);
        bool (*fDeleteData)(void*);
    uint32_t (*fDuplicatedNode)(node_t*, node_t*);
    uint32_t (*fNodeDataCmp)(void*, void*);
} list_t;

/**
 * @fn bool AddNodeAscend(list_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
bool AddNodeAscend(list_t*, void*);

/**
 * @fn bool AddNodeAtHead(list_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
bool AddNodeAtHead(list_t*, void*);

/**
 * @fn list_t CreateLList*(void*(*)(void*), bool(*)(void*), uint32_t(*)(node_t*, node_t*), uint32_t(*)(void*, void*))
 * @brief
 *
 * @param fCreateData
 * @param fDeleteData
 * @param fDuplicatedNode
 * @param fNodeDataCmp
 * @return
 */
list_t* CreateLList(void* (*fCreateData)(void*), bool (*fDeleteData)(void*), uint32_t (*fDuplicatedNode)(node_t*, node_t*),
        uint32_t (*fNodeDataCmp)(void*, void*));

/**
 * @fn node_t CreateNode*(list_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
node_t* CreateNode(list_t*, void*);

/**
 * @fn bool DeleteNode(list_t*, node_t*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
bool DeleteNode(list_t*, node_t*);

/**
 * @fn node_t FindNode*(list_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
node_t* FindNode(list_t*, void*);

/**
 * @fn node_t FindNodeAscend*(list_t*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
node_t* FindNodeAscend(list_t*, void*);

/**
 * @fn bool DestroyLList(list_t*)
 * @brief
 *
 * @param
 * @return
 */
bool DestroyLList(list_t*);

#endif  /* __LLGEN_H__ */
