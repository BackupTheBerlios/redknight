#ifndef ACTORS_H
#define ACTORS_H

//The details out bot will want to know about the actor
typedef struct
{
	int actor_id;
	int actor_type;
	int fighting;

    char actor_name[30];
    char guild[6];

	int x_tile_pos;
	int y_tile_pos;
	
	void *k;  // Reference the accompanying kill queue node
	          // convert to kill_queue to use.
}actor;

//Kill Queue
struct kill_queue
{
    unsigned int time;   
    actor *k;       // Reference accompanying actor - actors_list[k]
    struct kill_queue *next, *prev;
};
extern struct kill_queue *first_node;

extern actor *actors_list[1000];
extern int yourself, insult_enemy;
extern int max_actors;

#endif
