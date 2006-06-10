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
 
/*
** Functions for extending the 'hash-lists' functionality in RedKnight.
**/
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "hash.h"
#include "log.h"
#include "connection.h"

void hashl_save(hashp h, char *dest)
{
     FILE *out = fopen(dest, "w");
     hashindex *o;
     int q;

     for(q = 0; q < 27; q++) {
          o = h->list[q];
          
          for(; o != NULL; o = o->next)
               fprintf(out, "%s\n", o->data);
     }
     
     fclose(out);
}

void hashl_load(hashp h, char *src)
{
     FILE *in = fopen(src, "r");
     int i;
     char line[512];   
     
     while(fgets(line, 512, in)) {
          for(i = 0; line[i] != '\0' && line[i] != '\r' && line[i] != '\n'; i++)
               line[i] = tolower(line[i]);  // Empty loop.
               
          if(i == 0) continue;     // Empty line.
          line[i] = '\0';
          
          if(hash_add(h, line, NULL) == -1)
               log_error("Error: Could not load name %s.\n", line);
     }
     
     fclose(in);
}

void hashl_dump(hashp h, char *name)
{
     hashindex *o;
     int q;

     for(q = 0; q < 27; q++) {
          o = h->list[q];
          
          for(; o != NULL; o = o->next)
               send_pm("%s %s", name, o->data);
     }
}

void hashl_add(hashp h, char *name, char *admin)
{
     char *temp = name;
     
     for(; *temp != '\0'; temp++)
          *temp = tolower(*temp);
          
     if(hash_add(h, name, (void *)(1)) == -1)
     {
          log_error("Error: Could not add name %s.\n", name);
          send_pm("%s For some reason, %s couldn't be added.", admin, name);
     }
}

int hashl_chk(hashp h, char *name)
{
     char *temp = name;

     for(; *temp != '\0'; temp++)
          *temp = tolower(*temp);

     return hash_test(h, name);
}

void hashl_del(hashp h, char *name, char *admin)
{
     char *temp = name;

     for(; *temp != '\0'; temp++)
          *temp = tolower(*temp);

     if(hash_del(h, name) == NULL)
     {
          log_error("Error: Could not delete name %s.\n", name);
          send_pm("%s %s wasn't be removed. (Maybe they don't exist?)", admin, name);
     }
}
