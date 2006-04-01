#include "includes.h"
#include "local.h"

// Keep duplication down
void hail_guildmember(char *name, char *guild, int gender)
{
        if(gender==FEMALE)send_raw_text ("Hail milady %s of %s!\0", name, guild);
      	else if(gender==MALE)send_raw_text ("Hail m'lord %s of %s!\0", name, guild);
      	else send_raw_text ("Hail %s of %s!\0", name, guild);
}

/* FIXME: Needs cleanup */

// This reads the names of people that come into the bots view and responds.
int check_if_we_should_hail (const Uint8 *data, int gender) {
  int i, j, len = strlen (data);
  
  memset (name, 0, 40);
  memset (guild, 0, 5);
  for (i = 1; i < len; i++)
    /* All colours after the first character must be the guild. 
       (We can skip char 1). */
    if ((unsigned char) data[i] >= 127) break;
  if (i >= len)j= i > 40 ? 40-1 : i;
  else j= i > 40 ? 40-1 : i-1;
  strncpy (name, data, j);
  strcpy (name+j, "\0");
  if (i < len)strncpy (guild, data+i+1, 5-1);
  log_info ("I see: %s\n", name);
  // Test if they're a demi-god. We want to be safe...
  if(data[0] == (127+c_green3)) {
      send_raw_text("#help_me %s is using demigod in my cave - this must be an abuse of power to bypass me!", name); 
      return 0;
  }
  if (strcasecmp (name, boss_name) == 0) {
  	if (hail_master == 1 || (hail_master == 2 && bot_map.map[bot_map.cur_map].id == CONFIG_NULL)) {
        hail_guildmember(name, guild, gender);
  		return 0;
   }
  }
  else {
  	if (strcasecmp(my_guild, guild)) { 
		if (hail_everyone == 1 || (bot_map.map[bot_map.cur_map].id == CONFIG_NULL && hail_everyone == 2)) {
  			if(get_string(&A_guild, guild, strlen(guild)) != -1)
            {
                 hail_guildmember(name, guild, gender);
                 return 0;
            }
  			if(get_string(&A_player, name, strlen(name)) != -1)
            {
                 hail_guildmember(name, guild, gender);
                 return 0;
            }  			
            send_raw_text("%s! %s", name, greet_message);  //Greeting, name, greeting message
  			return 1;          //Kill them
		}
	}
  }
  
  if (i < len) {
    /* player has a guild tag */
    strncpy (guild, data+i+1, 5-1);
    if (strcasecmp (my_guild, guild) == 0) {
      /* player has the desired guild tag */
      strncpy (name, data, i > 40 ? 40-1 : i-1);
      strcpy (name+(i > 40 ? 40-1 : i-1), "\0");
      if (hail_guild == 1 || (hail_guild == 2 && bot_map.map[bot_map.cur_map].id == CONFIG_NULL)) {
        hail_guildmember(name, guild, gender);
      	return 0;
      }
    }
   if (strcasecmp (enemy_guild, guild) == 0) {
   /* player has enemy guild tag */
   	strncpy (name, data, i > 40 ? 40-1 : i-1);
	if (insult_enemy == 1) {
		send_raw_text ("%s\0", insult_message);
	}
   }
  }
}
