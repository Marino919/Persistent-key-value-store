#ifndef PERSISTENT_KEY_VALUE_STORE_LOG_COMMAND_H
#define PERSISTENT_KEY_VALUE_STORE_LOG_COMMAND_H
#include "helpers.h"

int log_command(KVStore *kvs, uint8_t operation, const char *key, const char *value);
#endif //PERSISTENT_KEY_VALUE_STORE_LOG_COMMAND_H