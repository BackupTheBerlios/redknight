#ifndef STORIES_H
/*stories.h*/
#define STORIES_H
//Story definitions
#define NO_STORIES 3

#include "includes.h"

extern int story;
extern int story_pause[NO_STORIES];
extern int story_time;

void read_story();
void read_intro();
void calc_story_time(int STORY);

#endif/*stories.h*/
