#include <stdio.h>
#include <string.h>

#include "client_serv.h"
#include "includes.h"
#include "elbot.h"
#include "items.h"

ITEM inv[36+8];

ITEM_NAME item_name_list[] = {
	{ ITEM_BREAD, "bread", "bread" },
	{ ITEM_FRUIT, "fruit", "fruit" },
	{ ITEM_VEGETABLES, "vegetable", "vegetables" },
	{ ITEM_GOLD_COINS, "gold coin", "gold coins" },
	{ ITEM_UNKNOWN_BOOK1, "book", "books" },
	{ ITEM_MEAD, "mead", "mead" },
	{ ITEM_WINE, "wine", "wine" },
	{ ITEM_ALE, "ale", "ale" },
	{ ITEM_UNKNOWN_BOOK2, "book", "books" },
	{ ITEM_EMPTY_VIAL, "empty vial", "empty vials" },
	{ ITEM_UNKNOWN_POTION1, "ITEM_UNKNOWN_POTION1", "ITEM_UNKNOWN_POTION1" },
	{ ITEM_UNKNOWN_POTION2, "ITEM_UNKNOWN_POTION2", "ITEM_UNKNOWN_POTION2" },
	{ ITEM_UNKNOWN_POTION3, "ITEM_UNKNOWN_POTION3", "ITEM_UNKNOWN_POTION3" },
	{ ITEM_DEFENSE_POTION, "defense potion", "defense potions" },
	{ ITEM_UNKNOWN_POTION5, "ITEM_UNKNOWN_POTION5", "ITEM_UNKNOWN_POTION5" },
	{ ITEM_UNKNOWN_POTION6, "UNKNOWN_POTION6", "UNKNOWN_POTION6" },
	{ ITEM_UNKNOWN_POTION7, "UNKNOWN_POTION7", "UNKNOWN_POTION7" },
	{ ITEM_PHYSIQEU_POTION, "physique potion", "physique potions" },
	{ ITEM_UNKNOWN_POTION9, "UNKNOWN_POTION9", "UNKNOWN_POTION9" },
	{ ITEM_UNKNOWN_POTION10, "UNKNOWN_POTION10", "UNKNOWN_POTION10" },
	{ ITEM_UNKNOWN_BOOK3, "book", "books" },
	{ ITEM_UNKNOWN_BOOK4, "book", "books" },
	{ ITEM_UNKNOWN_BOOK5, "book", "books" },
	{ ITEM_UNKNOWN_BOOK6, "book", "books" },
	{ ITEM_UNKNOWN_PARCHMENT1, "UNKNOWN_PARCHMENT1", "UNKNOWN_PARCHMENT1" },
	{ ITEM_FLOWER_SUNFLOWER, "sunflower", "sunflowers" },
	{ ITEM_FLOWER_BLUE_STAR_FLOWER, "blue star flower", "blue star flowers" },
	{ ITEM_FLOWER_IMPATIENS, "impatien", "impatiens" },
	{ ITEM_FLOWER_CHRYSANTHEMUMS, "chrysanthemum", "chrysanthemums" },
	{ ITEM_FLOWER_TIGER_LILLY, "tiger lilly", "tiger lillies" },
	{ ITEM_FLOWER_YELLOW_ROSE, "yellow rose", "yellow roses" },
	{ ITEM_FLOWER_RED_ROSE, "red rose", "red roses" },
	{ ITEM_FLOWER_BLACK_ROSE, "black rose", "black roses" },
	{ ITEM_FLOWER_WHITE_ASIATIC_LILLY, "white asiatic lilly", "white asiatic lillies" },
	{ ITEM_FLOWER_BLUE_LUPINE, "blue lupine", "blue lupines" },
	{ ITEM_FLOWER_RED_SNAPDRAGONS, "red snapdragon", "red snapdragons" },
	{ ITEM_FLOWER_LILACS, "lilac", "lilacs" },
	{ ITEM_FLOWER_SWAMP_CANDLES, "swamp candle", "swamp candles" },
	{ ITEM_QUARTZ, "quartz", "quartz" },
	{ ITEM_BLUE_QUARTZ, "blue quartz", "blue quartz" },
	{ ITEM_ROSE_QUARTZ, "rose quartz", "rose quartz" },
	{ ITEM_MERCURY, "mercury", "mercury" },
	{ ITEM_SULPHUR, "sulphur", "sulphur" },
	{ ITEM_SILVER_ORE, "Silver Ore", "Silver Ore" },
	{ ITEM_GOLD_ORE, "Gold Ore", "Gold Ore" },
	{ ITEM_IRON_ORE, "Iron Ore", "Iron Ore" },
	{ ITEM_COAL, "Coal", "Coal" },
	{ ITEM_SERPENT_STONE, "serpent stone", "serpent stones" },
	{ ITEM_UNKNOWN_ITEM1, "UNKNOWN_ITEM1", "UNKNOWN_ITEM1" },
	{ ITEM_UNKNOWN_ITEM2, "UNKNOWN_ITEM2", "UNKNOWN_ITEM2" },
	{ ITEM_FIRE_ESSENCE, "fire essence", "fire essences" },
	{ ITEM_WATER_ESSENCE, "water essence", "water essences" },
	{ ITEM_EARTH_ESSENCE, "earth essence", "earth essences" },
	{ ITEM_AIR_ESSENCE, "air essence", "air essences" },
	{ ITEM_SPIRIT_ESSENCE, "spirit essence", "spirit essences" },
	{ ITEM_MATTER_ESSENCE, "matter essence", "matter essence" },
	{ ITEM_ENERGY_ESSENCE, "energy essence", "energy essence" },
	{ ITEM_LIFE_ESSENCE, "life essence", "life essence" },
	{ ITEM_DEATH_ESSENCE, "death essence", "death essence" },
	{ ITEM_HEALTH_ESSENCE, "health essence", "health essences" },
	{ ITEM_MAGIC_ESSENCE, "magic essence", "magic essences" },
	{ ITEM_DIAMOND, "diamond", "diamonds" },
	{ ITEM_MORTAR_AND_PESTLE, "mortar and pestle", "mortars and pestles" },
	{ ITEM_BONE, "bone", "bones" },
	{ ITEM_CACTUS, "cactus", "cacti" },
	{ ITEM_RAW_MEAT, "raw meat", "raw meat" },
	{ ITEM_DEER_SKIN, "deer skin", "deer skins" },
	{ ITEM_DEER_FUR, "deer fur", "deer furs" },
	{ ITEM_WOLF_FUR, "wolf fur", "wolf furs" },
	{ ITEM_BEAR_FUR, "bear fur", "bear furs" },
	{ ITEM_DEER_ANTLERS, "deer antlers", "deer antlers" },
	{ ITEM_BONE_POWDER, "bone powder", "bone powders" },
	{ ITEM_WHITE_RABBIT_FUR, "white rabbit fur", "white rabbit furs" },
	{ ITEM_BROWN_RABBIT_FUR, "brow rabbit fur", "brown rabbit furs" },
	{ ITEM_COOKED_MEAT, "cooked meat", "cooked meat" },
	{ ITEM_GREEN_SNAKE_SKIN, "green snake skin", "green snake skins" },
	{ ITEM_RED_SNAKE_SKIN, "red snake skin", "red snake skins" },
	{ ITEM_BROWN_SNAKE_SKIN, "brown snake skin", "brown snake skins" },
	{ ITEM_FOX_FUR, "fox fur", "fox furs" },
	{ ITEM_PUMA_FUR, "puma fur", "puma furs" },
	{ ITEM_NEEDLE, "needle", "needles" },
	{ ITEM_THREAD, "thread", "threads" },
	{ ITEM_WARM_FUR_GLOVES, "warm fur gloves", "warm fur gloves" },
	{ ITEM_FOX_SCARF, "fox scarf", "fox scarves" },
	{ ITEM_FUR_HAT, "fur hat", "fur hats"},
	{ ITEM_FUR_CLOAK, "fur cloak", "fur cloaks" },
	{ ITEM_LEATHER_GLOVES, "leather gloves", "leather gloves" },
	{ ITEM_UNKNOWN_ITEM3, "polished emerald", "polished emeralds" },
	{ ITEM_POLISHED_SAPPHIRE, "polished sapphire", "polished sapphires" },
	{ ITEM_POLISHED_RUBY, "polished ruby", "polished rubies" },
	{ ITEM_LEATHER_TORSO, "leather torso", "leather torsos" },
	{ ITEM_IRON_CHAINMAIL, "iron chainmail", "iron chainmails" },
	{ ITEM_LEATHER, "leather", "leather" },
	{ ITEM_LEATHER_BOOTS, "leather boots", "leather boots" },
	{ ITEM_UNICORN_MEDALLION, "unicorn med", "unicorn meds" },
	{ ITEM_SUN_MEDALLION, "sun med", "sun meds" },
	{ ITEM_MOON_MEDALLION, "moon med", "moon meds" },
	{ ITEM_STARS_MEDALLION, "stars med", "star meds" },
	{ ITEM_SILVER_RING, "silver ring", "silver rings" },
	{ ITEM_GOLD_RING, "gold rings", "gold rings" },
	{ ITEM_PLATINUM_COINS, "platinum coin", "platinum coins" },
	{ ITEM_IRON_SWORD, "iron sword", "iron swords" },
	{ ITEM_IRON_BROAD_SWORD, "iron broad sword", "iron broad swords" },
	{ ITEM_STEEL_LONG_SWORD, "steel long sword", "steel long swords" },
	{ ITEM_TWO_EDGED_STEEL_SWORD, "two edged steel sword", "two edged steel swords" },
	{ ITEM_TITANIUM_STEEL_SHORT_SWORD, "titanium steel short sword", "titanium steel short swords" },
	{ ITEM_TITANIUM_STEEL_LONG_SWORD, "titanium steel long sword", "titanium steel long swors" },
	{ ITEM_TITANIUM_STEEL_SERPENT_SWORD, "titanium steel serpent sword", "titanium steel serpent swords" },
	{ ITEM_PICKAXE, "pickaxe", "pickaxes" },
	{ ITEM_HAMMER, "hammer", "hammers" },
	{ ITEM_WOODEN_BATTLE_HAMMER, "wooden battle hammer", "wooden battle hammers" },
	{ ITEM_WOODEN_STAFF, "wooden staff", "wooden staffs" },
	{ ITEM_QUARTER_STAFF, "quarter staff", "quarter staffs" },
	{ ITEM_UNKNOWN_ITEM7, "UNKNOWN_ITEM7", "UNKNOWN_ITEM7" },
	{ ITEM_UNKNOWN_ITEM8, "UNKNOWN_ITEM8", "UNKNOWN_ITEM8" },
	{ ITEM_SILVER_BAR, "silver bar", "silver bars" },
	{ ITEM_GOLD_BAR, "gold bar", "gold bars" },
	{ ITEM_IRON_BAR, "iron bar", "iron bars" },
	{ ITEM_STEEL_BAR, "steel bar", "steel bars" },
	{ ITEM_TITANIUM_BAR, "titanium bar", "titanium bars" },
	{ ITEM_TITANIUM_ORE, "titanium Ore", "titanium Ore" },
	{ ITEM_WOODEN_SHIELD, "wooden shield", "wooden shields" },
	{ ITEM_ENHANCED_WOODEN_SHIELD, "enhanced wooden shield", "enhanced wooden shields" },
	{ ITEM_IRON_SHIELD, "iron shield", "iron shields" },
	{ ITEM_STEEL_SHIELD, "steel shield", "steel shield" },
	{ ITEM_FIGHTER_CLOAK, "excavator cloak", "excavator cloaks" },
	{ ITEM_UNKNOWN_CLOAK2, "moderator cloak", "moderator cloaks" },
	{ ITEM_UNKNOWN_CLOAK3, "conjuror cloak", "conjuror cloaks" },
	{ ITEM_UNKNOWN_CLOAK4, "UNKNOWN_CLOAK4", "UNKNOWN_CLOAK4" },
	{ ITEM_HARVESTER_CLOAK, "monster magnetism cloak", "monster magnetism cloaks" },
	{ ITEM_UNKNOWN_CLOAK6, "body piercing cloak", "body piercing cloaks" },
	{ ITEM_POTION_MAKER_CLOAK, "fast regeneration cloak", "fast regeneration cloaks" },
	{ ITEM_UNKNOWN_CLOAK8, "powersaving cloak", "powersaving cloaks" },
	{ ITEM_DOOM_CLOAK, "mirror skin cloak", "mirror skin cloak" },
	{ ITEM_UNKNOWN_CLOAK10, "UNKNOWN_CLOAK10", "UNKNOWN_CLOAK10" },
	{ ITEM_IRON_BATTLE_HAMMER, "iron battle hammer", "iron battle hammers" },
	{ ITEM_IRON_HELMET, "iron helmet", "iron helmets" },
	{ ITEM_LEATHER_PANTS, "leather pants", "leather pants" },
	{ ITEM_STEEL_CHAINMAIL, "steel chainmail", "steel chainmails" },
	{ ITEM_TITANIUM_CHAINMAIL, "titanium chainmail", "titanium chainmails" },
	{ ITEM_UNKNOWN_ITEM9, "wood branches", "wood branches" },
	{ ITEM_UNKNOWN_RING1, "UNKNOWN_RING1", "UNKNOWN_RING1" },
	{ ITEM_RING_OF_DAMAGE, "damage ring", "damage rings" },
	{ ITEM_UNKNOWN_RING3, "UNKNOWN_RING2", "UNKNOWN_RING3" },
	{ ITEM_RAT_TAIL, "rat tail", "rat tails" },
	{ ITEM_BEAVER_FUR, "beaver fur", "beaver furs" },
	{ ITEM_UNKNOWN_ITEM10, "UNKNOWN_ITEM10", "UNKNOWN_ITEM10" },
	{ ITEM_UNKNOWN_ITEM11, "UNKNOWN_ITEM11", "UNKNOWN_ITEM11" },
	{ ITEM_UNKNOWN_ITEM12, "UNKNOWN_ITEM12", "UNKNOWN_ITEM12" },
	{ ITEM_UNKNOWN_ITEM13, "UNKNOWN_ITEM13", "UNKNOWN_ITEM13" },
	{ ITEM_UNKNOWN_ITEM14, "UNKNOWN_ITEM14", "UNKNOWN_ITEM14" },
	{ ITEM_UNKNOWN_ITEM15, "UNKNOWN_ITEM15", "UNKNOWN_ITEM15" },
	{ ITEM_POTION_OF_FEASTING, "potion of feasting", "potions of feasting" },
	{ ITEM_UNKNOWN_ITEM17, "UNKNOWN_ITEM17", "UNKNOWN_ITEM17" },
	{ ITEM_UNKNOWN_ITEM18, "UNKNOWN_ITEM18", "UNKNOWN_ITEM18" },
	{ ITEM_POTION_OF_BODY_RESTORATION, "potion of body restoration", "potions of body restoration" },
	{ ITEM_POTION_OF_REASONING, "potion of reasoning", "potions of reasoning" },
	{ ITEM_UNKNOWN_ITEM21, "UNKNOWN_ITEM21", "UNKNOWN_ITEM21" },
	{ ITEM_RAW_DIAMOND, "raw diamond", "raw diamonds" },
	{ ITEM_UNKNOWN_RING4, "UNKNOWN_RING4", "UNKNOWN_RING4" },
	{ ITEM_UNKNOWN_RING5, "UNKNOWN_RING5", "UNKNOWN_RING5" },
	{ ITEM_UNKNOWN_RING6, "UNKNOWN_RING6", "UNKNOWN_RING6" },
	{ ITEM_UNKNOWN_RING7, "UNKNOWN_RING7", "UNKNOWN_RING7" },
	{ ITEM_DISENGAGEMENT_RING, "disengagement ring", "disengagement rings" },
	{ ITEM_UNKNOWN_RING9, "UNKNOWN_RING9", "UNKNOWN_RING9" },
	{ ITEM_UNKNOWN_RING10, "UNKNOWN_RING10", "UNKNOWN_RING10" },
	{ ITEM_UNKNOWN_RING11, "UNKNOWN_RING11", "UNKNOWN_RING11" },
	{ ITEM_UNKNOWN_MEDALLION1, "gold medallion", "gold medallions" },
	{ ITEM_UNKNOWN_MEDALLION2, "silver medallion", "silver medallions" },
	{ ITEM_HAMMER_AND_CHISEL, "hammer and chisel", "hammers and chisels" },
	{ ITEM_SANDING_PAPER, "sanding paper", "sanding papers" },
	{ ITEM_UNKNOWN_ITEM24, "UNKNOWN_ITEM24", "UNKNOWN_ITEM24" },
	{ ITEM_UNKNOWN_ITEM25, "UNKNOWN_ITEM25", "UNKNOWN_ITEM25" },
	{ ITEM_UNKNOWN_ITEM26, "UNKNOWN_ITEM26", "UNKNOWN_ITEM26" },
    { ITEM_UNKNOWN_ITEM27, "UNKNOWN_ITEM27", "UNKNOWN_ITEM27" },
    { ITEM_IRON_AXE, "iron axe", "iron axes" },
    { ITEM_STEEL_AXE, "steel axe", "steel axes" },
    { ITEM_TIT_AXE, "titanium axe", "titanium axes" },
    { ITEM_IRON_PLATE_MAIL, "iron plate mail", "iron plate mail" },
    { ITEM_IRON_CUISSES, "iron cuisses", "iron cuisses" },
    { ITEM_IRON_GREAVE, "iron greaves", "iron greaves" },
    { ITEM_LEATHER_HELM, "leather helm", "leather helms" },
    { ITEM_UNKNOWN_ITEM35, "UNKNOWN_ITEM35", "UNKNOWN_ITEM35" },
    { ITEM_UNKNOWN_ITEM36, "UNKNOWN_ITEM36", "UNKNOWN_ITEM36" },
    { ITEM_UNKNOWN_ITEM37, "UNKNOWN_ITEM37", "UNKNOWN_ITEM37" },
    { ITEM_UNKNOWN_ITEM38, "UNKNOWN_ITEM38", "UNKNOWN_ITEM38" },
    { ITEM_UNKNOWN_ITEM39, "UNKNOWN_ITEM39", "UNKNOWN_ITEM39" },
    { ITEM_UNKNOWN_ITEM40, "UNKNOWN_ITEM40", "UNKNOWN_ITEM40" },
    { ITEM_UNKNOWN_ITEM41, "UNKNOWN_ITEM41", "UNKNOWN_ITEM41" },
    { ITEM_UNKNOWN_ITEM42, "UNKNOWN_ITEM42", "UNKNOWN_ITEM42" },
    { ITEM_UNKNOWN_ITEM43, "UNKNOWN_ITEM43", "UNKNOWN_ITEM43" },
    { ITEM_UNKNOWN_ITEM44, "UNKNOWN_ITEM44", "UNKNOWN_ITEM44" },
    { ITEM_UNKNOWN_ITEM45, "UNKNOWN_ITEM45", "UNKNOWN_ITEM45" },
    
    { ITEM_UNKNOWN, NULL, NULL }
};

