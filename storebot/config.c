#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elbot.h"
#include "items.h"
#include "misc.h"
#include "config.h"

CONFIG cfg;
char line[256];

void read_global_config(FILE *f)
{
	char *ptr;
	
	memset(&cfg, 0, sizeof(cfg));

	while (fgets(line, sizeof(line), f)) {
		if (!strncmp(line, "//", 2) || !strncmp(line, "#", 1)) {
			continue;
		}
		if (!strncasecmp(line, "</global>", 9)) {
			return;
		}
		if (strstr(line, "server address") && (ptr = strchr(line, '='))) {
			strcpy(cfg.server_address, trim(ptr+1));
			continue;
		}
		if (strstr(line, "server port") && (ptr = strchr(line, '='))) {
			cfg.server_port = atoi(trim(ptr+1));
			continue;
		}
		if (strstr(line, "username") && (ptr = strchr(line, '='))) {
			strcpy(cfg.username, trim(ptr+1));
			continue;
		}
		if (strstr(line, "password") && (ptr = strchr(line, '='))) {
			strcpy(cfg.password, trim(ptr+1));
			continue;
		}
		if (strstr(line, "guild") && (ptr = strchr(line, '='))) {
			strcpy(cfg.guild, trim(ptr+1));
			continue;
		}
		if (strstr(line, "admin") && (ptr = strchr(line, '='))) {
			strcpy(cfg.admin, trim(ptr+1));
			continue;
		}
		if (strstr(line, "use credit system") && (ptr = strchr(line, '='))) {
			ptr = trim(ptr+1);
			if (!strcasecmp(ptr, "YES")
					||
			    !strcasecmp(ptr, "TRUE")
			    	||
			    !strcmp(ptr, "1")) {
				cfg.use_credit_system = 1;   
			}
			continue;
		}
		if (strstr(line, "max trade time") && (ptr = strchr(line, '='))) {
			ptr = trim(ptr+1);
			cfg.max_trade_time = atoi(ptr);
			cfg.max_trade_time *= 1000;
			continue;
		}
	}
}

void read_sell_list(FILE *f)
{
	char *ptr;
	int i;
	
	// clear the sell list
	for (i = 0; i < MAX_TRADE_LIST_SIZE; i++) {
		cfg.sell[i].id = ITEM_UNKNOWN;
	}
	
	i = 0;
	
	while (fgets(line, sizeof(line), f)) {	
		if (!strncmp(line, "//", 2) || !strncmp(line, "#", 1)) {
			continue;
		}
		if (!strncasecmp(line, "</sell>", 7)) {
			return;
		}
		
		ptr = trim(strtok(line, ","));
		cfg.sell[i].id = get_item_id(ptr);
		
		if (cfg.sell[i].id == ITEM_UNKNOWN) {
			fprintf(stderr, "WARNING: \"%s\" is not a valid item name. Ignoring.\n", ptr);
			continue;
		}
		
		ptr = trim(strtok(NULL, ","));
		cfg.sell[i].price = atoi(ptr);
		
		ptr = trim(strtok(NULL, ","));
		cfg.sell[i].guild_price = atoi(ptr);
		
		if (++i >= MAX_TRADE_LIST_SIZE) {
			fprintf(stderr, "WARNING: Too many items in sell list (max=%d).\n", MAX_TRADE_LIST_SIZE);
			return;
		}
	}
}

void read_buy_list(FILE *f)
{
	char *ptr;
	int i;
	
	// clear the buy list
	for (i = 0; i < MAX_TRADE_LIST_SIZE; i++) {
		cfg.buy[i].id = ITEM_UNKNOWN;
	}
	
	i = 0;
	
	while (fgets(line, sizeof(line), f)) {
		if (!strncmp(line, "//", 2) || !strncmp(line, "#", 1)) {
			continue;
		}		if (++i > 35) {
			fprintf(stderr, "WARNING: Too many items in buy list.\n");
			return;
		}
		if (!strncasecmp(line, "</buy>", 6)) {
			return;
		}
		
		ptr = trim(strtok(line, ","));
		cfg.buy[i].id = get_item_id(ptr);
		
		if (cfg.buy[i].id == ITEM_UNKNOWN) {
			fprintf(stderr, "WARNING: \"%s\" is not a valid item. Ignoring.\n", ptr);
			continue;
		}
		
		ptr = trim(strtok(NULL, ","));
		cfg.buy[i].price = atoi(ptr);
		
		if (++i >= MAX_TRADE_LIST_SIZE) {
			fprintf(stderr, "WARNING: Too many items in buy list (max=%d).\n", MAX_TRADE_LIST_SIZE);
			return;
		}
	}
}

void load_config()
{
	int i;
	FILE *f = NULL;
	
	if (!(f = fopen("storebot.conf", "r"))) {
		fprintf(stderr, "ERROR: Could not open storebot.conf\n");
		die();
	}
	
	memset(&cfg, 0, sizeof(cfg));
	
	for (i = 0; i < 35; i++) {
		cfg.buy[i].id = ITEM_UNKNOWN;
		cfg.sell[i].id = ITEM_UNKNOWN;
	}
	
	while (fgets(line, sizeof(line), f)) {
		if (!strncasecmp(line, "<global>", 8)) {
			read_global_config(f);
			continue;
		}
		if (!strncasecmp(line, "<sell>", 6)) {
			read_sell_list(f);
			continue;
		}
		if (!strncasecmp(line, "<buy>", 5)) {
			read_buy_list(f);
			continue;
		}
	}
	
	fclose(f);
}
