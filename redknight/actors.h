#ifndef ACTORS_H
#define ACTORS_H

//Kill Queue
struct kill_queue
{
    unsigned int time;   
    int k;       // Reference accompanying actor - actors_list[k]
    struct kill_queue *next, *prev;
}extern *first_node;
    

//The details out bot will want to know about the actor
typedef struct
{
	int actor_id;
	int actor_type;
	int fighting;

    char actor_name[30];

	int x_tile_pos;
	int y_tile_pos;
	
	struct kill_queue *k;  // Reference the accompanying kill queue node
}actor;

extern actor *actors_list[1000];
extern int yourself, insult_enemy;
extern int max_actors;

#endif