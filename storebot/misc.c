#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#endif

#include "elbot.h"
#include "misc.h"

void die()
{
	exit(1);
}

void *safe_malloc(int size)
{
	void *buffer = malloc(size);
	if (!buffer) {
		fprintf(stderr, "ERROR: Couldn't allocate memory\n");
		die();
	}
	memset(buffer, 0, size);
	return buffer;
}

void safe_free(void *ptr)
{
	if (ptr) {
		free(ptr);
	}
}

char *trim(char *s)
{
	char *ptr;
	
	if (!s) return NULL;
	
	while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
		s++;
	}
	
	ptr = s + strlen(s) - 1;
	
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
		*ptr-- = 0;
	}
	
	return s;
}

void strtoupper(char *s)
{
	while (*s) {
		*s = toupper(*s);
		s++;
	}
}
