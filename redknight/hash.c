/*
 * Copyright (c) 2006, Brendan MacDonell.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 *  - Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *  - Redistributions in binary form must reproduce
 *    the above copyright notice, this list of
 *    conditions and the following disclaimer in the
 *    documentation and/or other materials provided
 *    with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "hash.h"

/*          Private Functions          */

int __mindex(Uint8 head)
{
     return ((head >= 'A' && head <= 'z' && head != '_') ?
             (tolower(head) - 'a') : 26);
}


/*          Public Functions           */
int djb2 (Uint8 *str)
{
     Uint32 sum = 5381;

     for (; *str; str++) sum = (sum << 5) + sum + *str;

     return sum;
}

hashp hash_init ()
{
     hash *hashptr;
     int i;

     hashptr = (hash *) malloc(sizeof(hash));

     for(i = 0; i < 27; i++) {
          hashptr->list[i] = NULL;
     }

     return hashptr;
}

int hash_add (hashp h, Uint8 *data, void *info)
{
     hashindex *n;
     Uint32 sum;
     int pos;

     if ((sum = djb2(data)) == -1)
     {
          // Too Long. Invalid information.
          return -1;
     }
     pos = __mindex(data[0]);
     
     n = (hashindex *)malloc(sizeof(hashindex));
     n->sum = sum;
     n->data = (char *)malloc(strlen(data)+1);
     n->prev = NULL;	// Insert onto the front.
     strcpy(n->data, data);
     n->info = info;

     // Inserting onto the front is a threefold optimization.
     // * It takes fewer operations (don't have to seek the end).
     // * It's a fair assumption that a new entry will soon be used.
     // * It's fair to assume that new additions will be temporaries.

     n->next = h->list[pos];
     if(n->next != NULL)
     {
          n->next->prev = n;
     }
     h->list[pos] = n;

     return 1;
}

void *hash_grok(hashp h, Uint8 *query)
{
     hashindex *o;
     Uint32 sum;

     if ((sum = djb2(query)) == -1)
     {
          // Too long, invalid.
          return NULL;
     }
 
     o = h->list[__mindex(query[0])];
     if (o == NULL)
     {
          // It can't be in an empty list.
          return NULL;
     }
     
     // Find it.
     for ( ; o != NULL; o = o->next) {
          if (o->sum == sum)
          {
               // Last test; djb2 isn't perfect.
               if(!strcmp(query, o->data))
               {
                    return o->info;
               }
          }
     }

     return NULL;
}

Sint8 hash_test(hashp h, Uint8 *query)
{
     hashindex *o;
     Uint32 sum;

     if ((sum = djb2(query)) == -1)
     {
          // Too long, invalid.
          return -1;
     }
 
     o = h->list[__mindex(query[0])];
     if (o == NULL)
     {
          // It can't be in an empty list.
          return -1;
     }
     
     // Find it.
     for ( ; o != NULL; o = o->next) {
          if (o->sum == sum)
          {
               // Last test; djb2 isn't perfect.
               if(!strcmp(query, o->data))
               {
                    return 1;
               }
          }
     }

     return 0;
}

void *hash_del(hashp h, Uint8 *query)
{
     hashindex *o;
     void *i;
     Uint32 sum;
     int pos;

     if ((sum = djb2(query)) == -1)
     {
          // Too long, invalid.
          return NULL;
     }
     pos = __mindex(query[0]);

     o = h->list[pos];
     if (o == NULL)
     {
          // It can't be in an empty list.
          return NULL;
     }
     
     // Find it.
     for ( ; o != NULL; o = o->next) {
          if (o->sum == sum)
          {
               // Last test; djb2 isn't perfect.
               printf("Possible match for deletion!\n");
               if (!strcmp(query, o->data))
               {
                    i = o->info;
                    free(o->data);
                    // Patch up the list
                    if (o->prev == NULL && o->next == NULL)
                    {
                         h->list[pos] = NULL;
                    } 
                    else if (o->prev == NULL && o->next != NULL)
                    {
                         h->list[pos] = o->next;
                         h->list[pos]->prev = NULL;
                    }
                    else if (o->prev != NULL && o->next == NULL)
                    {
                         o->prev->next = NULL;
                    }
                    else if (o->prev != NULL && o->next != NULL)
                    {
                         o->prev->next = o->next;
                         o->next->prev = o->prev;
                    }
                    free(o);
                    return i;
               }
          }
     }

     return NULL;
}

void hash_free(hashp h, void (*freef)( ))
{
     hashindex *o, *f;
     int i;

     for (i = 0; i < 27; i++) {
          o = h->list[i];
          while(o != NULL) {
               f = o;
               o = o->next;
               free(f->data);

               if(freef != NULL)
                    freef(f->info);

               free(f);
          }
          h->list[i] = NULL;
     }
}

