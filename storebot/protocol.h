#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifdef WIN32
#include <windows.h>
#endif

#include "types.h"

#ifdef WIN32
#define vsnprintf _vsnprintf
#define closesocket close
#endif

#ifndef WIN32
#define closesocket close
#endif

#define MAX_COMMAND_LENGTH 1024
#define BUFFER_SIZE MAX_COMMAND_LENGTH

typedef struct {
	Uint8 *data;
	int used;
	int size;
} BUFFER;

#ifdef WIN32
#pragma pack (push, 1)
typedef struct {
	Uint8 id;
	Uint16 length;
	Uint8 data[MAX_COMMAND_LENGTH];
} COMMAND;
#pragma pack (pop)
#else
typedef struct __attribute__ ((packed)) _COMMAND {
	Uint8 id;
	Uint16 length;
	Uint8 data[MAX_COMMAND_LENGTH];
} COMMAND;
#endif

extern BUFFER *buf;
extern COMMAND *cmd;
#ifdef WIN32
extern SOCKET server_socket;
#else
extern int server_socket;
#endif
extern int disconnected;

Uint32 net_resolve(const char *hostname);
void connect_to_server();
void read_message_from_server();
void handle_message_from_server(Uint8 command, int length, Uint8 *data);
void handle_raw_text_from_server(Uint8 *data);
void send_command();
void send_login_info();
void send_raw_text(char *text);
void send_pm(char *user, char *format, ...);

#endif /* __PROTOCOL_H__ */
