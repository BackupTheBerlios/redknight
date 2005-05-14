/* Lifted From the Client   */
/* I'll rewrite it soon ... */

#include <stdlib.h>
#include "includes.h"
#include "actors.h"
#include "pathfinder.h"

PF_OPEN_LIST pf_open;
PF_TILE *pf_tile_map;
PF_TILE *pf_src_tile, *pf_dst_tile, *pf_cur_tile;
int pf_follow_path = 0;
int pf_actor_id;
unsigned int path_time = 0;

// Map stuff
// This is all we use, and need
typedef struct
{
	char file_sig[4];//should be "elmf", or else the map is invalid
	int tile_map_x_len;
	int tile_map_y_len;
	int tile_map_offset;
	int height_map_offset;
	int obj_3d_struct_len;
	int obj_3d_no;
	int obj_3d_offset;
	int obj_2d_struct_len;
	int obj_2d_no;
	int obj_2d_offset;
	int lights_struct_len;
	int lights_no;
	int lights_offset;
	char dungeon;//no sun
	char res_2;
	char res_3;
	char res_4;
	float ambient_r;
	float ambient_g;
	float ambient_b;
	int particles_struct_len;
	int particles_no;
	int particles_offset;
	int reserved_8;
	int reserved_9;
	int reserved_10;
	int reserved_11;
	int reserved_12;
	int reserved_13;
	int reserved_14;
	int reserved_15;
	int reserved_16;
	int reserved_17;
}map_header;

int tile_map_size_x;
int tile_map_size_y;
unsigned char *tile_map;
unsigned char *height_map;
char map_file_name[256];

unsigned short cur_map = -1;  // Don't know where we are until we know

//Load what we need...
void load_map(char * name)
{   
     map_header cur_map_header;
     char * mem_map_header=(char *)&cur_map_header;
     
     int i, x, y;
     
     FILE *f = NULL;
	 f=fopen(name, "rb");
	 if(!f)return;    //Should NEVER happen
	 
	 // Unload the pathfinder
	 pf_destroy_path();
     if(pf_tile_map)
		{
			free(pf_tile_map);
			pf_tile_map=NULL;
		}
	 
	 fread(mem_map_header, 1, sizeof(cur_map_header), f);
    	 
 	 tile_map_size_x=cur_map_header.tile_map_x_len;
	 tile_map_size_y=cur_map_header.tile_map_y_len;
	 
	 //allocate memory for the tile map
	 tile_map=(char *)malloc(tile_map_size_x*tile_map_size_y);
	 
 	 //read the tiles map
	 fread(tile_map, 1, tile_map_size_x*tile_map_size_y, f);
	 free(tile_map);
	 tile_map = NULL;   // Don't need it...

     //allocates the memory for the heights
	 height_map=(char *)malloc(tile_map_size_x*tile_map_size_y*6*6);

	 //read the heights map
	 fread(height_map, 1, tile_map_size_x*tile_map_size_y*6*6, f);

     fclose(f);

	 //create the tile map that will be used for pathfinding
	 pf_tile_map = (PF_TILE *)calloc(tile_map_size_x*tile_map_size_y*6*6, sizeof(PF_TILE));
    		
	for (x = 0; x < tile_map_size_x*6; x++) {
	    for (y = 0; y < tile_map_size_y*6; y++) {
	        i = y*tile_map_size_x*6+x;
	        pf_tile_map[i].x = x;
	        pf_tile_map[i].y = y;
			pf_tile_map[i].z = height_map[i];
	    }
	}
	
	examine_map(name+7);
    return;
}

#define PF_HEUR(a, b) ((PF_DIFF(a->x, b->x) + PF_DIFF(a->y, b->y)) * 10)

PF_TILE *pf_get_tile(int x, int y)
{
	if (x >= tile_map_size_x*6 || y >= tile_map_size_y*6 || x < 0 || y < 0) {
		return NULL;
	}
	return &pf_tile_map[y*tile_map_size_x*6+x];
}

