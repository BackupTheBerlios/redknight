#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "elbot.h"
#include "client_serv.h"
#include "items.h"
#include "credit.h"
#include "misc.h"
#include "actors.h"
#include "types.h"
#include "config.h"
#include "trade.h"

ITEM new_inv_a[36];
ITEM new_inv_b[36];

TRADE_INFO trade;

void trade_with(char *name)
{
    Uint8 str[8]; 
	int id = get_actor_by_name(name);
	
	if (id == -1) {
		return;
	}
	
	str[0] = TRADE_WITH;
	*((int *)(str+1)) = id;
	send_to_server(str, 5);
}

void activate_trade(char *trade_partner)
{
	int i;
	
	if (trade.partner) {
		safe_free(trade.partner);
	}
	
	trade.partner = strdup(trade_partner);

	if (cfg.use_credit_system) {
		memcpy(&new_inv_a, &inv, sizeof(new_inv_a));
	}
	
	for (i = 0; i < 16; i++) {
		trade.our_list[i].quantity = 0;
		trade.his_list[i].quantity = 0;
	}
	
	trade.active = 1;
	trade.time = cur_time;
	trade.mode = TRADE_MODE_NORMAL;
	trade.he_accepted = 0;
	trade.we_accepted = 0;
	trade.verified = 0;
	
	send_pm("%s Greetings, %s! To view the available commands send me a PM with the word \"HELP\".", trade.partner, trade.partner);
}

