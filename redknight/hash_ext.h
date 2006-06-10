#ifndef __HASH_EXT_H__
#define __HASH_EXT_H__

#include "hash.h"

// Don't pass the following const char *'s; they will be tolower'd.
void hashl_add(hashp h, char *name);
void hashl_del(hashp h, char *name);

void hashl_dump(hashp h, char *name);
void hashl_load(hashp h, char *src);
void hashl_save(hashp h, char *dest);

#endif