char *get_item_name(int id, int plural)
{
	int i;
	
	for (i = 0; item_name_list[i].id != ITEM_UNKNOWN; i++) {
		if (item_name_list[i].id == id) {
			return plural ? item_name_list[i].name_plural : item_name_list[i].name;
		}
	}
	
	return NULL;
}

int get_item_id(char *name)
{
	int i;
	
	for (i = 0; item_name_list[i].id != ITEM_UNKNOWN; i++) {
		if (!strcasecmp(item_name_list[i].name, name) || !strcasecmp(item_name_list[i].name_plural, name)) {
			return item_name_list[i].id;
		}
	}
	
	return ITEM_UNKNOWN;
}

void get_inventory_from_server(Uint8 *data)
{
	int i, pos, total_inv = data[0];
	
	for (i = 0; i < 36+8; i++) {
		inv[i].quantity = 0;
	}
	
	for(i = 0; i < total_inv; i++) {
		pos = data[i*8+7];
		inv[pos].id = *((Uint16 *)(data+i*8+1));
		inv[pos].quantity = *((Uint32 *)(data+i*8+3));
	}
	parse_items();        // Find out our equipment
}

void get_new_inventory_item_from_server(Uint8 *data)
{
	int pos;
		
	pos = data[6];
	inv[pos].id = *((Uint16 *)(data));
	inv[pos].quantity = *((Uint32 *)(data+2));
    equip_item(pos);  // Check if we can equip it
//	if (!trade.verified) {
//		check_inventory();
//	}
}

