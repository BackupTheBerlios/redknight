/***************************************************************************
 *   Copyright (C) 2004 See the "CONTRIBUTORS.txt" file                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef WINDOWS
#include <windows.h>
#endif

#include <stdarg.h>

#include "includes.h"
#include "pathfinder.h"

#include "connection.h"
#include "log.h"
#include "local.h"
#include "stories.h"
#include "items.h"

char * ROOTDIR="/usr/opt/redknight";    // Anyone actually want this?
char *fortune[]={"fortune", NULL};
char *quote[]={"quote", NULL};

/*********************************************/
/* TODO: FIX ALL THESE UGLY GLOBAL VARIABLES */
/*********************************************/


/* Version for the -v or --version option */
int version_major=0, version_minor=3, patch_version=9;

char my_name[40], my_password[40], my_guild[5], boss_name[40], quit_message[50], greet_message[50], enemy_guild[5], insult_message[50];
char pubgreet1[50], pubgreet2[50], pubgreet3[50];
int debug = -1;
int hail_master, hail_everyone, hail_guild, insult_enemy;
unsigned short int verbose = 0;     // Verbose mode 0|1

/* The connection: host name and port number */
char *hostname = "eternal-lands.solexine.fr";
int port = 2000;

/* Extern which must pass between all functions */
// char chat_name[40];
char fname[60] = "elbot.dat";
char inname[60];
long queue_len=0;
int last_attack;

/* Not sure where else to put this... */
char name[40], guild[5];

/* Function Prototypes */
void process_text_message(const char *msg, char *chat_name, int PM); 


void process_raw_text (const Uint8*data, int data_len) {
  int len;
  int len2;
  int PM=0;
  //char name[40];
  char chat_name[40];
  char *loc;
  char chat_seperator[5];
  
  strcpy (chat_seperator, ":");

  if (debug >= DEBUG_MEDIUM) log_info ("%s\n", data);
  if ((unsigned char) data[0] != 128 + c_red1) return;
  if (debug >= DEBUG_HIGH) log_info ("Looks like chat...\n");

  if (strncasecmp (data+1, "[PM from ", 9) == 0) {
  	PM=1;
    len = strlen (boss_name);
	loc = strstr(data, chat_seperator);

	if (debug >= DEBUG_HIGH) log_info ("I got a PM!\n");

  	/*len2 = loc-data- 10;
  	strncpy (chat_name, data+10, len2);
  	strcpy (chat_name+len2, "\0");*/
  	for (len2 = 0; len2 < 16 && data[len2+10] != ':'; len2++) {
			chat_name[len2] = data[len2+10];
		}
    chat_name[len2] = 0;
  	
  	if(!strcmp(my_name, chat_name))return; // Don't talk to myself
  	if (debug >= DEBUG_HIGH) log_info ("I've got %s\n", chat_name);
    
	if(PM!=0) process_text_message(data+10+len2+2, chat_name, 1);
  }
	if (data[0] == 136 && strstr(data+1, " wants to trade with you")) {
		char *actor_name;
		
		actor_name = strstr(data+1, " wants to trade with you");
		*actor_name = 0;
		actor_name = data+1;
		
		printf("Someone wants to trade with me!\n");
		
		trade_with(actor_name);
		return;
	}
}


