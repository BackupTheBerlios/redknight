#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "elbot.h"
#include "privilege.h"
#include "misc.h"

char privilege_file[] = "./db/privilege.bin";

void create_privilege_file()
{
	FILE *f = fopen(privilege_file, "w");
	
	if (!f) {
		fprintf(stderr, "ERROR: Could not create privilege file (%s)", strerror(errno));
		die();
	}
	
	fclose(f);
}

int get_privilege_for_user(char *user)
{
	PRIVILEGE_INFO pi;
	FILE *f = fopen(privilege_file, "rb");
	
	if (!f) {
		create_privilege_file();
		return 0;
	}
	
	while (fread(&pi, 1, sizeof(pi), f) == sizeof(pi)) {
		if (strcasecmp(user, pi.user)) {
			continue;
		}
		return pi.level;
	}
	
	return 0;
}

int set_privilege_for_user(char *user, int level)
{
	PRIVILEGE_INFO pi;
	FILE *f = fopen(privilege_file, "r+");
	
	if (!f) {
		create_privilege_file();
		return 0;
	}
	
	while (fread(&pi, 1, sizeof(pi), f) == sizeof(pi)) {
		if (strcasecmp(user, pi.user)) {
			continue;
		}
		pi.level = level;
		fseek(f, -sizeof(pi), SEEK_CUR);
		fwrite(&pi, 1, sizeof(pi), f);
		fclose(f);
		return pi.level;
	}
	
	memset(&pi.user, 0, sizeof(pi.user));
	
	strncpy(pi.user, user, 16);
	if (strlen(user) >= 16) {
		pi.user[15] = 0;
	}
	pi.level = level;
	fwrite(&pi, 1, sizeof(pi), f);
	fclose(f);
	
	return pi.level;
}
