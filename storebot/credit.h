#include "elbot.h"
#include "types.h"

typedef struct {
	char user[16];
	Uint32 amount;
} CREDIT_INFO;

void create_credit_file();
Uint32 get_credits_for_user(char *user);
Uint32 add_credits_to_user(char *user, int amount);
Uint32 remove_credits_from_user(char *user, int amount);
Uint32 set_credits_for_user(char *user, int amount);
