#ifndef __ACTORS_H__
#define __ACTORS_H__

#include "types.h"

#define MAX_ACTORS 1024

typedef struct _ACTOR {
	Uint8 *name;
	Uint8 *guild;
	short x;
	short y;
} ACTOR;

extern ACTOR *actor_list[MAX_ACTORS];
extern int max_actor_id;
extern int me;

void add_actor(int id, char *name, short x, short y);
void destroy_actor(int id);
void destroy_all_actors();
int get_actor_by_name(const char *name);
void handle_actor_command(int id, int command);
int is_guild_member(char *name);

#endif /* __ACTORS_H__ */
