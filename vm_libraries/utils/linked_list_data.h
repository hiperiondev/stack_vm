/*
 * @llapp.h
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

#ifndef __LLAPP_H__
#define __LLAPP_H__

#include <stdint.h>
#include <stdbool.h>

#include "linked_list.h"

/**
 * @struct nodedata_s
 * @brief
 *
 */
typedef struct nodedata_s {
        char *pWord;
    uint32_t uCont;
} nodedata_t;

/**
 * @fn void CreateData*(void*)
 * @brief
 *
 * @param
 */
void* CreateData(void*);

/**
 * @fn bool DeleteData(void*)
 * @brief
 *
 * @param
 * @return
 */
bool DeleteData(void*);

/**
 * @fn uint32_t DuplicatedNode(node_t*, node_t*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
uint32_t DuplicatedNode(node_t*, node_t*);

/**
 * @fn uint32_t NodeDataCmp(void*, void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
uint32_t NodeDataCmp(void*, void*);

/**
 * @fn uint32_t DataLen(void*)
 * @brief
 *
 * @param
 * @return
 */
uint32_t DataLen(void*);

#endif  /* __LLAPP_H__ */
