// INCLUDES
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "includes.h"
#include "pathfinder.h"

// MACRO DEFINITIONS
#define MALE        1  
#define FEMALE      2

// EXTERN GLOBAL VARIABLES
/*extern char name[40];
extern char guild[5];
extern char my_guild[5];
extern char boss_name[40];
extern long queue_len;
extern int hail_everyone;
extern int hail_master;
*/
// GLOBAL VARIABLES
actor *actors_list[1000];
int max_actors = 0;

// STRUCTURE DEFINITIONS
struct EquipStrings {
       char *Weapon;
       char *bodyarmor;
       char *legarmor;
       char *Boots;
       char *Helmet;
       char *Cape;
       char *Shield;
};

// PROTOTYPES
struct kill_queue *first_node = NULL, *last_node = NULL;
#ifdef DETECT_EQUIP
void process_equipment(int equipment_part, Uint8 equipment_id, struct EquipStrings *E);
#endif


void add_actor_from_server(char * in_data, int kill)
{      
    int i;
    int this_actor_id = *((short *)(in_data));
    struct kill_queue *new_node;
    
    if(debug >= DEBUG_LOW)log_info("Adding actor. Max Actors was %d.\n", max_actors);
             
    for(i=0; i < max_actors+1; i++) {
             if(!actors_list[i]) {
                  actors_list[i] = malloc(sizeof(actor));
                  break;
             }
             else if(actors_list[i]->actor_id == this_actor_id) {
                  // Duplicate Actor ID - Overwrite it
                  log_error("Duplicate actor_id %d.\n", this_actor_id);
                  actors_list[i]->actor_id=*((short *)(in_data));
                  actors_list[i]->fighting=0;
	              actors_list[i]->x_tile_pos=*((short *)(in_data+2));
	              actors_list[i]->y_tile_pos=*((short *)(in_data+4));
	              actors_list[i]->actor_type=*((short *)(in_data+10));
	              my_strncp(actors_list[i]->actor_name,&in_data[23],30); //<--Actor
	              if((actors_list[i]->actor_type == 1) || (actors_list[i]->actor_type == 4)) {
	                   my_strncp(actors_list[i]->actor_name,&in_data[28],30); //<--Enhanced
                  }
                  // Don't alloc kill_queue, because it already will/won't exist
                  return;
             }                  
    }
    if (i >= max_actors+1) return;
    
    actors_list[i]->actor_id=*((short *)(in_data));
    actors_list[i]->fighting = 0;
	actors_list[i]->x_tile_pos=*((short *)(in_data+2));
	actors_list[i]->y_tile_pos=*((short *)(in_data+4));
	actors_list[i]->actor_type=*((short *)(in_data+10));
	my_strncp(actors_list[i]->actor_name,&in_data[23],30); //<--Actor
	if((actors_list[i]->actor_type == 1) || (actors_list[i]->actor_type == 4)) {
	    my_strncp(actors_list[i]->actor_name,&in_data[28],30); //<--Enhanced
    }
    if(kill == 1) {
        new_node = malloc(sizeof(struct kill_queue));
        new_node->time = SDL_GetTicks() + 5000;      // Though this will only chase newbies ...
        new_node->k = actors_list[i];
        actors_list[i]->k = new_node;
        if(first_node == NULL) {
            if(debug >= DEBUG_LOW)log_info("Adding to Front\n");          
            first_node = new_node;
            last_node = new_node;
            first_node->next = NULL;
            first_node->prev = NULL;
        } else {
            if(debug >= DEBUG_LOW)log_info("Adding to Back\n");     
            last_node->next = new_node;
            new_node->prev = last_node;
            new_node->next = NULL;
            last_node = new_node;
        }
    }
    else
    {
        actors_list[i]->k = NULL;
    }
    max_actors++;
    if(debug >= DEBUG_LOW)log_info("Finished adding actor.\n");
}

