/*Includes*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "includes.h"
/*Definitions*/
#define MALE 1  
#define FEMALE 2
extern char name[40];
extern char guild[5];
extern char my_guild[5];
extern char boss_name[40];
extern long queue_len;
extern int hail_everyone;
extern int hail_master;
char *Weapon;
char *bodyarmor;
char *legarmor;
char *Boots;
char *Helmet;
char *Cape;
char *Shield;

struct kill_queue *first_node = NULL, *last_node = NULL;
/*Prototypes*/
void process_equipment(int equipment_part, Uint8 equipment_id);

actor *actors_list[1000];
int max_actors = 0;

void add_actor_from_server(char * in_data, int kill)
{     
    int i;
    struct kill_queue *new_node;
             
    for(i=0; i < max_actors+1; i++) {
             if(!actors_list[i]) {
                  actors_list[i] = calloc(1, sizeof(actor));
                  break;
             }
    }
    if (i >= max_actors+1) return;
    
    actors_list[i]->actor_id=*((short *)(in_data));
    actors_list[i]->fighting=0;
	actors_list[i]->x_tile_pos=*((short *)(in_data+2));
	actors_list[i]->y_tile_pos=*((short *)(in_data+4));
	actors_list[i]->actor_type=*((short *)(in_data+10));
	my_strncp(actors_list[i]->actor_name,&in_data[23],30); //<--Actor
	if((actors_list[i]->actor_type == 1) || (actors_list[i]->actor_type == 4)) {
	    my_strncp(actors_list[i]->actor_name,&in_data[28],30); //<--Enhanced
    }
    if(kill == 1) {
        new_node = calloc(sizeof(struct kill_queue), 1);
        new_node->time = SDL_GetTicks() + 10000;
        new_node->k = i;
        actors_list[i]->k = new_node;
        if(first_node == NULL) {
            first_node = new_node;
            last_node = new_node;
            first_node->next = NULL;
            first_node->prev = NULL;
        } else {  
             last_node->next = new_node;
             new_node->prev = last_node;
             new_node->next = NULL;
             last_node = new_node;
        }
    }    
    max_actors++;
}

//POSSIBLE FIXME
void destroy_all_actors()
{
     int i = 0;
     
     first_node = last_node = NULL;
          
     for(i=0;i<max_actors;i++)
		{
			if(actors_list[i])
				{
                     actors_list[i]->actor_id=NULL;
	                 actors_list[i]->x_tile_pos=NULL;
	                 actors_list[i]->y_tile_pos=NULL;
	                 actors_list[i]->actor_type=NULL;
	                 actors_list[i]->fighting=NULL;
	                 
                     if(actors_list[i]->k) {
                          actors_list[i]->k->prev=NULL;
                          actors_list[i]->k->next=NULL;
                          actors_list[i]->k->k=NULL;
                          actors_list[i]->k->time=NULL;
                          free(actors_list[i]->k);                          
                     }                                 
                     actors_list[i]->k=NULL;
                     
	                 free(actors_list[i]);
                     actors_list[i]=NULL;
                }
        }    
}

void destroy_actor(int actor_id)
{
     int i = 0;
     struct kill_queue *tofree;
          
     for(i=0;i<max_actors;i++)
		{
			if(actors_list[i])
				{
                 	if(actors_list[i]->actor_id==actor_id)
                 		{
                            actors_list[i]->actor_id=NULL;
	                        actors_list[i]->x_tile_pos=NULL;
	                        actors_list[i]->y_tile_pos=NULL;
	                        actors_list[i]->actor_type=NULL;
                            actors_list[i]->fighting=NULL;                              
                            
                            if(actors_list[i]->k) {
                                  tofree = actors_list[i]->k;
                                  if(tofree->prev && tofree->next) {
                                       tofree->prev->next = tofree->next;
                                       tofree->next->prev = tofree->prev;
                                  }
                                 else if(!tofree->next && tofree->prev) {
                                       last_node = tofree->prev;
                                       tofree->prev->next = NULL;
                                  }
                                  else if(!tofree->prev && tofree->next) {
                                       first_node = tofree->next;
                                       tofree->next->prev = NULL;
                                  }
                                  else if(!tofree->prev && !tofree->next) {
                                       first_node = NULL;
                                       last_node = NULL;
                                  } 
                                  tofree->prev=NULL;
                                  tofree->next=NULL;
                                  tofree->k=NULL;
                                  tofree->time=NULL;
                                  free(tofree);
                                  actors_list[i]->k = NULL;
                            }                                 
                                                         
	                 		free(actors_list[i]);
	                 		actors_list[i]=NULL;
	                 		max_actors--;
				            if(i<max_actors)
						        {
							         //copy the last one down and fill in the hole
							         actors_list[i]=actors_list[max_actors];
							         actors_list[max_actors]=NULL;
						        }
                        }
                }
        }
}


