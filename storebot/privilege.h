#ifndef __PRIVILEGE_H__
#define __PRIVILEGE_H__

#include "types.h"

typedef struct {
	char user[16];
	Uint8 level;
} PRIVILEGE_INFO;

void create_privilege_file();
int get_privilege_for_user(char *user);
int set_privilege_for_user(char *user, int level);

#endif /* __PRIVILEGE_H__ */