void destroy_all_actors()
{
     int i = 0;
     struct kill_queue *tofree;
     
     first_node = last_node = NULL;     
     if(debug >= DEBUG_LOW)log_info("Destroy all actors.\n");
               
     for(i=0;i<max_actors;i++)
		{
			if(actors_list[i])
				{
                     actors_list[i]->actor_id=NULL;
	                 actors_list[i]->x_tile_pos=NULL;
	                 actors_list[i]->y_tile_pos=NULL;
	                 actors_list[i]->actor_type=NULL;
	                 actors_list[i]->fighting=NULL;
	                 
                     if(actors_list[i]->k != NULL) {
                          tofree = actors_list[i]->k;
                          tofree->prev=NULL;
                          tofree->next=NULL;
                          tofree->k=NULL;
                          tofree->time=NULL;
                          free(tofree);
                     }                                 
                     actors_list[i]->k = tofree = NULL;
                     
	                 free(actors_list[i]);
                     actors_list[i]=NULL;
                }
        }
        max_actors = 0;
        if(debug >= DEBUG_LOW)log_info("Finished destroying all actors.\n");    
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
                            if(debug >= DEBUG_LOW)log_info("Destroy actor. Max Actors was %d.\n", max_actors);                                                          
                            actors_list[i]->actor_id=NULL;
	                        actors_list[i]->x_tile_pos=NULL;
	                        actors_list[i]->y_tile_pos=NULL;
	                        actors_list[i]->actor_type=NULL;
                            actors_list[i]->fighting=NULL;                              
                            
                            if(actors_list[i]->k != NULL) {
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
                                  actors_list[i]->k = NULL;
                                  free(tofree);
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
                else log_error("Hole in actors_list!\n");
        }
        if(debug >= DEBUG_LOW)log_info("Finished destroying actor.\n");
}


//Look over the actor info
void check_actor_equip(char * in_data)
{
    int gender=0;//NULL until set 
    int this_actor_id=*((short *)(in_data));
    int kill = 0;
    Uint8 actor_type;
    
    #ifdef DETECT_EQUIP
	Uint8 weapon, skin, hair, shirt, pants, boots, frame, cape, head;
	Uint8 shield, helmet;
    struct EquipStrings e;
    #endif

	actor_type=*((short *)(in_data+10));
	if(((actor_type % 2 ==0) && actor_type < 5) || (((actor_type-1) % 2 ==0) && (actor_type-1) < 41) && actor_type > 36)gender=FEMALE;
 	else
 	{
  	   if((((actor_type-1) % 2 ==0) && (actor_type-1) < 5) || (((actor_type-2) % 2 ==0) && (actor_type-2) < 41) && actor_type > 36)gender=MALE;
       else log_error("Actor_id %d named %s has no gender! The actor_type (numerical) is %d (Maybe an NPC!?)\n",this_actor_id,in_data+28,actor_type);    
    }
   	if(this_actor_id != yourself)kill = check_if_we_should_hail (((char *) (in_data+28)), gender);
   	else kill = 0;
   	
   	// Keep the size down for some versions
   	#ifdef DETECT_EQUIP
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
	#endif
	
	//Memorize equipment
	if(this_actor_id != yourself && strcasecmp(my_guild, guild) && strcasecmp(name, boss_name) && name[0] != 3)
	{
        #ifdef DETECT_EQUIP             
        if(hail_master == 1) {
            process_equipment(KIND_OF_WEAPON, weapon, &e);
            process_equipment(KIND_OF_SHIELD, shield, &e);
            process_equipment(KIND_OF_HELMET, helmet, &e);
            process_equipment(KIND_OF_LEG_ARMOR, pants, &e);
            process_equipment(KIND_OF_BODY_ARMOR, shirt, &e);	
            process_equipment(KIND_OF_BOOT_ARMOR, boots, &e);	
            send_pm("%s I see %s, with Weapon: %s, Shield: %s,  Helmet: %s,",
	        boss_name, name, e.Weapon, e.Shield, e.Helmet);
	        send_pm("%s Leg Armor: %s, Body Armor: %s and Boots: %s", boss_name,
            e.legarmor, e.bodyarmor, e.Boots);
        }
        #endif
     }
     add_actor_from_server(in_data, ((insult_enemy ==1) ? kill : 0));    
}