void remove_inventory_item(int pos)
{
	inv[pos].quantity = 0;
	
//	if (!trade.verified) {
//		check_inventory();
//	}
}


/******************************************/
/*-------------Bag Handling---------------*/
/******************************************/

struct BAG bags_list[200];
ITEM bag[50];
Uint16 no_bags = 0;



// First function using SDL functionality
void add_bag_list(Uint8 *data)
{
     int i, o, id;
     
     no_bags = data[0];
     for(i = 0; i < no_bags; i++) {
           o = i * 5 + 1;
           id = *((Uint8 *) (data + 4 + o));  
           if(id >= 200) continue;       
           bags_list[id].x = SDL_SwapLE16(*((Uint16 *) (data + o)));
           bags_list[id].y = SDL_SwapLE16(*((Uint16 *) (data + 2 + o)));
     }
}

void remove_item_from_ground(Uint8 pos)
{
	bag[pos].quantity = 0;
}

void remove_bag(Uint16 bag)
{
     // Make it inaccessible
     bags_list[bag].x = -1;
     bags_list[bag].y = -1;
}

void open_bag(Uint16 bag)
{
	Uint8 str[4];

    str[0] = INSPECT_BAG;
    str[1] = bag;
    send_to_server(str,2);
    return;
}

void get_bag_item(Uint8 *data)
{
	int	pos;
	
	pos = data[6];
	bag[pos].id = SDL_SwapLE16(*((Uint16 *) (data)));
	bag[pos].quantity = SDL_SwapLE32(*((Uint32 *) (data + 2)));
}

