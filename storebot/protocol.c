#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "elbot.h"
#include "client_serv.h"
#include "misc.h"
#include "types.h"
#include "config.h"
#include "actors.h"
#include "protocol.h"

BUFFER *buf;
COMMAND *cmd;
#ifdef WIN32
SOCKET server_socket = 0;
#else
int server_socket = 0;
#endif
int disconnected;

Uint32 net_resolve(const char *hostname)
{
	Uint32 ip = inet_addr(hostname);
	if (ip == INADDR_NONE) {
		struct hostent *he = gethostbyname(hostname);
		if (!he) {
			return INADDR_NONE;
		}
		memcpy(&ip, he->h_addr_list[0], he->h_length);
	}
	return ip;
}

void connect_to_server()
{
	struct sockaddr_in sin;
	
	Uint32 ip = net_resolve(cfg.server_address);
	
	if (ip == INADDR_NONE) {
		fprintf(stderr, "ERROR: Could not resolve %s (h_errno=%d)\n", cfg.server_address, h_errno);
		die();
	}
	
	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		fprintf(stderr, "ERROR: Could not create socket\n");
		die();
	}
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ip;
	sin.sin_port = htons(cfg.server_port);
	
	if (connect(server_socket, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		fprintf(stderr, "ERROR: Could not connect to server\n");
		die();
	}
}

void read_message_from_server()
{
	Uint16 packet_length;
	int have_packet_length;
	int len;

	buf->used = have_packet_length = packet_length = 0;
	
	while (!have_packet_length) {
		len = recv(server_socket, buf->data+buf->used, 3 - buf->used, 0);
		if (len <= 0) {
			fprintf(stderr, "ERROR: Disconnected\n");
			disconnected = 1;
			return;
		}
		buf->used += len;
		if (buf->used == 3) {
			packet_length = *((Uint16 *)(buf->data+1));
			packet_length--;
			have_packet_length = 1;
		}
	}
	
	if (packet_length > buf->size-3) {
		fprintf(stderr, "ERROR: Packet too large (%d bytes)\n", packet_length);
		die();
	}
	
	while (buf->used < packet_length+3) {
		len = recv(server_socket, buf->data+buf->used, packet_length - buf->used + 3, 0);
		
		if (len <= 0) {
			fprintf(stderr, "ERROR: Disconnected\n");
			disconnected = 1;
			return;
		}
		
		buf->used += len;
	}
	
	buf->data[packet_length+3] = 0;
	
	handle_message_from_server(buf->data[0], packet_length, buf->data+3);
}

void handle_message_from_server(Uint8 command, int length, Uint8 *data)
{
	switch (command) {
	case RAW_TEXT:
 		printf("%s\n", data+1);
		handle_raw_text_from_server(data);
		break;
	case YOU_ARE:
		me = *((short *)(data));
		break;
	case ADD_NEW_ACTOR:
		add_actor(*((short *)(data)), data+23, *((short*)(data+2)), *((short*)(data+4)));
		break;
	case ADD_NEW_ENHANCED_ACTOR:
		add_actor(*((short *)(data)), data+28, *((short*)(data+2)), *((short*)(data+4)));
		break;
	case REMOVE_ACTOR:
		destroy_actor(*((short *)(data)));
		break;
	case KILL_ALL_ACTORS:
		destroy_all_actors();
		break;
	case ADD_ACTOR_COMMAND:
		handle_actor_command(*((short *)(data)), data[2]);
		break;
	case HERE_YOUR_INVENTORY:
		get_inventory_from_server(data);
		break;
	case GET_NEW_INVENTORY_ITEM:
		get_new_inventory_item_from_server(data);
		break;
	case REMOVE_ITEM_FROM_INVENTORY:
		remove_inventory_item(data[0]);
		break;
	case GET_TRADE_ACCEPT:
		if (data[0]) {
			trade.he_accepted = 1;
		} else {
			trade.we_accepted = 1;
		}
		break;
	case GET_TRADE_REJECT:
		if (data[0]) {
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
		if (data[7]) {
			get_trade_object(data[6], *((Uint16 *)(data)), *((Uint32 *)(data+2)));
		}
		break;
	case REMOVE_TRADE_OBJECT:
		if (data[3]) {
			remove_trade_object(data[2], *((Uint16 *)(data)));
		}
		break;
	case GET_YOUR_TRADEOBJECTS:
		get_inventory_from_server(data);
		break;
	case GET_TRADE_PARTNER_NAME:
		data[length] = 0;
		activate_trade(data);
		break;
	case LOG_IN_NOT_OK:
		fprintf(stderr, "ERROR: Invalid password\n");
		die();
	case YOU_DONT_EXIST:
		fprintf(stderr, "ERROR: Invalid username\n");
		die();
	}
}

void handle_raw_text_from_server(Uint8 *data)
{
	if (data[0] == 128 && !strncmp(data+1, "[PM from ", 8)) {
		Uint8 user[16], *command, *args;
		int i;
		
		// Get the user's name
		for (i = 0; i < sizeof(user) && data[i+10] != ':'; i++) {
			user[i] = data[i+10];
		}
		user[i] = 0;
		
		// Get the command name
		command = data+i+12;
		command[strlen(command)-1] = 0;
		command = trim(command);
		
		// Get the arguments if there are any
		if ((args = strstr(command, " "))) {
			*args++ = 0;
			args = trim(args);
		}
		
		handle_command(user, command, args);
		
		return;
	}
	
	if (data[0] == 136 && strstr(data+1, " wants to trade with you")) {
		char *actor_name;
		
		actor_name = strstr(data+1, " wants to trade with you");
		*actor_name = 0;
		actor_name = data+1;
		
		trade_with(actor_name);
		return;
	}
}

void send_command()
{
	send(server_socket, (void *)cmd, cmd->length+2, 0);
}

void send_login_info()
{
	cmd->id = LOG_IN;
	sprintf(cmd->data, "%s %s", cfg.username, cfg.password);
	cmd->length = strlen(cmd->data) + 2;
	send_command();
}

void send_raw_text(char *text)
{
	cmd->id = RAW_TEXT;
	cmd->length = strlen(text)+2;
	strcpy(cmd->data, text);
	send_command();
}

void send_pm(char *user, char *format, ...)
{
	va_list arg;
			
	sprintf(cmd->data, "%s ", user);
	
	va_start(arg, format);
	vsnprintf(cmd->data+strlen(cmd->data), MAX_COMMAND_LENGTH-strlen(cmd->data), format, arg);
	va_end(arg);
	
	cmd->id = SEND_PM;
	cmd->length = strlen(cmd->data) + 2;
	send_command();
}
