#ifndef KVSTORE_RESIZE_H
#define KVSTORE_RESIZE_H
#include "helpers.h"

int needs_resize(const KVStore *kvs);
void table_resize(KVStore *kvs);

#endif //KVSTORE_RESIZE_H