#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "types.h"
#include "items.h"
#include "trade.h"

#define MAX_TRADE_LIST_SIZE 200

typedef struct
{
	char server_address[256];
	Uint16 server_port;
	
	char username[16];
	char password[16];
	char guild[16];
	char admin[16];
	
	Uint8 use_credit_system;
	int max_trade_time;
	
	TRADE_ITEM buy[MAX_TRADE_LIST_SIZE];
	TRADE_ITEM sell[MAX_TRADE_LIST_SIZE];
} CONFIG;

extern CONFIG cfg;

void load_config();

#endif /* __CONFIG_H__ */
