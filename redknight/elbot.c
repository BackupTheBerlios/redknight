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

#include "includes.h"
#include <stdarg.h>
#include "connection.h"
#include "log.h"
#include "local.h"
#include "stories.h"

char * ROOTDIR="/home/bremac/elbot/";    // Anyone actually want this?
char *fortune[]={"fortune", NULL};
char *quote[]={"quote", NULL};

/* Version for the -v or --version option */
int version_major=0, version_minor=3, patch_version=9;

char my_name[40], my_password[40], my_guild[5], boss_name[40], quit_message[50], greet_message[50], enemy_guild[5], insult_message[50];
char pubgreet1[50], pubgreet2[50], pubgreet3[50];
int debug = -1;
int hail_master, hail_everyone, hail_guild, insult_enemy;

/* The connection: host name and port number */
char *hostname = "eternal-lands.solexine.fr";
int port = 2000;

/* Extern which must pass between all functions */
char chat_name[40];
char fname[60] = "elbot.dat";
char inname[60];
long queue_len=0;
int last_attack;

/* Not sure where else to put this... */
char name[40], guild[5];
int yourself;

/* Function Prototypes */
void process_text_message(const char *data, int PM);
unsigned char logged_in;



// admin.lst usage
char admins[5*30];
int admins_len = 0;
// " <name> <name> ..."
// (A space is appended to the front to make searches faster)
// (tolower'd because case is insensitive, ie. CrusadingKNIGHT == CrUsAdInGkNiGhT)
void load_admins()
{
     FILE *f = NULL;
     char b;
     char buffer[30*100];
     int s = 0, c = 0, i = 0, d = 0;
     
	 if(!(f=open("admin.lst", 0))) return;	 
	 admins[i++] = ' ';
	 while((s=read(f, &b, 1))) admins[i++] = tolower(b);
	 admins[i] = '\0';
	 admins_len = i;
     close(f);
}

// Usage :
//         if(get_admins_string("CrusaDingKnIGHT", 15) != -1) printf("Pwn3d!1~!");
//         (All strings are tolower'd)         
int get_admins_string(char * in_name, int len)
{
     int i = 0, j = 0, k = 0;
     if(len >= 30) return; //Too big to be a name
     char name[31]=" ";
     char lower_name[30];
     
     while(*in_name != '\0') {
          lower_name[i++] = tolower(*in_name);
          in_name++;
     }
     i = 0;          
     strcat(name, tolower(lower_name));
     
     while(i < admins_len && admins[i] != '\0') {   
          if(admins[i] == name[0]) {
                j = i;
                k = 0;
                while(j < admins_len && k < len && admins[j] == name[k]) {
                     j++;
                     k++;
                }
                if(k == len) return i;
          }
          i++;
     }
     log_info("Couldn't Process %s, len %d", name, len);
     return -1;
}

// this function takes a single string. The first word of the string is the name of the person to send the pm to.
void send_pm (unsigned char *fmt, ...) {
  unsigned char msg[256];
  va_list ap;
  
  memset (msg, 0, 256);
  msg[0] = SEND_PM;

  va_start (ap, fmt);
  vsnprintf (msg+1, 256-2, fmt, ap);

  if (debug >= DEBUG_HIGH) log_info ("trying to send: %s\n", msg);
  send_to_server (msg, strlen(msg+1) + 2);
}

// For taking a string and a name, and sending as necessary
// Special is that it allows formatting
void send_pm_enhanced(unsigned char *buffer, unsigned char *name)
{
     char to_send[256];
     unsigned short j = 0, k = 0, i = 0;
     short c = -1;
     
     //K Check is to prevent a bizarre hang.
     while(buffer[i] != '\0' && k++ < 256) {
           j = 0;
           for(i = c + 1; buffer[i] != '\n' && buffer[i] != '\0'; i++)
                 to_send[j++] = buffer[i];
           if(j > 0)
           {
                to_send[j] = '\0';
                send_pm("%s %s", name, to_send);
           }     
           c = i;
     }
}


void process_raw_text (const char *data) {
  int len;
  int len2;
  int PM=0;
  //char name[40];
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

  	len2 = loc-data - 10;
  	strncpy (chat_name, data+10, len2);
  	strcpy (chat_name+len2, "\0");
  	
  	if(!strcmp(my_name, chat_name))return; // Don't talk to myself
  	if (debug >= DEBUG_HIGH) log_info ("I've got %s\n", chat_name);

	if(PM!=0) process_text_message(data+10+len2+2,1);
  }
}

// Old Describe section, kinda useless though...
      /*  //Unless someone wants to add local support ... 
          //FIXME -- Load describe me's/playername from file
      
      if(!strncasecmp(data,"describe",8) && data[8] != '\0' && data[9] != '\0' && data[10] != '\0')
      { 
        if(debug >= DEBUG_HIGH)log_info("Describing...");    

        if(!strcasecmp(data,"describe me]"))
        {
            if(!strcasecmp(chat_name,"crusadingknight"))
            {
                send_pm("%s %s%s", chat_name, "You are my lord and master, ", chat_name);
                return;
            }

            if(!strcasecmp(chat_name,"tirashazor"))
            {
                send_pm("%s %s%s %s", chat_name, "I won't describe you, ", chat_name, "bith :P");
                return;
            }

            //Don't know player
            send_pm("%s %s%s", chat_name, "I don't know how you are, ", chat_name);
            return;
        }
        else
        {
            if(!strcasecmp(data,"describe crusadingknight]"))

            {
                send_pm("%s %s", chat_name, "He is my lord and master.");
                return;
            }
            if(!strcasecmp(data,"describe tirashazor]"))
            {
                send_pm("%s %s", chat_name, "He calls me a 'bith'!.");
                return;
            }

            strncpy(des_name,data+9,(strlen(data+9)-1));
            strcpy(des_name+(strlen(data+9)-1), "\0"); 
            send_pm("%s I don't know who \"%s\" is.", chat_name, des_name);
            return;
        }
      }
      
      // End Commented-Out describe section
*/


