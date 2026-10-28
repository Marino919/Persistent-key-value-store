#ifndef KVSTORE_REMOVE_H
#define KVSTORE_REMOVE_H

#include "helpers.h"

int kv_delete( KVStore *kvs, const char *key);
int table_remove(KVStore *kvs, const char *key);

#endif //KVSTORE_REMOVE_H