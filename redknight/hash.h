#ifndef __MINIHASH_H__
#define __MINIHASH_H__


#include "types.h"


typedef struct CPAC_HASHINDEX {
     Uint8 *data;
     Uint32 sum;
     void *info;

     struct CPAC_HASHINDEX *next, *prev;
} hashindex;

typedef struct CPAC_HASH {
     hashindex *list[27];
} hash, *hashp;

//typedef hashp (hash *);

/*
 * Function: djb2
 * Generate a DJB2 hash from a string.
 *
 * Parameters:
 * data - A 0-terminated string to hash.
 *
 * Returns:
 * The hash on success.
 * -1 on failure.
 */
int djb2 (Uint8 *data);

/*
 * Function: hash_init
 * Create a new indexed hash.
 *
 * Returns:
 * The new hash.
 */
hashp hash_init ();

/*
 * Function: hash_add
 * Add a new hash element to an existing hash.
 * (There is no safety net for passing a bad hash).
 *
 * Parameters:
 * hash - An initialized hash.
 * data - A name/array of bytes (< 16 elements).
 * info - A pointer to data to associate with this hash.
 *
 * Returns:
 * -1 on invalid input.
 * 1 on success.
 */
int hash_add (hashp hash, Uint8 *data, void *info);

/*
 * Function: hash_grok
 * Get the associated information for a value.
 *
 * Parameters:
 * hash - An initialized hash.
 * query - A name/array of bytes (<16 elements).
 *
 * Returns:
 * NULL on invalid input or failure.
 * A pointer to your data on success.
 */
void *hash_grok(hashp hash, Uint8 *query);

/*
 * Function: hash_test
 * Test to see if a given value exists.
 *
 * Parameters:
 * hash - An initialized hash.
 * query - A name/array of bytes (<16 elements).
 *
 * Returns:
 * -1 on invalid input.
 * 0 on failure.
 * 1 on success.
 */
Sint8 hash_test(hashp hash, Uint8 *query);

/*
 * Function: hash_del
 * Remove a hash element from an existing hash.
 * (There is no safety net for passing a bad hash).
 * You will have to delete associated info yourself.
 *
 * Parameters:
 * hash - An initialized hash.
 * query - A name/array of bytes (< 16 elements).
 *
 * Returns:
 * NULL on invalid input or failure.
 * A pointer to your info on success.
 */
void *hash_del(hashp hash, Uint8 *query);

/*
 * Function: hash_free
 * Remove an existing hash, and clean it up.
 * It uses the supplied function to free items.
 *
 * Parameters:
 * hash - An initialized hash.
 * freef - A function to apply to each void *, to clean it up. You can also pass NULL if you don't want it cleaned up.
 */
void hash_free(hashp hash, void (*freef)( ));


#endif // __MINIHASH_H__
