#include "dcacheServerService.h"
#include "../storage/datastorage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ObjectNode* getObject(char *clientId, char key[17], ClientEntry* clientArray) {
    int index = hashClientId(clientId, BUCKETSNUM);

    if (strcmp(clientArray[index].clientId, clientId) == 0) {
        for (ObjectNode* node = clientArray[index].objectList; node != NULL; node = node->next) {
            if (strcmp(node->key, key) == 0) {
                return node;
            }
        }
        return NULL;
    } else {
        for (int i = index + 1; i < BUCKETSNUM; i++) {
            if (strcmp(clientArray[i].clientId, clientId) == 0) {
                for (ObjectNode* node = clientArray[i].objectList; node != NULL; node = node->next) {
                    if (strcmp(node->key, key) == 0) {
                        return node;
                    }
                }
                return NULL;
            }
        }
    }

    return NULL;
}