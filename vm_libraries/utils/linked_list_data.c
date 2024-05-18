/*
 * @llapp.c
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
#include <string.h>
#include <stdlib.h>

#include "linked_list_data.h"
#include "linked_list.h"

void* CreateData(void *data) {
    nodedata_t *pNewData;

    pNewData = (nodedata_t*) malloc(sizeof(nodedata_t));
    if (pNewData == NULL)
        return (NULL);

    pNewData->uCont = 1;
    pNewData->pWord = strdup((char*) data);

    if (pNewData->pWord == NULL) {
        free(pNewData);
        return (NULL);
    } else {
        return (pNewData);
    }
}

bool DeleteData(void *data) {
    free(((nodedata_t*) data)->pWord);
    free(data);

    return true;
}

uint32_t DuplicatedNode(node_t *slkNewNode, node_t *slkListNode) {
    nodedata_t* pnd;

    pnd = slkListNode->pdata;
    pnd->uCont += 1;

    return 1;
}

uint32_t NodeDataCmp(void *first, void *second) {
    return (strcmp(((nodedata_t*) first)->pWord, ((nodedata_t*) second)->pWord));
}

uint32_t DataLen(void *data) {
    return ((int) strlen(data));
}
