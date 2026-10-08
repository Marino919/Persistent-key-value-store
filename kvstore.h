//
// Created by Marino Ante Knezovic on 05.10.2026..
//

#ifndef PERSISTENT_KEY_VALUE_STORE_KVSTORE_H
#define PERSISTENT_KEY_VALUE_STORE_KVSTORE_H

#endif //PERSISTENT_KEY_VALUE_STORE_KVSTORE_H

#include <stddef.h>

typedef struct KVStore KVStore;

KVStore *kv_open( const char *path );
void kv_close( KVStore *kvs);
int kv_put( KVStore *kvs, const char *key, const char *value);
const char *kv_get(KVStore *kvs, const char *key);
int kv_delete( KVStore *kvs, const char *key);
size_t kv_count(const KVStore *kvs);
void kv_print_store(KVStore *kvs);

