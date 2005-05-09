#include "includes.h"

// Navigation
struct BOT_WORLD bot_map = {
     {}, -1, 0
};


int load_map_config(char *file)
{
    FILE *f = NULL;
    int i, j = 0, m = 0;
    char line[512];
    char buf[512];
    char error[256];
    
    if(!(f = fopen(file, "r")))
    {
          sprintf(error, "No such file: %s", file);             
          log_info(error);
          return 0;
    }
    
    /*****************************/
    /* Format :                  */
    /* name x,y,id               */
    /*****************************/

    while (fgets(line, 512, f)) {
          for(i = 0; line[i] != ' ' && line[i] != '\n' && line[i] != '\0'; i++) {
                bot_map.map[bot_map.loaded_maps].name[i] = line[i];
          }
          bot_map.map[bot_map.loaded_maps].name[i] = '\0';
          i++;
          for(j = 0; line[i] != ',' && line[i] != '\n' && line[i] != '\0'; j++) {
                buf[j] = line[i++];
          }
          buf[j] = '\0';
          bot_map.map[bot_map.loaded_maps].x = atoi(buf);
          i++;
          for(j = 0; line[i] != ',' && line[i] != '\n' && line[i] != '\0'; j++) {
                buf[j] = line[i++];
          }
          buf[j] = '\0';
          bot_map.map[bot_map.loaded_maps].y = atoi(buf);
          i++;
          for(j = 0; line[i] != '\n' && line[i] != '\0'; j++) {
                buf[j] = line[i++];
          }
          buf[j] = '\0';
          bot_map.map[bot_map.loaded_maps].id = atoi(buf);
          bot_map.loaded_maps++;
    }
    
    fclose(f);
}
    


// New check_map
void examine_map(char *name)
{
     int i;
     
     for(i = 0; i < bot_map.loaded_maps; i++) {
           if(!strcmp(bot_map.map[i].name, name))
           {
                bot_map.cur_map = i;
                return;
           }
     }
     log_error("Do not recognize map name %s", name);
     // Sequence to escape... maybe #beam me?
     send_raw_text("#beam me");
}


/******************************************************************************/
/*--------------------------CONFIGURATION LISTS SECTION-----------------------*/
/******************************************************************************/

/**********************/
/*    File Example:   */
/* donpedro           */
/* crusadingknight    */
/* duckman            */
/**********************/

// Max 5 items per each config list, except guilds, which is 10
struct CONFIG_LIST admin = {NULL, 0, 5};
struct CONFIG_LIST A_player = {NULL, 0, 5};
struct CONFIG_LIST A_guild = {NULL, 0, 10};


/******************************************************************************/
/* void get_string USAGE :                                                    */
/*   load_list(&admin, "admin.lst"))                                          */
/*         (All loaded strings are tolower'd)                                 */
/*   return number of nodes on success, 0 on failure                          */
/******************************************************************************/
int load_list(struct CONFIG_LIST *list, char *file)
{
     FILE *f = NULL;
     int i = 0;
     char error[256];
     char line[256];
     struct CONFIG_NODE *cur;
          
	 if(!(f = fopen(file, "r")))
     {
          sprintf(error, "No such file: %s", file);             
          log_info(error);
          return 0;
     }
     
     
     while(fgets(line, 256, f) && list->l < list->m) {
          if(!list->list) {
               cur = list->list = malloc(sizeof(struct CONFIG_NODE));
               cur->prev = list->list->prev = NULL;
               cur->next = NULL;
          }
          else {
               while(cur->next != NULL) cur = cur->next;
               cur->next = malloc(sizeof(struct CONFIG_NODE));
               cur->next->prev = cur;
               cur = cur->next;
               cur->next = NULL;
          }
          for(i = 0; i < 30; i++) {
                  line[i] = tolower(line[i]);
          }
          for(i = 0; line[i] != '\n' && line[i] != ' '; i++) {
                   cur->data[i] = line[i];
          }
          cur->data[i] = '\0';
          cur->len = i-1;
          list->l++;
     }
     fclose(f);
     return list->l;
}

/******************************************************************************/
/* int get_string USAGE :                                                     */
/*   if(get_string(&admin, "CrusaDingKnIGHT", 15) != -1) printf("Pwn3d!1~!"); */
/*         (All strings are tolower'd)                                        */
/*   return list position on success, -1 on failure                           */
/******************************************************************************/
int get_string(struct CONFIG_LIST *list, char * in_name, int len)
{
     int i = 0;
     struct CONFIG_NODE *cur;
     char name[31];
     char lower_name[30];
     
     if(len >= 30) return -1; //Too big to be a name
     if(strlen(in_name) <= 0) return -1;
     
     while(*in_name != '\0') {
          lower_name[i++] = tolower(*in_name);
          in_name++;
     }
     lower_name[i] = '\0';
     
     strcpy(name, tolower(lower_name));
     cur = list->list;
          
     for(i = 0; i < list->l && cur; i++) {
          if(!strcmp(cur->data, name)) return i;
          printf("%s - %s\n", cur->data, name);
          cur = cur->next;
     }

     return -1;
}

/******************************************************************************/
/* int remove_string USAGE :                                                  */
/*   load_list(&admin, "Crusadingknight", strlen("CrusAdingKNiGht"))          */
/*         (All strings are tolower'd)                                        */
/*   return list length on success, 0 on failure                              */ 
/******************************************************************************/
int remove_string(struct CONFIG_LIST *list, char * in_name, int len)
{
     int i = 0;
     struct CONFIG_NODE *cur;
     char name[31];
     char lower_name[30];
     
     if(len >= 30) return 0; //Too big to be a name
     if(strlen(in_name) <= 0) return 0;
     
     while(*in_name != '\0') {
          lower_name[i++] = tolower(*in_name);
          in_name++;
     }
     
     strcpy(name, tolower(lower_name));
     cur = list->list;
     
     for(i = 0; i < list->l && cur; i++) {
          if(!strcmp(cur->data, in_name)) {
                cur->prev->next = cur->next;
                cur->next->prev = cur->prev;
                free(cur);
                return list->l--;
          }
          cur = cur->next;
     }
     
     return 0;
}

/******************************************************************************/
/* int remove_string USAGE :                                                  */
/*   load_list(&admin, "Crusadingknight", strlen("CrusAdingKNiGht"))          */
/*         (All strings are tolower'd)                                        */
/*   return list length on success, 0 on failure                              */     
/******************************************************************************/
int add_string(struct CONFIG_LIST *list, char * in_name, int len)
{
     int i = 0;
     struct CONFIG_NODE *cur;
     char name[31];
     char lower_name[30];
     
     if(len >= 30) return 0; //Too big to be a name
     if(strlen(in_name) <= 0 || list->l >= list->m) return 0;
     
     while(*in_name != '\0') {
          lower_name[i++] = tolower(*in_name);
          in_name++;
     }
     
     strcpy(name, tolower(lower_name));
     cur = list->list;
     while(cur->next) cur = cur->next;
     
     cur->next = malloc(sizeof(struct CONFIG_LIST));
     cur->next->prev = cur;
     cur = cur->next;
     
     strcpy(cur->data, name);
     cur->len = i;
     
     return list->l++;;
}
