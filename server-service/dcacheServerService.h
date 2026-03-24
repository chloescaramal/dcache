#ifndef DCACHE_DCACHESERVER_H
#define DCACHE_DCACHESERVER_H
#include "../storage/datastorage.h"

ObjectNode* getObject(char *clientId, char key[17], ClientEntry* clientArray);

#endif