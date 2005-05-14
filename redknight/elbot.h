#ifndef BOTDEFS__H
#define BOTDEFS__H

/* exit levels */
#define EXIT_SDL_QUIT            2
#define EXIT_SDLNET_QUIT         3
#define EXIT_CLOSE_SOCKET        4
#define EXIT_FREE_SET            5
#define EXIT_ALL                 100

#define MAX_NERR                 0
#define BUF_SIZE                 8192

extern char *ROOTDIR;

extern int debug;
extern int hail_master, hail_everyone, hail_guild, insult_enemy, debug;
extern char my_name[], my_password[], boss_name[], my_guild[], enemy_guild[], greet_message[],
            insult_message[], guild[], name[];
extern char *fortune[], *quote[];
/* check what sort of message comes in, and take action based on that */
int process_message (unsigned char* msg, int len);
//void send_hail (int hail_type);

/* The Debug Levels */
#define DEBUG_NONE        0
#define DEBUG_LOW         1
#define DEBUG_MEDIUM      2
#define DEBUG_HIGH        3 

typedef struct
{
	Sint16 b;
	Sint16 c;
} att16;

#endif /* def BOTDEFS__H */