void handle_trade_command(char *user, char *command, char *args)
{
	int use_guild_prices = is_guild_member(user);
		
	if (!strcasecmp(command, "STOCK")) {
		char arr[36][64];
		char str[150];
		int i, j, k = 0;
		
		for (i = 0; i < MAX_TRADE_LIST_SIZE; i++) {
			if (cfg.sell[i].id == ITEM_UNKNOWN) {
				continue;
			}
			for (j = 0; j < 36; j++) {
				if (cfg.sell[i].id == inv[j].id && inv[j].quantity > 0) {
					sprintf(arr[k++], "%d %s (%dgp)", inv[j].quantity, get_item_name(inv[j].id, 1),
						use_guild_prices ? cfg.sell[i].guild_price : cfg.sell[i].price);
				}
			}
		}
		
		if (k == 0) {
			send_pm("%s %s", user, "Nothing in stock, sorry.");
			return;
		}
		
		for (i = 0, str[0] = 0; i < k; i++) {
			if ((strlen(str) + strlen(arr[i]) + 2) >= 150) {
				send_pm("%s %s", user, str);
				str[0] = 0;
			}
			sprintf(str+strlen(str), "%s, ", arr[i]);
		}
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
		send_pm("%s %s", user, str);
		
		if (trade.active && !strcasecmp(user, trade.partner)) {
		send_pm("%s To buy an item: BUY <quantity> <item name> ", user);
		} else {
		send_pm("%s Trade with me to buy or sell items.", user);
		}
		
		return;
	}

	if (!strcasecmp(command, "WANTED")) {
		char str[2];
		int i, j = 0;
		
		str[1] = 0;
		
		for (i = 0; i < 35; i++) {
			if (cfg.buy[i].id == ITEM_UNKNOWN) {
				continue;
			}
			str[0] = (cfg.buy[i].price > 1) ? 's' : 0;
			send_pm("%s - %s for %d gold coin%s", user,
				get_item_name(cfg.buy[i].id, 1),
				cfg.buy[i].price,
				str);
			j++;
		}
		
		if (j == 0) {
			send_pm("%s %s", user, "- I'm not buying anything");
		}
		
		return;
	}
	
	if (trade.active && !strcasecmp(user, trade.partner) && cfg.use_credit_system && !strcasecmp(command, "DEPOSIT")) {
		send_pm("%s %s", trade.partner, "OK, you will be paid in credits instead of gold coins.");
		trade.mode = TRADE_MODE_DEPOSIT;
		return;
	}
	
	if (!strcasecmp(command, "CREDITS")) {
		if (!cfg.use_credit_system) {
			send_pm("%s %s", user, "Sorry, the credit system is disabled.");
		} else {
			send_pm("%s You have %d gold coin(s) in credit.", user,
				get_credits_for_user(user));
		}
		return;
	}                     
	
	if (trade.active && !strcasecmp(user, trade.partner) && !strcasecmp(command, "DONATE")) {
		send_pm("%s Thank you, %s, any donations will be appreciated.", trade.partner, trade.partner);
		trade.mode = TRADE_MODE_DONATION;
		return;
	}
	
	if (trade.active && !strcasecmp(user, trade.partner) && !strcasecmp(command, "BUY")) {
		int i, pos, price, id, quantity;
		char *name, *name_plural;
		
		if (!args || !strchr(args, ' ')) {
			send_pm("%s %s", user, "Usage: BUY <quantity> <item name>");
			return;
		}
		
		quantity = atoi(args);
		
		if (!quantity) {
			send_pm("%s %s", user, "Usage: BUY <quantity> <item name>");
			return;		
		}
		
		while (*args && *args != ' ')
			args++;
		while (*args && *args == ' ')
			args++;
		
		name = args;
		
		id = get_item_id(name);
		
		if (id == ITEM_UNKNOWN) {
			send_pm("%s Sorry, I don't recognise that item name", trade.partner);
			return;
		}
		
		name = get_item_name(id, 0);
		name_plural = get_item_name(id, 1);

		price = get_sell_price(id, use_guild_prices);
		
		if (price == -1) {
			send_pm("%s Sorry, I'm not selling %s", trade.partner,  name_plural);
			return;
		}
		
		pos = -1;
		
		for (i = 0; i < 36; i++) {
			if (inv[i].quantity == 0 || inv[i].id != id) {
				continue;
			}
			pos = i;
		}
		
		if (pos == -1) {
			send_pm("%s Sorry, I don't have any %s in stock.", trade.partner, name_plural);
			return;
		}
		
		if (inv[pos].quantity < quantity) {
			quantity = inv[pos].quantity;
			
			send_pm("%s Sorry, I only have %d %s.", trade.partner,
				quantity,
				(quantity > 1) ? name_plural : name);
		}

		put_object_on_trade(pos, quantity);
		update_trade();
		
		if (trade.his_list_value > 0 && trade.our_list_value == trade.his_list_value) {
			accept_trade();
		} else {
			reject_trade();
		}
		return;
	}
	
	send_pm("%s ------------------------------------------- ", user);
	send_pm("%s STOCK    Display the list of items in stock ", user);
	send_pm("%s WANTED   Display the items I will buy ", user);
	
	if (cfg.use_credit_system && trade.active && !strcasecmp(user, trade.partner)) {
	send_pm("%s DEPOSIT  Exchange items/money for credits", user);
	}
	
	if (cfg.use_credit_system) {
	send_pm("%s CREDITS  Display your credit balance",user);
	}
	
	if (trade.active && !strcasecmp(user, trade.partner)) {
	send_pm("%s DONATE   Donate items or money",user);
	}
	
	send_pm("%s ------------------------------------------- ",user);
	
	if (trade.active && !strcasecmp(user, trade.partner)) {
	send_pm("%s To buy an item: BUY <quantity> <item name>",user);
	} else {
	send_pm("%s Trade with me to buy or sell items.",user);
	}
}

void finalize_trade()
{
	if (!cfg.use_credit_system) {
		return;
	}

	if (trade.his_list_value > 0 && trade.he_accepted && trade.we_accepted) {
		if (trade.deposit > 0) {
			add_credits_to_user(trade.partner, trade.deposit);
		}
		if (trade.deposit < 0) {
			remove_credits_from_user(trade.partner, -trade.deposit);
		}
		send_pm("%s Your credit balance is %d gold coin(s).", trade.partner,
			get_credits_for_user(trade.partner));
	}
}

void end_trade()
{
	trade.active = 0;
}

void abort_trade()
{
    Uint8 str[4]; 
	str[0] = EXIT_TRADE;

	send_to_server(str, 1);
	end_trade();
}

void get_trade_object(int pos, Uint16 id, Uint32 quantity)
{
	trade.his_list[pos].id = id;
	trade.his_list[pos].quantity += quantity;
	update_trade();
}

void remove_trade_object(int pos, Uint16 quantity)
{
	trade.his_list[pos].quantity -= quantity;
	update_trade();
}

