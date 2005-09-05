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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <SDL_net.h>
#include "actors.h"
#include "connection.h"
#include "config.h"
#include "log.h"
#include "elbot.h"
#include "client_serv.h"
#include "items.h"
#include "trade.h"

/* buffer that holds the message from the server */
unsigned char msgbuf[BUF_SIZE];
int buf_in_use;
unsigned char logged_in;

/* Structures that hold the connection */
SDLNet_SocketSet theset;
TCPsocket thesock;


/* initialize the connection to the game server. Return value is non-zero in
 * case of an error, this value can be given to exit_connection () to clean 
 * up */
int init_connection (char *hostname, short unsigned int port) {
  IPaddress ipad;

  /* initialize the SDLNet library */
  if (SDLNet_Init () == -1) {
    log_error ("Failed to initialize SDL_net: %s\n", SDLNet_GetError());
    return EXIT_SDL_QUIT;
  }
  if (debug >= DEBUG_HIGH) {
    log_info ("SDL network library succesfully initialized\n");
  }

  /* resolve server IP address */
  if (SDLNet_ResolveHost (&ipad, hostname, port) == -1) {
    log_error ("Cannot resolve host %s: %s\n", hostname,
              SDLNet_GetError());
    return EXIT_SDLNET_QUIT;
  }
  if (debug >= DEBUG_HIGH) {
  	log_info ("Resolved IP address: %u.%u.%u.%u\n", ipad.host & 0xff,
        	    (ipad.host >> 8) & 0xff, (ipad.host >> 16) & 0xff,
            	ipad.host >> 24);
  }
  
  /* try to connect to game server */
  thesock = SDLNet_TCP_Open (&ipad);
  if (thesock == NULL) {
    log_error ("Unable to connect to the game server: %s\n",
              SDLNet_GetError ());
    return EXIT_SDLNET_QUIT;
  }
  if (debug >= DEBUG_HIGH) {
  	log_info ("Connected to game server %s\n", hostname);
  }

  /* allocate a set for only one socket */
  theset = SDLNet_AllocSocketSet (1);
  if (theset == NULL) {
    log_error ("Cannot allocate socket set: %s\n", SDLNet_GetError ());
    return EXIT_CLOSE_SOCKET;
  }

  /* And add the socket */
  if (SDLNet_TCP_AddSocket (theset, thesock) == -1) {
    log_error ("Cannot add socket to set: %s\n", SDLNet_GetError ());
    return EXIT_FREE_SET;
  }
  if (debug >= DEBUG_HIGH) {
  	log_info ("Socket added to socket set\n");
  }

  buf_in_use = 0;

  return 0;
}

/* Close the connection to the game server, freeing used data structures. The 
 * input value lvl should be a return code from init_connection, or if 
 * init_connection was successfull EXIT_ALL */
void exit_connection (unsigned int lvl) {
  switch (lvl) {
    case EXIT_ALL:
    case EXIT_FREE_SET:
      SDLNet_FreeSocketSet (theset);
    case EXIT_CLOSE_SOCKET:
      log_info ("Closing down the connection, and exiting\n");
      SDLNet_TCP_Close (thesock);
    case EXIT_SDLNET_QUIT:
      SDLNet_Quit ();
    case EXIT_SDL_QUIT:
      SDL_Quit ();
  }
}

/* checks what kind of message was recieved */
int process_message (unsigned char* msg, int len) {
  switch (msg[0]) {
  // Actor Stuff
    case ADD_NEW_ENHANCED_ACTOR:                   
        check_actor_equip(msg+3); 
        break;      
    case ADD_ACTOR_COMMAND:
        process_command(*((short *)(msg+3)),msg[5]);
        break;
    case ADD_NEW_ACTOR:
        add_actor_from_server(msg+3, ((bot_map.map[bot_map.cur_map].id == CONFIG_NULL) ? 1 : 0));             // Kill monsters
        break;      
    case REMOVE_ACTOR:
		destroy_actor(*((Uint16 *)(msg+3)));
		break;
	case KILL_ALL_ACTORS:
		destroy_all_actors();
		break;  
    case YOU_ARE:
		yourself = *((short *)(msg+3));
	    break;
  // Inventory Stuff
   	case HERE_YOUR_INVENTORY:
		get_inventory_from_server(msg+3);
        break;
	case GET_NEW_INVENTORY_ITEM:
		get_new_inventory_item_from_server(msg+3);
	    break;
	case REMOVE_ITEM_FROM_INVENTORY:
		remove_inventory_item(msg[3]);
	    break;
  // Trade Stuff
  case GET_TRADE_ACCEPT:
		if (msg[3]) {
			trade.he_accepted = 1;
			accept_trade();
		} else {
			trade.we_accepted = 1;
		}
		break;
	case GET_TRADE_REJECT:
		if (msg[3]) {
			trade.he_accepted = 0;
		} else {
			trade.we_accepted = 0;
		}
		break;
	case GET_TRADE_EXIT:
		end_trade();
		check_inventory();
		break;
	case GET_TRADE_OBJECT:
		if (msg[10]) {
			get_trade_object(msg[9], *((Uint16 *)(msg+3)), *((Uint32 *)(msg+5)));
		}
		break;
	case REMOVE_TRADE_OBJECT:
		if (msg[6]) {
			remove_trade_object(msg[5], *((Uint16 *)(msg+3)));
		}
		break;
	case GET_YOUR_TRADEOBJECTS:
		get_inventory_from_server(msg+3);
		break;
	case GET_TRADE_PARTNER_NAME:
		msg[3+len] = 0;
		activate_trade(msg+3);
		break;
  // Bag Stuff
	case GET_NEW_BAG:
         put_bag_on_ground(SDL_SwapLE16(*((Uint16 *)(msg+3))), SDL_SwapLE16(*((Uint16 *)(msg+5))),*((Uint8 *)(msg+7)));
         break;
	case GET_BAGS_LIST:
         add_bag_list(&msg[3]);
		 break;
	case GET_NEW_GROUND_ITEM:
		 get_bag_item(msg+3);
		 break;
    case HERE_YOUR_GROUND_ITEMS:
		 get_bags_items_list(&msg[3]);
		 break;
    case CLOSE_BAG:
		 // Destroy Bag Contents
		 break;
    case REMOVE_ITEM_FROM_GROUND:
		 remove_item_from_ground(msg[3]);
		 break;
	case DESTROY_BAG:
		 remove_bag(msg[3]);
		 break;
 // Map Stuff  
   case CHANGE_MAP:		
      {
          bot_map.cur_map = -1;
          load_map(msg+3);
      }
      break;
 // Stat Stuff
   	case HERE_YOUR_STATS:
		proc_stats((Sint16 *)(msg+3));
		break;
    case SEND_PARTIAL_STAT:
		proc_stat(*((Uint8 *)(msg+3)),SDL_SwapLE32(*((Sint32 *)(msg+4))));
		break; 
 // Login stuff
    case LOG_IN_OK:
    case LOG_IN_NOT_OK:    
      logged_in = msg[0];
      break;
 // Text Stuff
    case RAW_TEXT:
      process_raw_text (msg+3, len-3);
      break;
    default:
      /* ignore */ ;
  }
  return 1;
}