//This functions figures out the text type, and responds accordingly
void process_text_message(const char *msg, char *chat_name, int PM) { 
    char text[1024];
    char buffer[1024];          // Misc. buffer
    char *data = malloc(256);
    int i=0;    

    if(PM==1)
    {
      // tolower' text
      while(i < strlen(msg)) {
          data[i] = tolower(msg[i]);
          i++;
      }
      data[i] = '\0';
      i = 0;
      
      if(debug>=2)log_info("PM: %s\n", data);
      
      // Commands are prefixed by $, admin commands have no prefix
      // Help Commands:
      if(data[0] == '$') {
           data++;  //Skip '$'    
           if((!strncmp(data, "help", 4))) {
                // Dump help info
                sprintf(text, "%s%s%s%s", "Help - Commands:\n",
                    "\"$Story\" -> List story channel, story, etc.\n",
                    "\"$Recite\" -> Read a random story (in ch. 144).\n",
                    "\"$Fortune\" -> Read a random quote.\n");
                send_pm_enhanced(text, chat_name);
                return;          
           }
          if((!strncmp(data, "admin_help", 10))) {
               // Dump admin help info
               if(get_string(&admin, chat_name, strlen(chat_name)) != -1) {
                    sprintf(text, "%s%s%s%s%s%s%s%s%s%s%s%s", "Admin - Commands:\n",
                         "\"!Echo <text>\" -> Send raw text <text>.\n",
                         "\"!Send <name> <text>\" -> PM <text> to <name>.\n",
                         "\"!Die -> Log off.\n",
                         "\"!gadd <name> -> Protect the guild <name>.\n",
                         "\"!gdel <name> -> Unprotect the guild <name>.\n",
                         "\"!glst -> List protected guilds.\n",
                         "\"!padd <name> -> Protect the player <name>.\n",
                         "\"!pdel <name> -> Unprotect the player <name>.\n",
                         "\"!plst -> List protected players.\n",
                         "\"!inv -> List inventory and statistics.\n",
                         "\"!equip -> List equipment.\n");
               }
               else strcpy(text, "R0fl, slave; U r not ~ admin!!`");
               send_pm_enhanced(text, chat_name);
               return;          
          }                     
      
          // Normal Commands :      
          if((!strncmp(data,"fortune", 7))) {
               get_quote("fortune.dat", chat_name);
               return;
          }
      
          if((!strncmp(data, "story", 5))) {
               // Dump story info
               strcpy(text, dump_story());     // A lot of the info is global, or 
                                               // shouldn't be...
               send_pm_enhanced(text, chat_name);
               return;
          }
          if((!strncmp(data, "recite", 6))) {
               random_story();
               return;
          }
      }
            
      // Admin Commands:
      // (Players won't know if they actidentally activate them)
      if(data[0] == '!') {
          data++;      // Skip '!'  
          if(get_string(&admin, chat_name, strlen(chat_name)) != -1) {                          
               if((!strncmp(data,"echo",4)) && data[4] != '\0' && data[5] != '\0' && data[6] != '\0') {
                   strncpy(text, data+5, (strlen(data+4))-1);
                   strcpy(text+(strlen(data+4)-2), "\0"); 
                   send_raw_text(text);
                   return;
               }      
               if((!strncmp(data,"send",4)) && data[4] != '\0' && data[5] != '\0' && data[6] != '\0') {
                   strncpy(text, data+5, (strlen(data+4))-1);
                   strcpy(text+(strlen(data+4)-2), "\0");
                   send_pm(text);
                   return;
               }                   
               if((!strncmp(data,"die",3))) {          
                   if (debug >= DEBUG_HIGH) log_info("The boss wants me to quit!\n");

                   send_pm ("%s Yes sir, %s\0", chat_name, chat_name);
                   send_raw_text ("%s\0", quit_message);
                   save_list(&A_player, "players.lst");
                   save_list(&A_guild, "guilds.lst");
                   exit_connection (EXIT_ALL);
                   exit (0);
                   return;
               }
               if((!strncmp(data,"save",4))) {          
                   send_pm ("%s Yes sir, %s\0", chat_name, chat_name);
                   save_list(&A_player, "players.lst");
                   save_list(&A_guild, "guilds.lst");
                   return;
               }
               if((!strncmp(data,"inv", 3))) {
                   dump_inv(chat_name);
                   return;
               }
               if((!strncmp(data,"equip", 5))) {
                   dump_equip(chat_name);
                   return;
               }           
               if((!strncmp(data,"padd",4))) {
                   for(i = 0; data[i+5] != ']'; i++) {
                         text[i] = data[i+5];
                   }
                   text[i] = '\0';
                   if(add_string(&A_player, text, strlen(text)) == 0)
                   {
                         send_pm("%s List is already full. Maybe you should remove some people?", chat_name);
                   }
                   return;
               }
               if((!strncmp(data,"pdel",4))) {
                   for(i = 0; data[i+5] != ']'; i++) {
                         text[i] = data[i+5];
                   }
                   text[i] = '\0';
                   if(remove_string(&A_player, text, strlen(text)) == -1)
                   {
                         send_pm("%s Name could not be removed. Maybe you should check your spelling?", chat_name);
                   }
                   return;
               }
               if((!strncmp(data,"plst",4))) {
                   send_pm("%s Players protected: %s", chat_name, print_list(&A_player, buffer));
                   return;
               }
               if((!strncmp(data,"gadd",4))) {
                   for(i = 0; data[i+5] != ']'; i++) {
                         text[i] = data[i+5];
                   }
                   text[i] = '\0';
                   if(add_string(&A_guild, text, strlen(text)) == 0)
                   {
                         send_pm("%s List is already full. Maybe you should remove some people?", chat_name);
                   }
                   return;
               }
               if((!strncmp(data,"gdel",4))) {
                   for(i = 0; data[i+5] != ']'; i++) {
                         text[i] = data[i+5];
                   }
                   text[i] = '\0';
                   if(remove_string(&A_guild, text, strlen(text)) == -1)
                   {
                         send_pm("%s Name could not be removed. Maybe you should check your spelling?", chat_name);
                   }
                   return;
               }
               if((!strncmp(data,"glst",4))) {
                   send_pm("%s Guilds protected: %s", chat_name, print_list(&A_guild, buffer));
                   return;
               }
           }          
      }               
      // Don't understand:
      send_pm("%s %s", chat_name, "Thanks, your message has been passed to guild chat. For more commands, use $help.");
      // Pass it to #gm, after trimming it
      for(i = 0; data[i] != ']'; i++) {
            text[i] = data[i];
      }
      text[i] = '\0';
      send_raw_text("#gm %s: %s", chat_name, text);
    }
    else
    {
        // Local Chat - TODO
        data++;  // Skip color
    } 
    free(data);       
}


