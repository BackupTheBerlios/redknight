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

char * ROOTDIR="/home/bremac/elbot/";

//Local Prototypes
char *fortune[]={"fortune", NULL};
char *quote[]={"quote", NULL};

char my_name[40], my_password[40], my_guild[5], boss_name[40], quit_message[50], greet_message[50], enemy_guild[5], insult_message[50];
char pubgreet1[50], pubgreet2[50], pubgreet3[50];
int debug, hail_master, hail_everyone, hail_guild, insult_enemy;

/* The connection: host name and port number */
char *hostname = "eternal-lands.solexine.fr";
int port = 2000;

/*Extern which must pass between all functions*/
char chat_name[40];
char fname[60] = "elbot.dat";
char inname[60];
long queue_len=0;
int last_attack;

/*Stories*/
#include "stories.h"

/*Not sure where else to put this...*/
char name[40], guild[5];
int yourself;
int _debug=0;

/*Version for the -v or --version option*/
int version_major=0, version_minor=5, patch_version=0;

void process_text_message(const char *data, int PM);
unsigned char logged_in;


// admin.lst usage
char admins[100*30];
int no_admins = 0;
// <name> <name> ...
//FIXME
void load_admins()
{
     FILE *f = NULL;
     char b;
     char buffer[30*100];
     int s = 0, c = 0, i = 0, d = 0;
     
	 if(!(f=open("admin.lst", 0))) return;
	 
	 
	 while((s=read(f, &b, 1))) admins[i++] = b;
     close(f);	 
}

int check_admins(char * name)
{
     int i = 0;
     
     while(i < no_admins) {
           printf("%s\n", admins[i]);  
           if(!strcasecmp(admins[i++], name)) return 1;
     }
     
     return 0;
}

// this function takes a single string. The first word of the string is the name of the person to send the pm to.

