#ifndef __ELBOT_H__
#define __ELBOT_H__

extern int cur_time;

void init();
void handle_command(char *user, char *command, char *args);
#endif /* __ELBOT_H__ */