// STATS STUFF
Sint16 food = 45;   // Default, so we don't worry
att16 human;
att16 magic;
att16 carry;
att16 matter;
att16 ethereal;

void proc_stats(Sint16 *stat)
{
     // Levels we need to know :
     human.c = stat[12];
     human.b = stat[13];          
     magic.c = stat[36];
     magic.b = stat[37];          
     carry.c = stat[40];
     carry.b = stat[41];
     matter.c = stat[42];
     matter.b = stat[43];
     ethereal.c = stat[44];
     ethereal.b = stat[45];
     food = stat[46];
}

void proc_stat(Uint8 stat, Sint32 value)
{
     switch(stat)
     {
   		case HUMAN_CUR:
			human.c = value;
            break;
		case HUMAN_BASE:
			human.b = value;
            break;
		case MAG_S_CUR:
			magic.c = value;
            break;
		case MAG_S_BASE:
			magic.b = value;
            break;
		case CARRY_WGHT_CUR:
			carry.c = value;
            break;
		case CARRY_WGHT_BASE:
			carry.b = value;
            break;
		case MAT_POINT_CUR:
			matter.c = value;
            break;
		case MAT_POINT_BASE:
			matter.b = value;
            break;
		case ETH_POINT_CUR:
			ethereal.c = value;
            break;
		case ETH_POINT_BASE:
			ethereal.b = value;
            break;
		case FOOD_LEV:
			food = value;
            break;
   }
}


struct TIMER *heartbeatptr, *storyptr, *guildmapptr, *pf_moveptr, *selfptr;

// Main Timer wrappers
Uint32 last_heart_beat;
Uint8 heartbeat = HEART_BEAT;

int heartbeat_timer(Uint32 time)
{
     send_to_server (&heartbeat, 1);
     last_heart_beat = time;
     return 1;
}

int story_timer(Uint32 time)
{
     if(story == -1 || story_time > time) return 1;
     read_story();
     return 1;
}

