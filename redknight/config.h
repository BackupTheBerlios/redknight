#ifndef __CUSTOM_H__
#define __CUSTOM_H__

// MACROS
#define CONFIG_NULL -1  // We use this because object_id 0 CAN exist.


// STRUCTS
struct BOT_MAP {
       int x;
       int y;
       int id;
       char name[256];
};

// I'll load this from file later.
struct BOT_WORLD {
       struct BOT_MAP map[40];
       int cur_map;
       int loaded_maps;
};
extern struct BOT_WORLD bot_map;

struct CONFIG_NODE {
       char data[30];
       int len;
       struct CONFIG_NODE *prev, *next;       
};

struct CONFIG_LIST {
       struct CONFIG_NODE *list;
       int l; // Number of nodes
       int m; // Max number of nodes
};
extern struct CONFIG_LIST admin, A_player, A_guild;


// PROTOTYPES
int load_list(struct CONFIG_LIST *list, char *file);
int get_string(struct CONFIG_LIST *list, char * in_name, int len);
int add_string(struct CONFIG_LIST *list, char * in_name, int len);
int remove_string(struct CONFIG_LIST *list, char * in_name, int len);
void examine_map(char *name);

#endif
