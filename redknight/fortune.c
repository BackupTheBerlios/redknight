#include <stdio.h>
#include <string.h>

//Fortune Generator

#ifdef STANDALONE
#define output(MYSTRING) printf(MYSTRING);
#else
#include "elbot.h"
#define output(MYSTRING) send_pm("%s%s", name, MYSTRING); sprintf(log,"Fortune:%s",MYSTRING); log_info(log);
#endif

#define MAXCHAR 152

long makerand(long max)
{
	/* Reseeding random number generator */
	srand(time(NULL));

	return (rand() % max);
}

int txt2bin(char *file)
{
     int i = 0, j = 0, fd = 0, _fd = 0, s = 1, c = 0, t = 0;
     char b, buffer[MAXCHAR];
     char lastchar, temp;
     
     fd = open(file, 0);
     sprintf(buffer, "%s.dat", file);
     _fd = creat(buffer, 0777);
     
     if(fd == -1) {
               printf("No such file!");
               return 0;
     }
     
     //First, find number of entries
     //Never use % ANYWHERE
     #ifndef HEADER
     s = 1;
     
     while(s > 0) {
             while(s = read(fd, &b, 1) > 0) {
                     temp = b;
                     if(b == '\n')
                            b = ' ';
                     if((b == ' ' && lastchar == ' ') || b >= 127 || (b <= 31 && (b != 10 || b != '\n')))
                     {
                          b = 0;
                          continue;
                     }
                     //find \n%
                     if(b == '%' && lastchar == '\n') 
                     {
                           //Write and reset
                           c++;
                           i = 0;
                           continue;
                     }
                     else if(i >= MAXCHAR)
                     {
                          i = 0;
                          //Ok, get to \n%
                          while(s = read(fd, &b, 1) > 0 && (b != '%' || lastchar != '\n')) {
                                  lastchar = b;
                                  b = 0;
                          }
                          b = 0;
                          continue;
                     }
                     lastchar = temp;
                     i++; //Char count
                     b = 0;
             }
     }
     printf("Fortune Cookies Generated: %d\n", c);
     
     //Rewind
     lseek(fd, 0L, 0);
     
     //Write number of entries. Count on + 0, thus % after the last too
     write(_fd, &c, sizeof(c));
     
     //Make sure the buffer is initilaised
     while(j < MAXCHAR)
             buffer[j++]='\0';
     j = 0;
     s = 1;
     lastchar = 0;
     
     while(s > 0) {
             while(s = read(fd, &b, 1) > 0) {
                     temp = b;
                     if(b == '\n')
                            b = ' ';
                     if((b == ' ' && lastchar == ' ') || b >= 127 || (b <= 31 && (b != 10 || b != '\n')))
                     {
                          b = 0;
                          continue;
                     }
                     //find \n%
                     if(b == '%' && lastchar == '\n') 
                     {
                           //Write and reset
                           write(_fd, buffer, MAXCHAR);
                           i = 0;
                           while(j < MAXCHAR)
                                   buffer[j++]='\0';
                           j = 0;
                           continue;
                     }
                     else if(i >= MAXCHAR)
                     {
                          i = 0;
                          while(j < MAXCHAR)
                                   buffer[j++]='\0';
                          j = 0;
                          //Ok, get to \n%
                          while((s = read(fd, &b, 1) > 0) && (b != '%' || lastchar != '\n')) {
                                  lastchar = b;
                                  b = 0;
                          }
                          b = 0;
                          continue;
                     }
                     lastchar = temp;                     
                     buffer[i++] = b;
                     b = 0;
             }
     }
     #else
     j = 0;
     while(j < MAXCHAR)
             buffer[j++]='\0';
     //Skip the header
     write(fd, &c, (long)sizeof(c));

     //This takes almost as long as copying, for accuracy purposes
     while(s > 0) {
             while(s = read(fd, &b, 1) > 0) {
                     if(b == '\n')
                            b = ' ';
                     if((b == ' ' && buffer[i-1] == ' ') || b >= 127 || (b <= 31 && (b != 10 || b != '\n')))
                     {
                          b = 0;
                          continue;
                     }
                     if(b == '%') 
                     {
                           //Note and reset
                           c++;
                           t += (i+1);
                           write(_fd, &t, sizeof(t));
                           i = 0;
                           while(j < MAXCHAR)
                                   buffer[j++]='\0';
                           j = 0;
                           continue;
                     }
                     else if(i >= MAXCHAR)
                     {
                          i = 0;
                          b = 0;
                          while(j < MAXCHAR)
                                   buffer[j++]='\0';
                          j = 0;
                          continue;
                     }
                     buffer[i++] = b;
                     b = 0;
             }
     }
     //Reset buffer
     j = 0;
     while(j < MAXCHAR)
             buffer[j++]='\0';
     //Write c
     lseek(_fd, 0L, 0);
     write(_fd, &c, sizeof(c));
     lseek(_fd, (long)(sizeof(t) * c), 0);
     lseek(fd, 0L, 0);

     //Now...copy it
     j = 0;
     s = 1;
     
     while(s > 0) {
             while(s = read(fd, &b, 1) > 0) {
                     if(b == '\n')
                            b = ' ';
                     if((b == ' ' && buffer[i-1] == ' ') || b >= 127 || (b <= 31 && (b != 10 || b != '\n')))
                     {
                          b = 0;
                          continue;
                     }
                     if(b == '%' && buffer[i-1] == '\n') 
                     {
                           //Write and reset
                           buffer[i+1]='\0';
                           write(_fd, buffer, i+1);
                           i = 0;
                           while(j < MAXCHAR)
                                   buffer[j++]='\0';
                           j = 0;
                           continue;
                     }
                     else if(i >= MAXCHAR)
                     {
                          i = 0;
                          b = 0;
                          while(j < MAXCHAR)
                                   buffer[j++]='\0';
                          j = 0;
                          continue;
                     }
                     buffer[i++] = b;
                     b = 0;
             }
     }
     #endif
}

#ifdef STANDALONE
int get_quote(char *file)
#else
int get_quote(char *file, char *name)
#endif
{
     int fd = 0, c = 0, s = 0, i = 0;
     long rand_no = 0;
     char buffer[MAXCHAR], log[512];
          
     fd = open(file, 0);
     
     if(fd == -1) {
               printf("No such file! %s%s", ROOTDIR, file);
               return 0;
     }
     
     //Read number of entries
     s = read(fd, &c, sizeof(c));
     //Pick a random entry
     rand_no = makerand(c);
     
     #ifndef HEADER
     //lseek it
     lseek(fd, (long)sizeof(c)+(MAXCHAR * rand_no), 0);
     //Read it
     s = read(fd, buffer, MAXCHAR);
     #else
     // BROKEN
     //Seek it
     lseek(fd, (long)(sizeof(c) + (sizeof(c) * rand_no)), 0);
     s = read(fd, &i, sizeof(i));
     printf("Entry %d, number %d, picked %d: Int=%d - C=%d", i, c, rand_no, sizeof(int), sizeof(c));
     lseek(fd, (long)(sizeof(c) + (sizeof(i) * c) + i), 0);     
     //Read it
     s = read(fd, buffer, i);     
     #endif
     
     output(buffer);
     //send_pm("%s %s", name, buffer);
}

#ifdef STANDALONE                     
int main()
{
      //if(txt2bin("fortune"))
      get_quote("fortune.dat");
}   
#endif
        