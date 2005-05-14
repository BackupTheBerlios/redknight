#ifndef __TIMERS_H__
#define __TIMERS_H__

// STRUCTURE DEFINITIONS
struct TIMER {
       Uint32 n;     // Incrementation after a successful iteration
       Uint32 t;     // Next time when we'll perform the callback
       unsigned short (*c)();     /* Callback, 0 on failure (don't set t), */
                                  /*  1 on success (set t)                 */
       /* We use a linked list, so we can add timers */
       /*    without modifying annoying macros       */
       struct TIMER *prev, *next;
} ;

// GLOBAL VARIABLES
extern struct TIMER *timer;

// PROTOTYPES
void timer_loop(Uint32 time);
struct TIMER * add_timer(Uint32 start, Uint16 increment, unsigned short (*func)());
unsigned short remove_timer(struct TIMER *kill);

#endif
