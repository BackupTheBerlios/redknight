#ifndef CONNECTION__H
#define CONNECTION__H

/* function to open and close connection to the game server */
int init_connection (char *hostname, short unsigned int port);
void exit_connection (unsigned int lvl);

/* check if there's a message from the server */
void get_server_message ();

/* send message str of len bytes to the game server */
void send_to_server (unsigned char *str, int len);

/* send raw text (local chat, or channel chat if first character is @) to 
 * server */
void send_raw_text (unsigned char *fmt, ...);

#endif /* def CONNECTION__H */