void send_pm (unsigned char *fmt, ...) {

  unsigned char msg[256];

  va_list ap;

  

  memset (msg, 0, 256);

  msg[0] = SEND_PM;

  

  va_start (ap, fmt);

  vsnprintf (msg+1, 256-2, fmt, ap);

  if (debug >=5) {

  	log_info ("trying to send: %s\n", msg);

  }

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

// this function takes x/y chords and tells the server to move you there.\

//Todo when adding actors/actor commands, keep track of our position

void send_move_cmd (int *X, int *Y) {

	unsigned char msg[5];



	memset (msg, 0, 5);

	msg[0] = MOVE_TO;



	*((short *)(msg+1)) = (short) *X;

	*((short *)(msg+3)) = (short) *Y;

	send_to_server(msg, 5);

}



void process_raw_text (const char *data) {

  int len;

  int len2;

  int PM=0;

  //char name[40];

  char *loc;

  char chat_seperator[5];

  strcpy (chat_seperator, ":");

  if (debug >= 2) {

  	log_info ("%s\n", data);

  }



  if ((unsigned char) data[0] != 128 + c_red1) return;

  if (debug >= 3) {

  	log_info ("Looks like chat...\n");

  }

  if (strncasecmp (data+1, "[PM from ", 9) == 0) {

  	PM=1;

    len = strlen (boss_name);

	loc = strstr(data, chat_seperator);

	if (debug >= 3) {

		log_info ("I got a PM!\n");

	}

  	len2 = loc-data - 10;

  	strncpy (chat_name, data+10, len2);

  	strcpy (chat_name+len2, "\0");
  	
  	if(!strcmp(my_name, chat_name))return; // Don't talk to myself

  	if (debug >= 3) {

		log_info ("I've got %s\n", chat_name);

  	}

  	if (strncasecmp (data+10, boss_name, len) == 0) {

		if (debug >= 3) {

  			log_info ("It's from the boss!\n");

		}

		if (strcasecmp (data+10+len, ": #die]") == 0) {

                        if (debug >= 3) {

                                log_info ("The boss wants me to quit!\n");

                        }

                        send_pm ("%s Yes sir, %s\0", chat_name, chat_name);

                        send_raw_text ("%s\0", quit_message);

                        exit_connection (EXIT_ALL);

                        exit (0);

                }

		if (strcasecmp (data+10+len, ": #move]") == 0) {

                if(pseudo_pf_find_path(50, 50)) {
                     return;
                }

        }
  	}

  	else {

		if (debug >= 3) {

  		log_info ("Not the boss\n");

		}

		// one of these days I want to make this message configurable without recompiling

		//send_pm ("%s I'm just a bot, and only %s can tell me what to do.\0", chat_name, boss_name);

	}

	//len=strlen(chat_name);

	if(PM!=0) process_text_message(data+10+len2+2,1);

	//else process_text_message(data+1,0);

  }

 }

 

//This functions figures out the text type, and responds accordingly

void process_text_message(const char *data, int PM) { 

    int x=0,y=0,z=0;

    char des_name[300];

    char text[300];

    /*//Get to the text

    int len;

    if((len=strlen(data))>40)len=40;

    if(PM==0)

    {

        for(x=0;x < len && data[x] != ':';x++) {

            if(x == len) return;//This was RPing...skip it

        }

    x+=4;

    }    

    const char * in_text=(data+x);*/ 

    if(debug>=3)log_info("PM: %s\n", data);

               

    int i=0;

    //TEMP

    if(PM==1)

    {

      if(!strncasecmp(data/*in_text*/,"describe",8) && data[8] != '\0' && data[9] != '\0' && data[10] != '\0')

      { 

        log_info("Describing...");    

        if(!strcasecmp(data/*in_text*/,"describe me]"))

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

            if(!strcasecmp(data/*in_text*/,"describe crusadingknight]"))

            {

                send_pm("%s %s", chat_name, "He is my lord and master.");

                return;

            }

            if(!strcasecmp(data/*in_text*/,"describe tirashazor]"))

            {

                send_pm("%s %s", chat_name, "He calls me a 'bith'!.");

                return;

            }

            //Don't know who to describe

            //Somehow adds some NULL variables? --FIXME

            strncpy(des_name,data+9,(strlen(data+9)-1));

            strcpy(des_name+(strlen(data+9)-1), "\0"); 

            send_pm("%s I don't know who \"%s\" is.", chat_name, des_name);

            return;

        }

      }

      if((!strncasecmp(data,"echo",4)) && data[4] != '\0' && data[5] != '\0' && data[6] != '\0') {

          strncpy(text, data+5, (strlen(data+4))-1);

          strcpy(text+(strlen(data+4)-2), "\0"); 

          send_raw_text(text);

          return;

      }
      
      if((!strncasecmp(data,"send",4)) && data[4] != '\0' && data[5] != '\0' && data[6] != '\0') {
          int i = 5, j = 0;
          char toname[30];

          while(data[i] != ' ' && data[i] != '\0') {
                toname[j++] = data[i++];
          }                                                                      
          toname[j] = '\0';
          i++;

          strncpy(text, data+i, (strlen(data+4))-1);

          strcat(text, "\0"); 

          send_pm("%s %s", toname, text);

          return;

      }
      

      if((!strncasecmp(data,"tell me a story", 15)) && story != -1) {

          send_pm("%s I'm already telling a story", chat_name);

          return;

      }

      if((!strncasecmp(data,"tell me a story", 15)) && story == -1) {

          send_pm("%s I Know: Harvy&Grim (A Tale in the Desert), Trik (The Battle at the well)", chat_name);

          return;

      }

      if((!strncasecmp(data,"tell me", 7)) && story == -1) {

          init_story(data+8);

          return;

      }
      
      if((!strncasecmp(data,"fortune", 7))) {

          get_quote("fortune.dat", chat_name);

          return;

      }

      //Shouldn't need this anymore

      if((!strncasecmp(data, "skip", 4))){

          story=14;

          return;

      }               



      //Don't understand

      //Somehow adds NULL some variables? --FIXME

      strncpy(text,data,(strlen(data)-1));

      strcpy(text+(strlen(data)-1), "\0");  

      send_pm("%s %s!?\0", chat_name,text);
      send_pm_enhanced("Help - Commands:\n\"Tell me a story\" -> List stories.\n\
\"Tell me <storyname>\" -> Tell story <storyname>.\n\"Describe <name>\" \
Describe character <name>, if I know them.\n\"Fortune\" -> Read a random quote.\n\
I'll be able to do more soon. :)", chat_name);

    }

    else

    {

        //in_text++;//Skip color character

        if(!strcasecmp((data+x)/*in_text*/,"describe me"))

        {

            if(!strcasecmp(chat_name,"crusadingknight"))

            {

                send_raw_text("%s %s%s", chat_name, "You are my lord and master, ", chat_name);

                return;

            }

            

            send_raw_text("%s %s%s", chat_name, "I don't know how you are, ", chat_name);

         }

        else

        {

            if(!strcasecmp((data+x)/*in_text*/,"describe crusadingknight"))

                send_raw_text("%s %s", chat_name, "He is my lord and master.");

        }

    return;

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
          cur_map = -5;
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

  if (debug >= 3) {

  	log_info ("Logging in as %s\n", name);

  }

  send_to_server (str, len);



  logged_in = 0;



  /* check if login was successfull */

  for (i = 0; i < 100; i++) {

    get_server_message ();

    if (logged_in == LOG_IN_OK) {

      if (debug >= 3) {

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



  while (1) {

    get_server_message ();
    
    time = SDL_GetTicks ();
    if (last_heart_beat + 25 * 1000 < time) {
      /* another 25 seconds have passed.
       * let the server know we're still there before the Grue eats us... */
      send_to_server (&heartbeat, 1);
      last_heart_beat = time;
    }

    timed_pf_move(time);

    if(insult_enemy == 1 && last_attack <= time && first_node)
    {
        last_attack = time+3000;
        if((first_node->time - 2000) <= time && !actors_list[first_node->k]->fighting)
        {
             int i = first_node->k;
             
             pseudo_pf_find_path(actors_list[i]->x_tile_pos, 
                                 actors_list[i]->y_tile_pos);
                                 
             if((first_node->time) <= time) {
                 attack(actors_list[i]->actor_id);
             }
        }  
    }

    //read?
    if(story != -1 && story_time <= time )read_story();    

    SDL_Delay (100);
  }
}





int read_info (char *fname) {

  FILE *fp = fopen (fname, "r");

  

  if(!fp)

  {

      char error_to_log[200];

      log_info("%s %s\n", "No such file:", fname);

      return 0;

  }

  

  int k;

  k = init_globals(fp, "login password guild admin quit_message greet_message debug hail_master hail_everyone hail_guild insult_enemy enemy_guild insult_message public_greeting1 public_greeting2 public_greeting3  ", "ssssssiiiiisssss", my_name, my_password, my_guild, boss_name, quit_message, greet_message, &debug, &hail_master, &hail_everyone, &hail_guild, &insult_enemy, enemy_guild, insult_message, pubgreet1, pubgreet2, pubgreet3);

 

  log_info("running in debug level: %d\n", debug);

  if (debug >= 3) {

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

  //Set the over-ridden debug level
  if(_debug)debug=_debug;
  if(debug >= 5) port=2001;

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