PF_TILE *pf_get_next_open_tile()
{
	PF_TILE *tmp, *ret = NULL;
	int i, j, done = 0;
	
	if (pf_open.count == 0) {
		return NULL;
	}
	
	ret = pf_open.tiles[1];
	
	ret->state = PF_STATE_CLOSED;
	
	pf_open.tiles[1] = pf_open.tiles[pf_open.count--];
	
	j = 1;
	
	while (!done) {
		i = j;
		
		if (2*i+1 <= pf_open.count) {
			if (pf_open.tiles[i]->f >= pf_open.tiles[2*i]->f) {
				j = 2*i;
			}
			if (pf_open.tiles[j]->f >= pf_open.tiles[2*i+1]->f) {
				j = 2*i+1;
			}
		} else if (2*i <= pf_open.count) {
			if (pf_open.tiles[i]->f >= pf_open.tiles[2*i]->f) {
				j = 2*i;
			}
		}
		
		if (i != j) {
			tmp = pf_open.tiles[i];
			pf_open.tiles[i] = pf_open.tiles[j];
			pf_open.tiles[j] = tmp;
		} else {
			done = 1;
		}
	}

	return ret;
}

void pf_add_tile_to_open_list(PF_TILE *current, PF_TILE *neighbour)
{
	PF_TILE *tmp;
	
	if (!neighbour || neighbour->z == 0 || (current && PF_DIFF(current->z, neighbour->z) > 2)) {
		return;
	}
	
	if (current) {
		int f, g, h;
		int diagonal = (neighbour->x != current->x && neighbour->y != current->y);
		
		g = current->g + (diagonal ? 14 : 10);
		h = PF_HEUR(neighbour, pf_dst_tile);
		f = g + h;
		
		if (neighbour->state != PF_STATE_NONE && f >= neighbour->f) {
			return;
		}
		
		neighbour->f = f;
		neighbour->g = g;
		neighbour->parent = current;
	} else {
		neighbour->f = PF_HEUR(pf_src_tile, pf_dst_tile);
		neighbour->g = 0;
		neighbour->parent = NULL;
	}
		
	if (neighbour->state != PF_STATE_OPEN) {
		neighbour->open_pos = ++pf_open.count;
		pf_open.tiles[neighbour->open_pos] = neighbour;
	}
	
	while (neighbour->open_pos > 1) {
		if (pf_open.tiles[neighbour->open_pos]->f <= pf_open.tiles[neighbour->open_pos/2]->f) {
			tmp = pf_open.tiles[neighbour->open_pos/2];
			pf_open.tiles[neighbour->open_pos/2] = pf_open.tiles[neighbour->open_pos];
			pf_open.tiles[neighbour->open_pos] = tmp;
			neighbour->open_pos /= 2;
		} else {
			break;
		}
	}
	
	neighbour->state = PF_STATE_OPEN;
}

int pf_find_path(int x, int y)
{
	actor *me = pf_get_our_actor();
	int i;
	
	pf_destroy_path();
	
	if (me == NULL) {
		return -1;
	}
	
	pf_src_tile = pf_get_tile(me->x_tile_pos, me->y_tile_pos);
	pf_dst_tile = pf_get_tile(x, y);
	
	if (!pf_dst_tile || pf_dst_tile->z == 0) {
        log_error("Invalid Destination!\n");
		return 0;
	}
	
	for (i = 0; i < tile_map_size_x*tile_map_size_y*6*6; i++) {
		pf_tile_map[i].state = PF_STATE_NONE;
		pf_tile_map[i].parent = NULL;
	}

	pf_open.tiles = (PF_TILE **)calloc(tile_map_size_x*tile_map_size_y*6*6, sizeof(PF_TILE*));
	pf_open.count = 0;
	
	pf_add_tile_to_open_list(NULL, pf_src_tile);
	
	while ((pf_cur_tile = pf_get_next_open_tile())) {
		if (pf_cur_tile == pf_dst_tile) {
			pf_follow_path = 1;
			break;
		}
		
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x, pf_cur_tile->y+1));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x+1, pf_cur_tile->y+1));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x+1, pf_cur_tile->y));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x+1, pf_cur_tile->y-1));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x, pf_cur_tile->y-1));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x-1, pf_cur_tile->y-1));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x-1, pf_cur_tile->y));
		pf_add_tile_to_open_list(pf_cur_tile, pf_get_tile(pf_cur_tile->x-1, pf_cur_tile->y+1));
	}
	
	free(pf_open.tiles);
	
	return pf_follow_path;
}

