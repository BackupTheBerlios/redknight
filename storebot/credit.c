#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "elbot.h"
#include "misc.h"
#include "credit.h"

char credit_file[] = "./db/credit.bin";

void create_credit_file()
{
	FILE *f = fopen(credit_file, "w");
	
	if (!f) {
		fprintf(stderr, "ERROR: Could not create credit file (%s)", strerror(errno));
		die();
	}
	
	fclose(f);
}

Uint32 get_credits_for_user(char *user)
{
	if (user) {
		CREDIT_INFO ci;
		FILE *f = fopen(credit_file, "rb");
		
		if (!f) {
			create_credit_file();
			return 0;
		}
		
		while (fread(&ci, 1, sizeof(ci), f) == sizeof(ci)) {
			if (strcasecmp(user, ci.user)) {
				continue;
			}
			return ci.amount;
		}
	}
	return 0;
}

Uint32 add_credits_to_user(char *user, int amount)
{
	if (user && amount > 0) {
		CREDIT_INFO ci;
		FILE *f = fopen(credit_file, "r+");
		
		if (!f) {
			create_credit_file();
			return 0;
		}
		
		while (fread(&ci, 1, sizeof(ci), f) == sizeof(ci)) {
			if (strcasecmp(user, ci.user)) {
				continue;
			}
			ci.amount += amount;
			fseek(f, -sizeof(ci), SEEK_CUR);
			fwrite(&ci, 1, sizeof(ci), f);
			fclose(f);
			return ci.amount;
		}
		
		memset(&ci.user, 0, sizeof(ci.user));
		
		strncpy(ci.user, user, 16);
		if (strlen(user) >= 16) {
			ci.user[15] = 0;
		}
		ci.amount = amount;
		fwrite(&ci, 1, sizeof(ci), f);
		fclose(f);
		
		return ci.amount;
	}
	return 0;
}

Uint32 remove_credits_from_user(char *user, int amount)
{
	if (user && amount > 0) {
		CREDIT_INFO ci;
		FILE *f = fopen(credit_file, "r+");
		
		if (!f) {
			create_credit_file();
			return 0;
		}
		
		while (fread(&ci, 1, sizeof(ci), f) == sizeof(ci)) {
			if (strcasecmp(user, ci.user)) {
				continue;
			}
			ci.amount -= amount;
			fseek(f, -sizeof(ci), SEEK_CUR);
			fwrite(&ci, 1, sizeof(ci), f);
			fclose(f);
			return ci.amount;
		}
	}
	return 0;
}

Uint32 set_credits_for_user(char *user, int amount)
{
	if (user && amount > 0) {
		CREDIT_INFO ci;
		FILE *f = fopen(credit_file, "r+");
		
		if (!f) {
			create_credit_file();
			return 0;
		}
		
		while (fread(&ci, 1, sizeof(ci), f) == sizeof(ci)) {
			if (strcasecmp(user, ci.user)) {
				continue;
			}
			ci.amount = amount;
			fseek(f, -sizeof(ci), SEEK_CUR);
			fwrite(&ci, 1, sizeof(ci), f);
			fclose(f);
			return ci.amount;
		}
		
		memset(&ci.user, 0, sizeof(ci.user));
		
		strncpy(ci.user, user, 16);
		if (strlen(user) >= 16) {
			ci.user[15] = 0;
		}
		ci.amount = amount;
		fwrite(&ci, 1, sizeof(ci), f);
		fclose(f);
		
		return ci.amount;
	}
	return 0;
}
