#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#endif

#include "elbot.h"
#include "client_serv.h"
#include "actors.h"
#include "privilege.h"
#include "protocol.h"
#include "trade.h"
#include "credit.h"
#include "misc.h"
#include "config.h"

int quit = 0;
int cur_time = 0;
int disconnected = 1;
int logged_in = 0;

void init()
{
	int i;
	
	for (i = 0; i < 36+8; i++) {
		inv[i].quantity = 0;
	}
	
	trade.partner = NULL;
	trade.verified = 0;
	
	load_config();
}

void handle_command(char *user, char *command, char *args)
{
	int privilege;
	 
	if (!strcasecmp(user, cfg.admin)) {
		privilege = 5;
	} else {
		privilege = get_privilege_for_user(user);
	}

	// Handle trade commands
	if (command[0] != '#') {          
		handle_trade_command(user, command, args);
		return;
	}

	if (privilege < 1) {
		return;
	}
	
	// Level 1 commands
	
	
	if (privilege < 2) {
		return;
	}
	
	// Level 2 commands
	
	if (!strcasecmp(command, "#reload")) {
		load_config();
		send_pm("%s %s", user, "OK");
		return;
	}
	
	if (privilege < 3) {
		return;
	}
	
	// Level 3 commands
	
	if (privilege < 4) {
		return;
	}
	
	// Level 4 commands
	
	if (!strcasecmp(command, "#get_credits")) {
		int credits = get_credits_for_user(args);
		send_pm("%s %s has %d gold coin(s) in credit", user, args, credits);
		return;
	}
	
	if (!strcasecmp(command, "#get_priv")) {
		int priv = get_privilege_for_user(args);
		send_pm("%s %s's privilege level is %d", user, args, priv);
		return;
	}
	
	if (privilege < 5) {
		return;
	}
	
	// Level 5 commands
	
	if (!strcasecmp(command, "#set_credits") && args) {
		char *ptr = strchr(args, ' ');
		int credits;
		
		if (!ptr || ptr[1] < '0' || ptr[1] > '9') { return; }
		*ptr++ = 0;
		credits = atoi(ptr);
		
		set_credits_for_user(args, credits);
		
		send_pm("%s %s now has %d gold coin(s) in credit", user, args, credits);
		return;
	}
	
	if (!strcasecmp(command, "#add_credits") && args) {
		char *ptr = strchr(args, ' ');
		int credits;
		
		if (!ptr || ptr[1] < '0' || ptr[1] > '9') { return; }
		*ptr++ = 0;
		credits = atoi(ptr);
		
		add_credits_to_user(args, credits);
		
		send_pm("%s %s now has %d gold coin(s) in credit", user,
			args, get_credits_for_user(args));
		return;
	}
	
	if (!strcasecmp(command, "#rem_credits") && args) {
		char *ptr = strchr(args, ' ');
		int credits;
		
		if (!ptr || ptr[1] < '0' || ptr[1] > '9') { return; }
		*ptr++ = 0;
		credits = atoi(ptr);
		
		remove_credits_from_user(args, credits);
		
		send_pm("%s %s now has %d gold coin(s) in credit", user,
			args, get_credits_for_user(args));
		return;
	}
	
	if (!strcasecmp(command, "#set_priv") && args) {
		char *ptr = strchr(args, ' ');
		int priv;
		
		if (!ptr || ptr[1] < '0' || ptr[1] > '9') { return; }
		*ptr++ = 0;
		priv = atoi(ptr);
		
		set_privilege_for_user(args, priv);
		
		send_pm("%s %s's privilege level is now %d", user, args, priv);
		return;
	}
	
	if (!strcasecmp(command, "#say") && args) {
		send_raw_text(args);
		return;
	}
}

int main(int argc, char **argv)
{
	int last_heartbeat = 0, i = 0;
	fd_set fds;
	struct timeval tv;
		
	init();
	
	disconnected = 1;
	
	while (!quit) {
		if (disconnected) {
			init_connection(cfg.server_address, cfg.server_port);
			
			unsigned char str[256];

            int i, len = strlen(cfg.username) + strlen(cfg.password) + 3;

            if (len >= 40-2) {
                log_error ("Name or password too long!\n");
                return 0;
            }

            str[0] = LOG_IN;
            sprintf ((char *) (str+1), "%s %s", cfg.username, cfg.password);

            send_to_server (str, len);			
			disconnected = 0;
			
			FD_ZERO(&fds);
		}
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		
		cur_time = SDL_GetTicks();
		
		while(i++ < 100 && !logged_in) {
		    get_server_message();
		    SDL_Delay(10);
        }
				
		get_server_message();
		
		// send a heartbeat every 25 seconds
		if (cur_time-(25 * 1000) >= last_heartbeat) {
            Uint8 str[4]; 
            str[0] = HEART_BEAT;           
			send_to_server(str, 1);
			
			last_heartbeat = cur_time;
		}
				
		// limit the length of a trade
		if (cfg.max_trade_time > 0 && trade.active && cur_time-cfg.max_trade_time >= trade.time) {
			abort_trade();
			send_pm("%s %s", trade.partner, "Sorry, the trade was taking too long.");
		}
				
		SDL_Delay (100);
	}
		
	return 0;
}