void pf_destroy_path()
{
	pf_follow_path = 0;
}

actor *pf_get_our_actor()
{
	int i;
	
	if (yourself == -1) {
		log_error("Self ID Unknown!");
        return NULL;
	}

	for (i = 0; i < max_actors; i++) {
        if (actors_list[i])
        {
		    if(actors_list[i]->actor_id == yourself) {
			    return actors_list[i];
		    }
        }
	}
	log_error("Can't find self in actors_list!\n");
	return NULL;
}

int pf_move()
{
	int x, y;
	actor *me;
	
	if(pf_follow_path != 1 || !(me = pf_get_our_actor())) {
        return 0;
	}
	
	if(me->fighting == 1) {
        return 0;
    }
	
	x = me->x_tile_pos;
	y = me->y_tile_pos;
	
	// Do we need to re-evaluate ?
	if ((PF_DIFF(x, pf_dst_tile->x) < 2 && PF_DIFF(y, pf_dst_tile->y) < 2)) {
        pf_destroy_path();
	} else {
		PF_TILE *t = pf_get_tile(x, y);
		int i = 0, j = 0;
		
		for (pf_cur_tile = pf_dst_tile; pf_cur_tile; pf_cur_tile = pf_cur_tile->parent) {
			if (pf_cur_tile == t) {
				break;
			}
			i++;
		}

		if (pf_cur_tile == t) {
			for (pf_cur_tile = pf_dst_tile; pf_cur_tile; pf_cur_tile = pf_cur_tile->parent) {
				if (j++ == i-12) {
					break;
				}
			}
			if (pf_cur_tile) {
				Uint8 str[5];

				str[0] = MOVE_TO;
				*((short *)(str+1)) = pf_cur_tile->x;
				*((short *)(str+3)) = pf_cur_tile->y;
				send_to_server(str, 5);

				return 1;
			}
		}

		for (pf_cur_tile = pf_dst_tile; pf_cur_tile; pf_cur_tile = pf_cur_tile->parent) {
			if (PF_DIFF(x, pf_cur_tile->x) <= 12 && PF_DIFF(y, pf_cur_tile->y) <= 12
			&& !pf_is_tile_occupied(pf_cur_tile->x, pf_cur_tile->y)) {
				Uint8 str[5];
				
				str[0] = MOVE_TO;
				*((short *)(str+1)) = pf_cur_tile->x;
				*((short *)(str+3)) = pf_cur_tile->y;
				send_to_server(str, 5);
				return 1;
			}
		}
	}
}

int pf_is_tile_occupied(int x, int y)
{
	int i;
    
	for (i = 0; i < max_actors; i++) {
		if(actors_list[i]) {
			if (actors_list[i]->x_tile_pos == x && actors_list[i]->y_tile_pos == y) {
				return 1;
			}
		}
	}
	
	return 0;
}

// Return if we can continue, or are busy
int timed_pf_move(Uint32 time)
{
     if(bot_map.cur_map == -1) return 1;
     
     if(pf_follow_path == 1) {
         if(!pf_move()) log_info("Cannot move!");
         return 1;        
     }
     else if(bot_map.map[bot_map.cur_map].id != CONFIG_NULL && pf_follow_path == 0) {
          walk_to_base_map();
     }
     return 1;
} 

// Emulate click functionality, so we don't stick
/*********************************************/
/* FIXME: This sometimes gives us an invalid */
/*        position. This should be fixed.    */
/*********************************************/
int pseudo_pf_find_path(int x, int y)
{ 
    int ux = x, uy = y;
    
    if (pf_find_path(x, y)) {
		return 1;
	}
	
	for (x = ux-4; x <= ux+4 ; x++) {
		for (y = uy-4; y <= uy+4; y++) {
			if (x == ux && y == uy) {
				continue;
			}
			if (pf_find_path(x, y)) {
				return 1;
			}
		}
	}
	return  0;
}
