#ifndef __TRADE_H__
#define __TRADE_H__

#include "items.h"

typedef struct _TRADE_INFO {
	int active;
	int time;
	int mode;
	
	int verified;
	
	ITEM our_list[16];
	ITEM his_list[16];
	
	int our_list_value;
	int his_list_value;
	
	int deposit;
	
	int he_accepted;
	int we_accepted;
	
	char *partner;
} TRADE_INFO;

typedef struct _TRADE_ITEM {
	int id;
	int price;
	int guild_price;
} TRADE_ITEM;

// Trade modes
enum {
	TRADE_MODE_NORMAL,
	TRADE_MODE_DEPOSIT,
	TRADE_MODE_DONATION
};

extern TRADE_INFO trade;

void trade_with(char *name);
void activate_trade(char *trade_partner);
void handle_trade_command(char *user, char *command, char *args);
void finalize_trade();
void end_trade();
void abort_trade();
void get_trade_object(int pos, Uint16 id, Uint32 quantity);
void remove_trade_object(int pos, Uint16 quantity);
int get_buy_price(int id);
int get_sell_price(int id, int use_guild_price);
void put_object_on_trade(int pos, Uint16 quantity);
void remove_object_from_trade(int pos, Uint16 quantity);
void update_trade();
int add_item_to_new_inventory(int id, Uint32 quantity);
void remove_item_from_new_inventory(int id, Uint32 quantity);
void check_inventory();
void accept_trade();
void reject_trade();

#endif /* __TRADE_H__ */
