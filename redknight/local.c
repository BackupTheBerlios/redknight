#include "local.h"
#include "includes.h"

// This reads the names of people that come into the bots view and responds.
int check_if_we_should_hail (const char *data, int gender) {
  int i, j, len = strlen (data);
  memset (name, 0, 40);
  memset (guild, 0, 5);
  for (i = 0; i < len; i++)
    /* 143 is the colour code for dark yellow, the colour in which the guild tag
     * is printed */
    if ((unsigned char) data[i] == 143) break;
  if (i >= len)j= i > 40 ? 40-1 : i;
  else j= i > 40 ? 40-1 : i-1;
  strncpy (name, data, j);
  strcpy (name+j, "\0");
  if (i < len)strncpy (guild, data+i+1, 5-1);
  log_info ("I see: %s\n", name);
  if (strcasecmp (name, boss_name) == 0) {
  	if (hail_master == 1) {
  	    if(gender==FEMALE)send_raw_text("Hail %s, my mistress!\0", name);
  		if(gender==MALE)send_raw_text("Hail %s, my master!\0", name);
  		else send_raw_text("Hail %s, my master!\0", name);
  		return 0;
   }
  }
  else {
  	if (strcasecmp(my_guild, guild)) { 
		if (hail_everyone == 1 || (cur_map == 1337 && hail_everyone == 2)) {
  			send_raw_text("%s! %s", name, greet_message);//Greeting, name, greeting message
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
      if (hail_guild == 1) {
      	if(gender==FEMALE)send_raw_text ("Hail milady %s of %s!\0", name, guild);
      	else if(gender==MALE)send_raw_text ("Hail m'lord %s of %s!\0", name, guild);
      	else send_raw_text ("Hail %s of %s!\0", name, guild);
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