void get_bags_items_list(Uint8 *data)
{
     int i, j, k;
     int pos, no_items, o;
     
     for(i = 0; i < 50; i++) bag[i].quantity = 0;
     
     no_items = data[0];
     for(i = 0; i < no_items; i++) {      
           o = i * 7 + 1;
           pos = data[o + 6];
           bag[pos].id = SDL_SwapLE16(*((Uint16 *) (data + o)));
           bag[pos].quantity = SDL_SwapLE16(*((Uint16 *) (data + 2 + o )));
     }
     // Now, react to it...
     k = 0;
     // If we can pick it up ...
     for(j = 0; j < 36; j++) {
          if(inv[j].quantity == 0)
          {
               // Pick up one item
               for(; k < 50; k++) {
                    if(bag[k].quantity > 0)
                    {
                         pick_item(k, bag[k].quantity);
                    }
               }
          }
     }
}

void pick_item(Uint16 pos, Uint16 quantity)
{
     Uint8 str[10];
     
     str[0] = PICK_UP_ITEM;
     str[1] = pos;
     *((Uint16 *) (str + 2)) = SDL_SwapLE16((Uint16) quantity);
	 send_to_server(str,4);
}

void put_bag_on_ground(int bag_x, int bag_y, int id)
{
     if(id >= 200) return;       
     bags_list[id].x = bag_x;
     bags_list[id].y = bag_y;
     no_bags++;
}
     