void walk_to_base_map()
{
     actor *me;
     
     if(bot_map.cur_map == -1) return;     
     if(pf_follow_path == 1 || pf_follow_path == 2 || bot_map.map[bot_map.cur_map].id == CONFIG_NULL) return;   
                       // Already moving/home/nowhere
     else if((me=pf_get_our_actor())) {
          if(me->x_tile_pos < bot_map.map[bot_map.cur_map].x+5 && 
             me->x_tile_pos > bot_map.map[bot_map.cur_map].x-5 &&
             me->y_tile_pos < bot_map.map[bot_map.cur_map].y+5 && 
             me->y_tile_pos > bot_map.map[bot_map.cur_map].y-5) {
                 use_object(bot_map.map[bot_map.cur_map].id);  // We're here, enter
                 return;
             }
          else {
               pf_follow_path = 2;
               if(!(pseudo_pf_find_path(bot_map.map[bot_map.cur_map].x, bot_map.map[bot_map.cur_map].y)))
                    log_info("Cannot Move!\n");
                                         // ^- Go there
               if(debug >= DEBUG_MEDIUM) log_info("Moving %dx%d\n", bot_map.map[bot_map.cur_map].x,bot_map.map[bot_map.cur_map].y);
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

#ifdef DETECT_EQUIP
void process_equipment(int equipment_part, Uint8 equipment_id, struct EquipStrings *E)
{
    if(equipment_part == KIND_OF_WEAPON)
    {
        //None
        if(equipment_id == WEAPON_NONE)
        {
            E->Weapon="None";
            return;
        }
            
        //Normal Swords
        if(equipment_id >= SWORD_1 && equipment_id <= SWORD_7)
        {
            switch(equipment_id) {
                case SWORD_1: E->Weapon="Iron Sword";
                    return;
                case SWORD_2: E->Weapon="Iron Broad Sword";
                    return;
                case SWORD_3: E->Weapon="Steel Long Sword";
                    return;
                case SWORD_4: E->Weapon="Steel Two-Edged Sword";
                    return;
                case SWORD_5: E->Weapon="Titanium-Steel Alloy Short Sword";
                    return;
                case SWORD_6: E->Weapon="Titanium-Steel Alloy Long Sword";
                    return;
                case SWORD_7: E->Weapon="Titanium Serpent Sword";
                    return;
                default: E->Weapon="None";
                    return;
            }
        }
        
        //Staffs and hammers
        if(equipment_id >= STAFF_1 && equipment_id <= HAMMER_2)
        {
            switch(equipment_id) {
                case STAFF_1: E->Weapon="Wooden Staff";
                    return;
                case STAFF_2: E->Weapon="Quarter Staff";
                    return;
                case STAFF_3: E->Weapon="Teh Magicz-rz Staff 1";
                    return;
                case STAFF_4: E->Weapon="Teh Magicz-rz Staff 2";
                    return;
                case HAMMER_1: E->Weapon="Wooden Battle Hammer";
                    return;
                case HAMMER_2: E->Weapon="Iron Battle Hammer";
                    return;
                default: E->Weapon="None";
                    return;
            }
        }
        
        //Pickaxe
        if(equipment_id == PICKAX)
        {
            E->Weapon="Pickaxe";
            return;
        }
        
        //Enhanced swords: Iron Fire to Steel 2-edged Thermal                                                                        
        if(equipment_id >= SWORD_1_FIRE && equipment_id <= SWORD_4_THERMAL)
        {
            switch(equipment_id) {
                case SWORD_1_FIRE: E->Weapon="Fire Iron Sword";
                    return;
                case SWORD_2_FIRE: E->Weapon="Fire Iron Broad Sword";
                    return;
                case SWORD_2_COLD: E->Weapon="Cold Iron Broad Sword";
                    return;
                case SWORD_3_FIRE: E->Weapon="Fire Steel Long Sword";
                    return;
                case SWORD_3_COLD: E->Weapon="Cold Steel Long Sword";
                    return;
                case SWORD_3_MAGIC: E->Weapon="Magic Steel Long Sword";
                    return;
                case SWORD_4_FIRE: E->Weapon="Fire Steel Two-edged Sword";
                    return;
                case SWORD_4_COLD: E->Weapon="Cold Steel Two-edged Sword";
                    return;
                case SWORD_4_MAGIC: E->Weapon="Magic Steel Two-edged Sword";
                    return;
                case SWORD_4_THERMAL: E->Weapon="Thermal Steel Two-edged Sword";
                    return;
                default: E->Weapon="None";
                    return;
            }
        }
        
        //Enhanced swords: Titanium-Steel Fire to Titanium Serpent Thermal                                                                        
        if(equipment_id >= SWORD_5_FIRE && equipment_id <= SWORD_7_THERMAL)
        {
            switch(equipment_id) {
                case SWORD_5_FIRE: E->Weapon="Fire Titanium-Steel Short Sword";
                    return;
                case SWORD_5_COLD: E->Weapon="Cold Titanium-Steel Short Sword";
                    return;
                case SWORD_5_MAGIC: E->Weapon="Magic Titanium-Steel Short Sword";
                    return;
                case SWORD_5_THERMAL: E->Weapon="Thermal Titanium-Steel Short Sword";
                    return;
                case SWORD_6_FIRE: E->Weapon="Fire Titanium-Steel Long Sword";
                    return;
                case SWORD_6_COLD: E->Weapon="Cold Titanium-Steel Long Sword";
                    return;
                case SWORD_6_MAGIC: E->Weapon="Magic Titanium-Steel Long Sword";
                    return;
                case SWORD_6_THERMAL: E->Weapon="Thermal Titanium-Steel Long Sword";
                    return;
                case SWORD_7_FIRE: E->Weapon="Fire Titanium Serpent Sword";
                    return;
                case SWORD_7_COLD: E->Weapon="Cold Titanium Serpent Sword";
                    return;
                case SWORD_7_MAGIC: E->Weapon="Magic Titanium Serpent Sword";
                    return;
                case SWORD_7_THERMAL: E->Weapon="Thermal Titanium Serpent Sword";
                    return;
                default: E->Weapon="None";
                    return;
            }
        }
        
        //Magic Pickaxe
        if(equipment_id == PICKAX_MAGIC)
        {
            E->Weapon="Magic Pickaxe";
            return;
        }
        
        //Iron Battleaxe to Titanium magic Battleaxe
        if(equipment_id >= BATTLEAXE_IRON && equipment_id <= BATTLEAXE_TITANIUM_MAGIC)
        {
            switch(equipment_id) {
                case BATTLEAXE_IRON: E->Weapon="Iron Battleaxe";
                    return;
                case BATTLEAXE_STEEL: E->Weapon="Steel Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM: E->Weapon="Titanium Battleaxe";
                    return;
                case BATTLEAXE_IRON_FIRE: E->Weapon="Fire Iron Battleaxe";
                    return;
                case BATTLEAXE_STEEL_COLD: E->Weapon="Cold Steel Battleaxe";
                    return;
                case BATTLEAXE_STEEL_FIRE: E->Weapon="Fire Steel Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_COLD: E->Weapon="Cold Titanium Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_FIRE: E->Weapon="Fire Titanium Battleaxe";
                    return;
                case BATTLEAXE_TITANIUM_MAGIC: E->Weapon="Magic Titanium Battleaxe";
                    return;
                default: E->Weapon="None";
                    return;
            }
        }
    }
    
    if(equipment_part == KIND_OF_SHIELD)
    {
        switch(equipment_id)
        {
            case SHIELD_WOOD: E->Shield="Wooden E->Shield";
                 return;
            case SHIELD_WOOD_ENHANCED: E->Shield="Enhanced Wooden E->Shield";
                 return;
            case SHIELD_IRON: E->Shield="Iron E->Shield";
                 return;
            case SHIELD_STEEL: E->Shield="Steel E->Shield";
                 return;
            default: E->Shield="None";
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
            case HELMET_IRON: E->Helmet="Iron Helm";
                 return;
            case HELMET_FUR: E->Helmet="Fur Cap";
                 return;
            case HELMET_LEATHER: E->Helmet="Leather Helm";
                 return;
            default: E->Helmet="None";
                 return;
        }    
    }
    if(equipment_part == KIND_OF_LEG_ARMOR)
    {
        switch(equipment_id)
        {
            case PANTS_LEATHER: E->legarmor="Leather Pants";
                 return;
            case PANTS_IRON_CUISSES: E->legarmor="Iron Cuisses";
                 return;
            default: E->legarmor="None";
                 return;
        }    
    }
    if(equipment_part == KIND_OF_BODY_ARMOR)
    {
        switch(equipment_id)
        {
            case SHIRT_LEATHER_ARMOR: E->bodyarmor="Leather Armor";
                 return;
            case SHIRT_CHAIN_ARMOR: E->bodyarmor="Iron Chainmail";
                 return;
            case SHIRT_STEEL_CHAIN_ARMOR: E->bodyarmor="Steel Chainmail";
                 return;
            case SHIRT_TITANIUM_CHAIN_ARMOR: E->bodyarmor="Titanium Chainmail";
                 return;
            case SHIRT_IRON_PLATE_ARMOR: E->bodyarmor="Iron Plate";
                 return;
            case SHIRT_ARMOR_6: E->bodyarmor="Some kind of shirt armor!?";
                 return;
            default: E->bodyarmor="None";
                 return;
        }
    }
    if(equipment_part == KIND_OF_BOOT_ARMOR)
    {
        switch(equipment_id)
        {
            case BOOTS_LEATHER: E->Boots="Leather Boots";
                return;
            case BOOTS_FUR: E->Boots="Fur Boots";
                return;
            case BOOTS_IRON_GREAVE: E->Boots="Iron Greaves";
                return;
            default: E->Boots="None";
                return;
        }    
    }
}
#endif   