//Look over the actor info
void check_actor_equip(char * in_data)
{
    int gender=0;//NULL until set 
    int this_actor_id=*((short *)(in_data));
    int actor_enhanced=1; 
    int kill = 0;
	Uint8 weapon;    
    Uint8 actor_type;
	Uint8 skin;
	Uint8 hair;
	Uint8 shirt;
	Uint8 pants;
	Uint8 boots;
	Uint8 frame;
	Uint8 cape;
	Uint8 head;
	Uint8 shield;
	Uint8 helmet;
	//x_tile_pos=*((short *)(in_data+2));
	//y_tile_pos=*((short *)(in_data+4));
	actor_type=*((short *)(in_data+10));
	if(((actor_type % 2 ==0) && actor_type < 5) || (((actor_type-1) % 2 ==0) && (actor_type-1) < 41) && actor_type > 36)gender=FEMALE;
 	else
 	{
  	   if((((actor_type-1) % 2 ==0) && (actor_type-1) < 5) || (((actor_type-2) % 2 ==0) && (actor_type-2) < 41) && actor_type > 36)gender=MALE;
       else log_error("Actor_id %d named %s has no gender! The actor_type (numerical) is %d (Maybe an NPC!?)\n",this_actor_id,in_data+28,actor_type);    
    }
   	kill = check_if_we_should_hail (((char *) (in_data+28)), gender);
    skin=*(in_data+12);
	hair=*(in_data+13);
	shirt=*(in_data+14);
	pants=*(in_data+15);
	boots=*(in_data+16);
	shield=*(in_data+18);
	weapon=*(in_data+19);
	if(weapon != WEAPON_NONE && this_actor_id != yourself && (strcasecmp (my_guild, guild)))
       if(hail_everyone == 1)send_raw_text("%s, remove your weapons before entering my home!", name);
	cape=*(in_data+20);
	helmet=*(in_data+21);
	//Memorize equipment
	if(this_actor_id != yourself && strcasecmp(my_guild, guild) && strcasecmp(name, boss_name) && name[0] != 3)
	{
        if(hail_master == 1) {
            process_equipment(KIND_OF_WEAPON, weapon);
            process_equipment(KIND_OF_SHIELD, shield);
            process_equipment(KIND_OF_HELMET, helmet);
            process_equipment(KIND_OF_LEG_ARMOR, pants);
            process_equipment(KIND_OF_BODY_ARMOR, shirt);	
            process_equipment(KIND_OF_BOOT_ARMOR, boots);	
            send_pm("%s I see %s, with Weapon: %s, Shield: %s,  Helmet: %s,",
	        boss_name, name, Weapon, Shield, Helmet);
	        send_pm("%s Leg Armor: %s, Body Armor: %s and Boots: %s", boss_name,
            legarmor, bodyarmor, Boots);
        }
     }
     add_actor_from_server(in_data, ((insult_enemy ==1) ? kill : 0));    
}

/*
Underworld - 98, 171 * 18
IP - 22, 21 * 441
WS - 720, 137 * 1014 
DP - 360, 331 * 1884
*/
ENTRY_MAP bot_path[4]={
     {98, 171, 18},
     {22, 21, 441},
     {720, 137, 1014},
     {360, 331, 1884},
};

void walk_to_base_map()
{
     actor *me;
          
     if(pf_follow_path == 1 || pf_follow_path == 2 || cur_map == 1337 || cur_map == -5) return;   
                       // Already moving/home/nowhere
     else if((me=pf_get_our_actor())) {
          if(me->x_tile_pos < bot_path[cur_map].x+5 && 
             me->x_tile_pos > bot_path[cur_map].x-5 &&
             me->y_tile_pos < bot_path[cur_map].y+5 && 
             me->y_tile_pos > bot_path[cur_map].y-5) {
                 use_object(bot_path[cur_map].id);  // We're here, enter
                 cur_map++;                         // Next map ...
                 return;
             }
          else {
               pf_follow_path = 2;
               pseudo_pf_find_path(bot_path[cur_map].x, bot_path[cur_map].y);
                                         // ^- Go there
               if(debug >= 3) log_info("Moving %dx%d\n", bot_path[cur_map].x, bot_path[cur_map].y);
               return;
          }
     }
}