/******************************************/
/*-----------Equipment section------------*/
/******************************************/

// LISTS. To be configurable.
#define HEAD    0
#define CAPE    1
#define MED     2
#define TORSO   3
#define SHIELD  4
#define WEAPON  5
#define LEG     6
#define BOOT    7

Uint16 equipment[8] = {0,0,0,0,0,0,0,0};

Uint16 eqlist[8][10] = {
       {    ITEM_IRON_HELMET,
            ITEM_LEATHER_HELM,
            ITEM_FUR_HAT,
            0,0,0,0,0,0,0
       }, 
       {0,0,0,0,0,0,0,0,0,0}, 
       {    ITEM_STARS_MEDALLION,
       	    ITEM_SUN_MEDALLION,
	        ITEM_MOON_MEDALLION,
       	    ITEM_UNICORN_MEDALLION,
            0,0,0,0,0,0
       }, 
       {    ITEM_IRON_PLATE_MAIL,
            ITEM_TITANIUM_CHAINMAIL,
            ITEM_STEEL_CHAINMAIL,
            ITEM_IRON_CHAINMAIL,
            ITEM_LEATHER_TORSO,
            0,0,0,0,0      // Filler
       }, 
       {    ITEM_STEEL_SHIELD,
            ITEM_IRON_SHIELD,
            ITEM_ENHANCED_WOODEN_SHIELD,
            ITEM_WOODEN_SHIELD,
            0,0,0,0,0,0},
       {    ITEM_TIT_AXE,
            ITEM_TITANIUM_STEEL_SERPENT_SWORD,
            ITEM_STEEL_AXE,
            ITEM_TITANIUM_STEEL_LONG_SWORD,
            ITEM_IRON_AXE,
            ITEM_TITANIUM_STEEL_SHORT_SWORD,
            ITEM_TWO_EDGED_STEEL_SWORD,
            ITEM_STEEL_LONG_SWORD,
            ITEM_IRON_BROAD_SWORD,
            ITEM_IRON_SWORD    
       }, 
       {    ITEM_LEATHER_PANTS,
            ITEM_IRON_CUISSES,
            0,0,0,0,0,0,0,0},
       {    ITEM_LEATHER_BOOTS,
            ITEM_IRON_GREAVE
            ,0,0,0,0,0,0,0,0}     
};

