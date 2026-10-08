#ifndef KVSTORE_DOOR_H
#define KVSTORE_DOOR_H
#include "helpers.h"

KVStore *kv_open( const char *path );
void kv_close( KVStore *kvs);

#endif //KVSTORE_DOOR_H