//This functions figures out the text type, and responds accordingly
void process_text_message(const char *data, int PM) { 
    int x=0,y=0,z=0;
    char des_name[300];
    char text[1024];
    int i=0;    

    if(PM==1)
    {
      // tolower' text
      while(i < strlen(data)) {
          data[i] = tolower(data[i]);
          i++;
      }
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
               if(get_admins_string(chat_name, strlen(chat_name)) != -1) {
                    sprintf(text, "%s%s%s%s", "Admin - Commands:\n",
                         "\"!Echo <text>\" -> Send raw text <text>.\n",
                         "\"!Send <name> <text>\" -> PM <text> to <name>.\n",
                         "\"!Die -> Log off.\n");
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
          if(get_admins_string(chat_name, strlen(chat_name)) != -1) {                          
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
                   exit_connection (EXIT_ALL);
                   exit (0);
                   return;
               }
          }          
      }               
      // Don't understand:
      strncpy(text,data,(strlen(data)-1));
      strcpy(text+(strlen(data)-1), "\0");

      send_pm("%s %s!?\0", chat_name, text);
      send_pm("%s %s", chat_name, "PM me $help for help");
    }
    else
    {
        // Local Chat - TODO
    }        
}

 
// checks what kind of message was recieved...
int process_message (unsigned char* msg, int len) {
  switch (msg[0]) {
    case ADD_NEW_ENHANCED_ACTOR:
      {                      
          check_actor_equip(msg+3);
      }    
      break;      
    case ADD_ACTOR_COMMAND:
         process_command(*((short *)(msg+3)),msg[5]);
      break;
    case ADD_NEW_ACTOR:
      {
            add_actor_from_server(msg+3, ((cur_map == 1337) ? 1 : 0));             // Kill monsters
      }

      break;      
    case REMOVE_ACTOR:
		{
			destroy_actor(*((short *)(msg+3)));
		}
		break;
	case KILL_ALL_ACTORS:
		{
			destroy_all_actors();
		}
		break;  
    case YOU_ARE:
      {
		yourself=*((short *)(msg+3));
	  }
	  break;  
   case CHANGE_MAP:		
      {
          cur_map = -1;
          cur_map = load_map(msg+3);          
      }
      break;
    case LOG_IN_OK:
    case LOG_IN_NOT_OK:    
      logged_in = msg[0];
      break;
    case RAW_TEXT:
      process_raw_text (msg+3);
      break;
    default:
      /* ignore */ ;
  }
  return 1;
}


int login (const char *name, const char *passwd) {
  unsigned char str[40];
  int i, len = strlen (name) + strlen (passwd) + 3;

  if (len >= 40-2) {
    log_error ("Name or password too long!\n");
    return 0;
  }

  str[0] = LOG_IN;
  sprintf ((char *) (str+1), "%s %s", name, passwd);
  if (debug >= DEBUG_HIGH) {
  	log_info ("Logging in as %s\n", name);
  }

  send_to_server (str, len);
  logged_in = 0;

  /* check if login was successfull */
  for (i = 0; i < 100; i++) {
    get_server_message ();
    if (logged_in == LOG_IN_OK) {
      if (debug >= DEBUG_HIGH) {
      	log_info ("Login was successful\n");
      }
      return 1;
    } else if (logged_in == LOG_IN_NOT_OK) {
      log_error ("Unable to log in, wrong password?\n");
      return 0;
    }
    /* we don't know if the login was succesfull: wait 10 milliseconds, and try 
     * again */
    SDL_Delay (10);
  }

  log_error ("Unable to get confirmation if login was successful\n");
  return 0;
}


void do_event_loop () {
  unsigned int last_heart_beat = SDL_GetTicks (), time;
  unsigned char heartbeat = HEART_BEAT;
  int j=0;
  actor *me = NULL;

  while (1) {
    get_server_message ();
    
    // Normal Timers
    time = SDL_GetTicks ();
    if (last_heart_beat + 25 * 1000 < time) {
      /* another 25 seconds have passed.
       * let the server know we're still there before the Grue eats us... */
      send_to_server (&heartbeat, 1);
      last_heart_beat = time;
    }    
    //read?
    if(story != -1 && story_time <= time )read_story();

    // Action Timers
    // We rely on our actor existing so we can complete the following...
    timed_pf_move(time);
    
    if(insult_enemy == 1 && (last_attack <= time) && (first_node != NULL))
    {        
         last_attack = time+2500;          
         if((me=pf_get_our_actor()) != NULL) {
              if(me->fighting != 1) {           
                  if((first_node->time - 2000) <= time && !(first_node->k->fighting))
                  {             
                       pseudo_pf_find_path(first_node->k->x_tile_pos, 
                                           first_node->k->y_tile_pos);
                                 
                       if((first_node->time) <= time) attack(first_node->k->actor_id);
                  }
              }
         }
    }
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





#ifdef WINDOWS
int Main (int argc, const char ** argv) {
#else
int main (int argc, const char ** argv) {
#endif
  int err, i;
  
  // Init
  for(i=0;i<max_actors;i++)
			actors_list[i]=NULL;

  if(!process_args(argc, argv))return;   

#ifdef LINUX
  chdir(ROOTDIR);
#endif
  if (!read_info (fname)) {
    log_error ("Unable to read input file %s%s\n", ROOTDIR, "elbot.dat");
    exit (57);
  }
  
  load_admins();

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

#ifdef WINDOWS
int APIENTRY WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{   
        return Main(_argc, (const char **)_argv);
}
#endif