void parse_items()
{
     int i, j, k;
     
     for(j = 0; j < 8; j++) {
          for(i = 36; i < 44; i++) {
                if(inv[i].quantity == 0) continue;
                for(k = 0; k < 10; k++) {
                     if(eqlist[j][k] == 0) continue;
                     if(inv[i].id == eqlist[j][k]) equipment[j] = i;
                }
          }
     }
}


void equip_item(Uint8 pos)
{
     int i, j, curpos, invpos;
     Uint8 str[8];
     
     for(i = 0; i < 8; i++) {
           for(j = 0; j < 10; j++) {
                 if(eqlist[i][j] == 0) continue;
                 if(eqlist[i][j] == inv[pos].id)
                 {
                      if(equipment[i] != 0)
                      {
                            // Is it better ?
                            for(curpos = 0; curpos < 10; curpos++) {
                                  if(eqlist[i][curpos] == 0) continue;
                                  if(eqlist[i][curpos] == inv[equipment[i]].id)
                                  {
                                       invpos = curpos;
                                       break;
                                  }
                            }
                      }
                      // Check ratings
                      if(j < invpos || equipment[i] == 0)
                      {
                           if(equipment[i] != 0)
                           {
                                 str[0] = MOVE_INVENTORY_ITEM;
			                     str[1] = equipment[i];
			                     // Find an empty space:
                                 for(j = 0; j < 36; j++) {
                                       if(inv[j].quantity == 0)
                                       {
                                             str[2] = j;
			                                 send_to_server(str,3);
                                       }
                                }
                           }
                           // Now, move the item to equip:
                           str[0] = MOVE_INVENTORY_ITEM;
			               str[1] = pos;
			               if(equipment[i] != 0)
                           {
                                 str[2] = equipment[i];
                           } 
                           else
                           {
                                 // Find an empty slot:
                                 for(j = 36; j < 44; j++) {
                                       if(inv[j].quantity == 0)
                                       {
                                            str[2] = j;
                                            equipment[i] = j;
                                       }
                                 }
                           }
                           send_to_server(str,3);
                           return;
                      }
                 }
           }
     }                    
}