int get_buy_price(int id)
{
	int i;
	
	for (i = 0; i < 35; i++) {
		if (cfg.buy[i].id == id) {
			return cfg.buy[i].price;
		}
	}
	
	return -1;
}

int get_sell_price(int id, int use_guild_price)
{
	int i;
	
	for (i = 0; i < 35; i++) {
		if (cfg.sell[i].id == id) {
			return use_guild_price ? cfg.sell[i].guild_price : cfg.sell[i].price;
		}
	}

	return -1;
}

void put_object_on_trade(int pos, Uint16 quantity)
{
	int i;
	Uint8 str[8];

	str[0] = PUT_OBJECT_ON_TRADE;
	str[1] = pos;
	*((Uint16 *)(str+2)) = quantity;
	send_to_server(str, 4);
	
	for (i = 0; i < 16; i++) {
		if (trade.our_list[i].id == inv[pos].id
				&& trade.our_list[i].quantity > 0
				&& (trade.our_list[i].quantity + quantity) < pow(2, 16)) {
			trade.our_list[i].quantity += quantity;
			return;
		}
	}
	
	for (i = 0; i < 16; i++) {
		if (trade.our_list[i].quantity == 0) {
			trade.our_list[i].id = inv[pos].id;
			trade.our_list[i].quantity = quantity;
			return;
		}
	}
}

void remove_object_from_trade(int pos, Uint16 quantity)
{
    Uint8 str[8]; 
     
	str[0] = REMOVE_OBJECT_FROM_TRADE;
	str[1] = pos;
	*((Uint16 *)(str+2)) = quantity;
	send_to_server(str, 4);
	
	trade.our_list[pos].quantity -= quantity;
}

void update_trade()
{
	int i, price, use_guild_prices;
	
	use_guild_prices = is_guild_member(trade.partner);
	
	trade.our_list_value = 0;
	trade.his_list_value = 0;
	trade.deposit = 0;
	
	memcpy(&new_inv_b, new_inv_a, sizeof(new_inv_b));
	
	// Calculate the value of his trade list.
	for (i = 0; i < 16; i++) {
		if (!trade.his_list[i].quantity) {
			continue;
		}
		
		if (trade.his_list[i].id == ITEM_GOLD_COINS) {
			trade.his_list_value += trade.his_list[i].quantity;
		} else {
			price = get_buy_price(trade.his_list[i].id);
		
			// If one of the items being offered is not in
			// our buy list, reject the trade.
			if (price == -1) {
				if (trade.mode == TRADE_MODE_DONATION) {
					send_pm("%s Sorry, I don't want %s.", trade.partner, 
						get_item_name(trade.his_list[i].id, 1));
				} else {
					send_pm("%s I'm not buying %s.", trade.partner, 
						get_item_name(trade.his_list[i].id, 1));
				}
				reject_trade();
				return;
			}
			
			trade.his_list_value += price * trade.his_list[i].quantity;
		}
		
		add_item_to_new_inventory(trade.his_list[i].id, trade.his_list[i].quantity);
	}
	
	// Calculate the value of our trade list.
	for (i = 0; i < 16; i++) {
		if (trade.our_list[i].quantity == 0) {
			continue;
		}
		
		if (trade.our_list[i].id == ITEM_GOLD_COINS) {
			trade.our_list_value += trade.our_list[i].quantity;
		} else {
			price = get_sell_price(trade.our_list[i].id, use_guild_prices);
			
			if (price == -1) {
				// This may happen if the items that are in our sell list
				// are altered during the trade.
				send_pm("%s Sorry, %s, an error occurred during the trade. Please try again.",
					trade.partner, trade.partner);
				abort_trade();
				return;
			}
			
			trade.our_list_value += price * trade.our_list[i].quantity;
		}
		
		remove_item_from_new_inventory(trade.our_list[i].id, trade.our_list[i].quantity);
	}
	
	if (trade.mode != TRADE_MODE_DONATION && trade.his_list_value > trade.our_list_value) {
		int amount_due = trade.his_list_value - trade.our_list_value;
		
		if (cfg.use_credit_system && trade.mode == TRADE_MODE_DEPOSIT) {
			trade.deposit += amount_due;
			trade.our_list_value += amount_due;
		} else {
			for (i = 0; i < 36; i++) {
				if (inv[i].quantity == 0 || inv[i].id != ITEM_GOLD_COINS) {
					continue;
				}
				if (inv[i].quantity >= amount_due) {
					put_object_on_trade(i, amount_due);
					trade.our_list_value += amount_due;
					amount_due = 0;
					break;
				} else {
					put_object_on_trade(i, inv[i].quantity);
					trade.our_list_value += inv[i].quantity;
					amount_due -= inv[i].quantity;
				}
			}
			
			if (amount_due > 0) {
				send_pm("%s Sorry, I dont have enough gold coins.", trade.partner);
				reject_trade();
				return;
			}
		}
	} else if (trade.our_list_value > trade.his_list_value) {
		int amount_due, credits = 0;
		
		if (cfg.use_credit_system) {
			credits = get_credits_for_user(trade.partner);
		}
		
		if ((amount_due = trade.our_list_value - trade.his_list_value) > 0) {
			for (i = 0; i < 16; i++) {
				if (trade.our_list[i].quantity == 0 || trade.our_list[i].id != ITEM_GOLD_COINS) {
					continue;
				}
				if (trade.our_list[i].quantity >= amount_due) {
					remove_object_from_trade(i, amount_due);
					trade.our_list_value -= amount_due;
					amount_due = 0;
					break;
				} else {
					remove_object_from_trade(i, trade.our_list[i].quantity);
					trade.our_list_value -= trade.our_list[i].quantity;
					amount_due -= trade.our_list[i].quantity;
				}
			}
		}
	
		if (amount_due > 0) {
			if (credits >= amount_due) {
				credits = amount_due;
			}
			
			trade.deposit -= credits;
			amount_due -= credits;
			trade.his_list_value += credits;
			
			if (credits > 0) {
				send_pm("%s This will use %d of your credits.", trade.partner, credits);
			}
		}
	}
	
	if (trade.his_list_value > 0 && (trade.mode == TRADE_MODE_DONATION || trade.his_list_value == trade.our_list_value)) {
		accept_trade();
	} else {
		reject_trade();
	}
	
	if (trade.our_list_value > trade.his_list_value) {
		send_pm("%s You owe %d gold coins.", trade.partner,
			trade.our_list_value - trade.his_list_value);
	}
}