void process_command(int actor, char command)
{    
     int i;
               
     for(i = 0; i < max_actors; i++) {
           if(!actors_list[i])continue;
           if(actors_list[i]->actor_id == actor) break;
     }
     if(i >= max_actors) return;
     switch(command) {
          case move_n:
          case run_n:
				actors_list[i]->y_tile_pos++;break;
          case move_s:
          case run_s:
				actors_list[i]->y_tile_pos--;break;
          case move_e:
          case run_e:
				actors_list[i]->x_tile_pos++;break;
          case move_w:
          case run_w:
				actors_list[i]->x_tile_pos--;break;
          case move_ne:
          case run_ne:
				actors_list[i]->x_tile_pos++;
				actors_list[i]->y_tile_pos++;
          break;
          case move_se:
          case run_se:
				actors_list[i]->x_tile_pos++;
				actors_list[i]->y_tile_pos--;
          break;
          case move_sw:
          case run_sw:
				actors_list[i]->x_tile_pos--;
				actors_list[i]->y_tile_pos--;
          break;
          case move_nw:
          case run_nw:
				actors_list[i]->x_tile_pos--;
				actors_list[i]->y_tile_pos++;
          break;
          case enter_combat:
               actors_list[i]->fighting=1;
          break;
          case leave_combat:
				actors_list[i]->fighting=0;
          break;
	 }
}


void attack(int id)
{
    Uint8 str[10];
    
    str[0]=ATTACK_SOMEONE;
    *((int *)(str+1))=id;
    send_to_server(str,5);
}

void use_object(int id)
{
     Uint8 str[10];
     
     str[0]=USE_MAP_OBJECT;
     *((int *)(str+1))=id;
     *((int *)(str+5))=-1;
     send_to_server(str,9);
}          