int guildmap_timer(Uint32 time)
{
     actor *me = NULL;
     int i;
             

     if(insult_enemy == 1 && bot_map.map[bot_map.cur_map].id == CONFIG_NULL)
     {
         if((me = pf_get_our_actor()) == NULL || me->fighting == 1) 
         {
              log_info("Cannot move, currently missing or fighting\n");
              return 1; // Can't do anything without it
         }
         // Are we near a bag ? 
         for(i = 0; i < no_bags; i++) {
              if(PF_DIFF(me->x_tile_pos, bags_list[i].x) < 3 && PF_DIFF(me->y_tile_pos, bags_list[i].y) < 3)
              {
                   open_bag(i);
                   return 1;
              }
         }
         if(first_node != NULL)
         {        
              if((first_node->time - 2000) <= time && !(first_node->k->fighting))
              {             
                   pseudo_pf_find_path(first_node->k->x_tile_pos, 
                                       first_node->k->y_tile_pos);
                                 
                   if((first_node->time) <= time) attack(first_node->k->actor_id);
              }
         } else {
              // Time to return to base...
              if((PF_DIFF(me->x_tile_pos, bot_map.map[bot_map.cur_map].x) >= 3 || PF_DIFF(me->y_tile_pos, bot_map.map[bot_map.cur_map].y) >= 3)
                  && pf_follow_path == 0)
              {
                   if(pseudo_pf_find_path(bot_map.map[bot_map.cur_map].x, bot_map.map[bot_map.cur_map].y) == 0) log_error("Cannot find valid path!\n");
              }
         }
     }
     return 1;
}

int self_timer(Uint32 time)
{
    int i;
    Uint8 str[10];
    // Check the food level
    if(food < 25)
    {
          for(i = 0; i < 36; i++) {
                if(inv[i].quantity > 0 && (inv[i].id == ITEM_FRUIT || inv[i].id == ITEM_VEGETABLES))
                {
                     str[0] = USE_INVENTORY_ITEM;
				     str[1] = i;
					 send_to_server(str,2);
					 break;
                }
          }
    }
    // Check health
    /*(matter.c < (matter.b/3))
    {
          // Cast restore - note this will be better supported later
          // when we'll check for essences, sigils, etc.
          str[0]=CAST_SPELL;
          str[1]=2;
          str[2]=1;
          str[3]=24;
          str[4]=0;
          
          send_raw_text("#gm I am below %d MP, so I am casting Restoration.", (matter.b/3));
          send_to_server(str, 4);
    }*/
    
    return 1;   
}
     
     

void do_event_loop () {
  Uint32 time;

  while (1) {
    get_server_message();
    
    // Normal Timers
    time = SDL_GetTicks();    
    timer_loop(time);

    SDL_Delay (100);
  }
}

int read_info (char *fname) {
  int intern_debug = DEBUG_NONE; 
    
  FILE *fp = fopen (fname, "r");

  if(!fp)
  {
      char error_to_log[200];
      log_info("%s %s\n", "No such file:", fname);
      return 0;
  }

  // TODO: RE-WRITE THIS TO USE A FUNCTION IN CONFIG.C
  // INIT_GLOBALS( .. ) ISN'T WHAT WE'RE LOOKING FOR
  int k;
  k = init_globals(fp, "login password server_port guild admin quit_message greet_message debug hail_master hail_everyone hail_guild insult_enemy enemy_guild insult_message public_greeting1 public_greeting2 public_greeting3", "ssissssiiiiisssss", my_name, my_password, &port, my_guild, boss_name, quit_message, greet_message, &intern_debug, &hail_master, &hail_everyone, &hail_guild, &insult_enemy, enemy_guild, insult_message, pubgreet1, pubgreet2, pubgreet3);

  if(debug == -1) debug = intern_debug;

  log_info("running in debug level: %d\n", debug);

  if (debug >= DEBUG_HIGH) {
  	log_info("geting data. errorlevel: %d\n", k);
  	log_info("My name is \"%s\"\n", my_name);
  	log_info("My password is \"%s\"\n", my_password);
 	log_info("My guild is \"%s\"\n", my_guild);
  	log_info("My boss is \"%s\"\n", boss_name);
  	log_info("I will say \"%s\" when I am told to quit\n", quit_message);

  	if (hail_master == 1) {
  		log_info("I will hail %s as Master\n", boss_name);
  	}
 	else {
  		log_info("I will not hail %s as Master\n", boss_name);
  	}

  	if (hail_everyone == 1) {
  		log_info("I will greet strangers with: \"%s\"\n", greet_message);
  	}
  	else {
  		log_info("I will not greet strangers.\n");
  	}
  	
	if (hail_guild == 1) {
		log_info("I will hail guild members.\n");
	}
	else {
		log_info("I will not hail guild members.\n");
	}
	log_info ("My enemy guild is %s.\n", enemy_guild);

	if (insult_enemy == 1) {
		log_info("I will insult members of my enemy guild by saying \"%s\".\n", insult_message);
	}
	else {
		log_info("I will not insult members of my enemy guild.\n");
	}
	log_info ("My greetings are: %s, %s and %s.\n", pubgreet1, pubgreet2, pubgreet3);		
  }
  return 1;
} 


