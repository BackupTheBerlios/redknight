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
#include "connection.h"
#include "log.h"
#include "elbot.h"
#include "client_serv.h"

/* buffer that holds the message from the server */
unsigned char msgbuf[BUF_SIZE];
int buf_in_use;

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
    exit (1);
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