void process_equipment(int equipment_part, Uint8 equipment_id)
{
    if(equipment_part == KIND_OF_WEAPON)
    {
        //None
        if(equipment_id == WEAPON_NONE)
        {
            Weapon="None";
            return;
        }
            
        //Normal Swords
        if(equipment_id >= SWORD_1 && equipment_id <= SWORD_7)
        {
            switch(equipment_id) {
                case SWORD_1: Weapon="Iron Sword";
                    return;
                case SWORD_2: Weapon="Iron Broad Sword";
                    return;
                case SWORD_3: Weapon="Steel Long Sword";
                    return;
                case SWORD_4: Weapon="Steel Two-Edged Sword";
                    return;
                case SWORD_5: Weapon="Titanium-Steel Alloy Short Sword";
                    return;
                case SWORD_6: Weapon="Titanium-Steel Alloy Long Sword";
                    return;
                case SWORD_7: Weapon="Titanium Serpent Sword";
                    return;
                default: Weapon="None";
                    return;
            }
        }
        
        //Staffs and hammers
        if(equipment_id >= STAFF_1 && equipment_id <= HAMMER_2)
        {
            switch(equipment_id) {
                case STAFF_1: Weapon="Wooden Staff";
                    return;
                case STAFF_2: Weapon="Quarter Staff";
                    return;
                case STAFF_3: Weapon="Teh Magicz-rz Staff 1";
                    return;
                case STAFF_4: Weapon="Teh Magicz-rz Staff 2";
                    return;
                case HAMMER_1: Weapon="Wooden Battle Hammer";
                    return;
                case HAMMER_2: Weapon="Iron Battle Hammer";
                    return;
                default: Weapon="None";
                    return;
            }
        }
        
        //Pickaxe
        if(equipment_id == PICKAX)
        {
            Weapon="Pickaxe";
            return;
        }
        
        //Enhanced swords: Iron Fire to Steel 2-edged Thermal                                                                        
        if(equipment_id >= SWORD_1_FIRE && equipment_id <= SWORD_4_THERMAL)
        {
            switch(equipment_id) {
                case SWORD_1_FIRE: Weapon="Fire Iron Sword";
                    return;
                case SWORD_2_FIRE: Weapon="Fire Iron Broad Sword";
                    return;
                case SWORD_2_COLD: Weapon="Cold Iron Broad Sword";
                    return;
                case SWORD_3_FIRE: Weapon="Fire Steel Long Sword";
                    return;
                case SWORD_3_COLD: Weapon="Cold Steel Long Sword";
                    return;
                case SWORD_3_MAGIC: Weapon="Magic Steel Long Sword";
                    return;
                case SWORD_4_FIRE: Weapon="Fire Steel Two-edged Sword";
                    return;
                case SWORD_4_COLD: Weapon="Cold Steel Two-edged Sword";
                    return;
                case SWORD_4_MAGIC: Weapon="Magic Steel Two-edged Sword";
                    return;
                case SWORD_4_THERMAL: Weapon="Thermal Steel Two-edged Sword";
                    return;
                default: Weapon="None";
                    return;
            }
        }
        
        //Enhanced swords: Titanium-Steel Fire to Titanium Serpent Thermal                                                                        
        if(equipment_id >= SWORD_5_FIRE && equipment_id <= SWORD_7_THERMAL)
        {
            switch(equipment_id) {
                case SWORD_5_FIRE: Weapon="Fire Titanium-Steel Short Sword";
                    return;
                case SWORD_5_COLD: Weapon="Cold Titanium-Steel Short Sword";
                    return;
                case SWORD_5_MAGIC: Weapon="Magic Titanium-Steel Short Sword";
                    return;
                case SWORD_5_THERMAL: Weapon="Thermal Titanium-Steel Short Sword";
                    return;
                case SWORD_6_FIRE: Weapon="Fire Titanium-Steel Long Sword";
                    return;
                case SWORD_6_COLD: Weapon="Cold Titanium-Steel Long Sword";
                    return;
                case SWORD_6_MAGIC: Weapon="Magic Titanium-Steel Long Sword";
                    return;
                case SWORD_6_THERMAL: Weapon="Thermal Titanium-Steel Long Sword";
                    return;
                case SWORD_7_FIRE: Weapon="Fire Titanium Serpent Sword";
                    return;
                case SWORD_7_COLD: Weapon="Cold Titanium Serpent Sword";
                    return;
                case SWORD_7_MAGIC: Weapon="Magic Titanium Serpent Sword";
                    return;
                case SWORD_7_THERMAL: Weapon="Thermal Titanium Serpent Sword";
                    return;
                default: Weapon="None";
                    return;
            }
        }
        
        //Magic Pickaxe
        if(equipment_id == PICKAX_MAGIC)
        {
            Weapon="Magic Pickaxe";
            return;
        }
        
        //Iron Battleaxe to Titanium magic Battleaxe
        if(equipment_id >= BATTLEAXE_IRON && equipment_id <= BATTLEAXE_TITANIUM_MAGIC)
        {
            switch(equipment_id) {
                case BATTLEAXE_IRON: Weapon="Iron Battleaxe";
                    return;
                case BATTLEAXE_STEEL: Weapon="Steel Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM: Weapon="Titanium Battleaxe";
                    return;
                case BATTLEAXE_IRON_FIRE: Weapon="Fire Iron Battleaxe";
                    return;
                case BATTLEAXE_STEEL_COLD: Weapon="Cold Steel Battleaxe";
                    return;
                case BATTLEAXE_STEEL_FIRE: Weapon="Fire Steel Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_COLD: Weapon="Cold Titanium Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_FIRE: Weapon="Fire Titanium Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_MAGIC: Weapon="Magic Titanium Battleaxe";
                    return;
                default: Weapon="None";
                    return;
            }
        }
    }
    
    if(equipment_part == KIND_OF_SHIELD)
    {
        switch(equipment_id)
        {
            case SHIELD_WOOD: Shield="Wooden Shield";
                 return;
            case SHIELD_WOOD_ENHANCED: Shield="Enhanced Wooden Shield";
                 return;
            case SHIELD_IRON: Shield="Iron Shield";
                 return;
            case SHIELD_STEEL: Shield="Steel Shield";
                 return;
            default: Shield="None";
                 return;
        }
    }        

    /*if(equipment_part == KIND_OF_CAPE)
    {
        if(
        
    } */   
    if(equipment_part == KIND_OF_HELMET)
    {
        switch(equipment_id)
        {
            case HELMET_IRON: Helmet="Iron Helm";
                 return;
            case HELMET_FUR: Helmet="Fur Cap";
                 return;
            case HELMET_LEATHER: Helmet="Leather Helm";
                 return;
            default: Helmet="None";
                 return;
        }    
    }
    if(equipment_part == KIND_OF_LEG_ARMOR)
    {
        switch(equipment_id)
        {
            case PANTS_LEATHER: legarmor="Leather Pants";
                 return;
            case PANTS_IRON_CUISSES: legarmor="Iron Cuisses";
                 return;
            default: legarmor="None";
                 return;
        }    
    }
    if(equipment_part == KIND_OF_BODY_ARMOR)
    {
        switch(equipment_id)
        {
            case SHIRT_LEATHER_ARMOR: bodyarmor="Leather Armor";
                 return;
            case SHIRT_CHAIN_ARMOR: bodyarmor="Iron Chainmail";
                 return;
            case SHIRT_STEEL_CHAIN_ARMOR: bodyarmor="Steel Chainmail";
                 return;
            case SHIRT_TITANIUM_CHAIN_ARMOR: bodyarmor="Titanium Chainmail";
                 return;
            case SHIRT_IRON_PLATE_ARMOR: bodyarmor="Iron Plate";
                 return;
            case SHIRT_ARMOR_6: bodyarmor="Some kind of shirt armor!?";
                 return;
            default: bodyarmor="None";
                 return;
        }
    }
    if(equipment_part == KIND_OF_BOOT_ARMOR)
    {
        switch(equipment_id)
        {
            case BOOTS_LEATHER: Boots="Leather Boots";
                return;
            case BOOTS_FUR: Boots="Fur Boots";
                return;
            case BOOTS_IRON_GREAVE: Boots="Iron Greaves";
                return;
            default: Boots="None";
                return;
        }    
    }
}   