int init_bot()
{  
#ifdef LINUX
  chdir(ROOTDIR);
#endif
  int err, i;

  // Init
  for(i=0;i<max_actors;i++)
			actors_list[i]=NULL;

  if (!read_info (fname)) {
    log_error ("Unable to read input file %s%s\n", ROOTDIR, "elbot.dat");
    exit (57);
  }
  // Load Configuration Lists
  load_list(&admin, "admin.lst");
  load_list(&A_player, "players.lst");
  load_list(&A_guild, "guilds.lst");
  
  // Load the map configuration
  load_map_config("map.ini");
  // Create the timers
  heartbeatptr = add_timer(25000, heartbeat_timer);
  storyptr = add_timer(1000, story_timer);
  guildmapptr = add_timer(2000, guildmap_timer);
  pf_moveptr = add_timer(2000, timed_pf_move);
  selfptr = add_timer(10000, self_timer);
  
  err = init_connection (hostname, port);
  if (err) {
    exit_connection (err);
    exit (2);
  }

  if (!login (my_name, my_password)) {
    log_error ("Unable to log in as %s\n", my_name);
    exit (35);
  }

  do_event_loop();

  exit_connection (EXIT_ALL);
  log_error ("Unable to log in as %s\n", my_name);
  
  return 0;
}


void getargs(int argc, const char ** argv)
{    
     for(*argv++; *argv != 0; *argv++) {
           if(*argv[0] == '-')
           { 
                 if(!strcmp(*argv, "-i")) // Set Input
                 {
                       strcpy(fname, *(++argv));
                       continue;
                 }
                 if(!strcmp(*argv, "-d")) // Debug
                 {
                       debug = atoi(*(++argv));
                       if(debug > 3) debug = 3;
                       if(debug < 0) debug = 0;
                       continue;
                 }
                 if(!strcmp(*argv, "-v")) // Verbose
                 {
                       verbose = 1;
                       continue;
                 }
                 if(!strcmp(*argv, "-f")) // Compile Fortune
                 {
                       txt2bin(*(++argv));
                       continue;
                 }
                 if(!strcmp(*argv, "-V") || !strcmp(*argv, "--version")) // Print Version Info
                 {
                       printf("Version: ELbot %d.%d-%d\n", version_major, version_minor, patch_version);
                       exit(0);
                 }
                 if(!strcmp(*argv, "-h") || !strcmp(*argv, "--help")) // Print Help Info
                 {
                       printf("Usage: ELbot [arguments]\n\n");
                       printf("  -d \t\t\tSet the debug level to the next argument.\n\t\t\t Possibilities are: 0 (None), 1, 2, 3(High).\n");
                       printf("  -i [filename]\t\tSet the input file to [filename]\n");
                       printf("  -h --help\t\tDisplay this information\n");
                       printf("  -f [filename]\t\tCompile the fortune file from [filename]\n");
                       printf("  -V --version\t\tDisplay version information\n");
                       printf("  -v \t\t\tDisplay the info to log whenever log_data()\n\t\t\t and/or log_error() are called.\n");
                       exit (0);
                 }
           } 
           else
           { 
                 printf("Unknown argument: %s\nUse redknight --help for more details", *argv);    // This shouldn't happen
                 exit(0);
           }
     }
     return;
}


#ifdef WINDOWS
int Main (int argc, const char ** argv) {
#else
int main (int argc, const char ** argv) {
#endif
  getargs(argc, argv);
  if(!init_bot()) return 0;
}

#ifdef WINDOWS
int APIENTRY WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{   
        return Main(_argc, (const char **)_argv);
}
#endif