int add_item_to_new_inventory(int id, Uint32 quantity)
{
	int i;
	
	for (i = 0; i < 36; i++) {
		if (new_inv_b[i].quantity == 0) {
			continue;
		}

		if (new_inv_b[i].id == id && (new_inv_b[i].quantity + quantity) < pow(2, 32)) {
			new_inv_b[i].quantity += quantity;
			return 0;
		}
	}
	
	for (i = 0; i < 36; i++) {
		if (new_inv_b[i].quantity > 0) {
			continue;
		}
		new_inv_b[i].id = id;
		new_inv_b[i].quantity = quantity;
		return 0;
	}
	
	return 1;
}

void remove_item_from_new_inventory(int id, Uint32 quantity)
{
	int i;
	
	for (i = 0; i < 36; i++) {
		if (new_inv_b[i].id != id || new_inv_b[i].quantity == 0) {
			continue;
		}
		if (new_inv_b[i].quantity >= quantity) {
			new_inv_b[i].quantity -= quantity;
			return;
		} else {
			quantity -= new_inv_b[i].quantity;
			new_inv_b[i].quantity = 0;
		}
	}
}

void check_inventory()
{
	// We only need to check our inventory if we're using
	// the credit system. This is to avoid mistakenly giving
	// out credits even if the trade failed.
	if (!cfg.use_credit_system) {
		finalize_trade();
		return;
	}
	
	if (trade.he_accepted && trade.we_accepted) {
		int i;
		
		for (i = 0; i < 36; i++) {
			if (inv[i].id != new_inv_b[i].id) {
				return;
			}
			if (inv[i].quantity != new_inv_b[i].quantity) {
				return;
			}
		}

		trade.verified = 1;
		
		finalize_trade();
		return;
	}
	
	return;
}

void accept_trade()
{
    Uint8 str[4];
     
	str[0] = ACCEPT_TRADE;
	send_to_server(str, 1);
}

void reject_trade()
{
    Uint8 str[4];
     
	str[0] = REJECT_TRADE;
	send_to_server(str, 1); 
}