/* see if there's a packet from the server, and if so, store it in the buffer. 
 * FIXME: this is taken from the original client code, which has been updated
 * in the meantime. Perhaps we should check it too. */
int get_packet () {
  int len, size, total;

  /* clear the buffer if this is a new packet */
  if (buf_in_use == 0)
    memset (msgbuf, 0, BUF_SIZE);

  /* get the header if necessary */
  if (buf_in_use < 3) {
    len = SDLNet_TCP_Recv (thesock, msgbuf+buf_in_use, 3-buf_in_use);
    if (len < 0) return 0;
    buf_in_use += len;
    if (buf_in_use < 3) {
      if (buf_in_use > 0) return -1;
      log_error ("Packet underrun, len = %d, buf_in_use = %d\n", len,
                 buf_in_use);
      return 0;
    }
  }

  size = (*((short *) (msgbuf+1)));
  size += 2;
  if (size >= BUF_SIZE-3) {
    /* uh oh */
    log_error ("Packet overrun ... data lost\n");
    return 0;
  }

  while (buf_in_use < size) {
    len = SDLNet_TCP_Recv (thesock, msgbuf+buf_in_use, size-buf_in_use);
    if (len <= 0) return -1;
    buf_in_use += len;
  }

  total = buf_in_use;
  buf_in_use = 0;
  return total;
}

/* check if there's a message from the server */
void get_server_message () {
  int nr_active_sock, reclen, startptr = 0, msglen;

try_again:
  nr_active_sock = SDLNet_CheckSockets (theset, 1);
  if (nr_active_sock == 0) return;
  if (!SDLNet_SocketReady (thesock)) return;

  reclen = get_packet ();
  if (reclen == -1) return;
  if (reclen <= 0) {
    log_error ("Disconnected by the server!\n");
    exit_connection (EXIT_ALL);
    // Call the main function again
    //init_bot();
    exit(0);
  }

  msglen = *((short *) (msgbuf + startptr + 1));
  msglen += 2;
  if (!process_message (msgbuf+startptr, msglen)) {
    /* something went wrong when processing our message. Log out. */
    exit_connection (EXIT_ALL);
    log_error ("Unable to process message");
    exit (1);
  }

  goto try_again;
}

void send_to_server (unsigned char *str, int len) {
  static int nerr = 0;
  unsigned char msg[1024];

  Uint8 c;

  if (len >= 1024-2) {
    log_error ("Cannot send message to server: too long\n");
    exit (9);
  }

  msg[0] = str[0];
  *((short *) (msg+1)) = (Uint16) len;
  memcpy (msg+3, str+1, len-1);

  if (SDLNet_TCP_Send (thesock, msg, len+2) < len+2) {
    log_error ("When sending message to server: %s\n",
              SDLNet_GetError ());
    if (++nerr > MAX_NERR) {
      log_error ("This has happened too many times, giving up...\n");
      exit (8);
    }
  }
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

/* send raw text (local chat, or channel chat if first character is @) to 
 * server */
void send_raw_text (unsigned char *fmt, ...) {
  unsigned char msg[256];
  va_list ap;

  memset (msg, 0, 256);
  msg[0] = RAW_TEXT;

  va_start (ap, fmt);
  vsnprintf (msg+1, 256-2, fmt, ap);
  send_to_server (msg, strlen(msg+1) + 2);
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
