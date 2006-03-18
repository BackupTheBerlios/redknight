#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "elbot.h"
#include "client_serv.h"
#include "misc.h"
#include "config.h"
#include "actors.h"

ACTOR *actor_list[MAX_ACTORS];
int max_actor_id = 0;
int me = 0;

void add_actor(int id, Uint8 *name, short x, short y)
{
	Uint8 *ptr = NULL;
	int i;
	
	if (id > MAX_ACTORS) {	
		return;
	}
	if (actor_list[id] != NULL) {
		destroy_actor(id);
	}
	actor_list[id] = (ACTOR *)safe_malloc(sizeof(ACTOR));
	actor_list[id]->x = x;
	actor_list[id]->y = y;
	
	// 25 doesn't really mean anything;  it's just a safety net, in case something goes wrong.
	for (i = 0; i < 25 && name[i] != '\0'; i++) {
		// Color is no longer a given; but a color character is.
		if (name[i] >= 127)
		{
			ptr = name+i;
			break;
		}
	}
	
	if (ptr != NULL) {
		// This player is in a guild
		*--ptr = 0;
		actor_list[id]->name = (char *)safe_malloc(strlen(name)+1);
		actor_list[id]->guild = (char *)safe_malloc(strlen(ptr+2)+1);
		strcpy(actor_list[id]->name, name);
		strcpy(actor_list[id]->guild, ptr+2);
	} else {
		actor_list[id]->name = (char *)safe_malloc(strlen(name)+1);
		actor_list[id]->guild = NULL;
		strcpy(actor_list[id]->name, name);
	}
	
	if (id > max_actor_id) {
		max_actor_id = id;
	}
}

void destroy_actor(int id)
{
	if (id > max_actor_id || !actor_list[id] || id == me) {
		return;
	}
	safe_free(actor_list[id]->name);
	safe_free(actor_list[id]->guild);
	safe_free(actor_list[id]);
	actor_list[id] = NULL;
}

void destroy_all_actors()
{
	int i;
	
	for (i = 0; i <= max_actor_id; i++) {
		destroy_actor(i);
	}
}

int get_actor_by_name(const char *name)
{
	int i;
	
	for (i = 0; i <= max_actor_id; i++) {
		if (!actor_list[i]) {
			continue;
		}

		if (!strcasecmp(name, actor_list[i]->name)) {
			return i;
		}
	}
	return -1;
}

void handle_actor_command(int id, int command)
{
	if (id > max_actor_id || !actor_list[id]) {
		return;
	}
	
	switch (command) {
	case move_n:
	case run_n:
		actor_list[id]->y++;
		break;
	case move_ne:
	case run_ne:
		actor_list[id]->x++;
		actor_list[id]->y++;
		break;
	case move_e:
	case run_e:
		actor_list[id]->x++;
		break;
	case move_se:
	case run_se:
		actor_list[id]->x++;
		actor_list[id]->y--;
		break;
	case move_s:
	case run_s:
		actor_list[id]->y--;
		break;
	case move_sw:
	case run_sw:
		actor_list[id]->x--;
		actor_list[id]->y--;
		break;
	case move_w:
	case run_w:
		actor_list[id]->x--;
		break;
	case move_nw:
	case run_nw:
		actor_list[id]->x--;
		actor_list[id]->y++;
		break;
	}
}

int is_guild_member(char *name)
{
	int i;
	
	for (i = 0; i < max_actor_id; i++) {
		if (actor_list[i] && !strcasecmp(actor_list[i]->name, name)
		&& actor_list[i]->guild
		&& !strcasecmp(actor_list[i]->guild, cfg.guild)) {
			return 1;
		}
	}
	
	return 0;
}
