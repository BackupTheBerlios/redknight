#ifndef __MISC_H__
#define __MISC_H__

void die();
void *safe_malloc(int size);
void safe_free(void *ptr);
char *trim(char *s);
void strtoupper(char *s);

#endif /* __MISC_H__ */
