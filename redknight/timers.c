/******************************************************************************/
/* Timers.c - Timing using SDL_GetTick() for the redknight program            */
/*            This contains all setup timers, and should be called from a     */
/*            main loop, unless rewritten to use threads.                     */
/******************************************************************************/


// FILE INCLUSIONS
#include "includes.h"
#include "timers.h"

// GLOBAL VARIABLES
struct TIMER *timer = NULL;


// Take the time as an argument. Good for debugging
void timer_loop(Uint32 time)
{
     struct TIMER *cur;
     
     // Loop, going to the next section, while there is one
     for(cur = timer; cur != NULL; cur = cur->next) {
           if(time < (cur->t - cur->n - 1))  // Looks like we've stuck
           {
                 cur->t = time;          // Reset it
                 log_error("Resetting Timer - it was too high!\n");
                 continue;
           }
           if(cur->t < time) {
                 // Run the callback
                 if(cur->c(time))
                 {
                       cur->t = time + cur->n;
                 }
           }
     }
}

// Return a timer pointer so we can delete it
struct TIMER * add_timer(Uint16 increment, unsigned short (*func)())
{
     struct TIMER *cur, *temp;  // Use the temp pointer and arrangement to
                                // prevent threading problems
     
     // Create the structure
     temp = malloc(sizeof(struct TIMER));
     temp->next = NULL;
     
     // Populate it
     temp->t = SDL_GetTicks();   // Current time
     temp->n = increment;
     temp->c = func;
     
     // Loop until we hit the end
     if(timer != NULL)
     {
          for(cur = timer; cur->next != NULL; cur = cur->next) ; 
          // Set it to the temp struct
          cur->next = temp;
          cur->next->prev = cur;
     }   
     else
     {
          timer = temp;
          timer->prev = NULL;
     }
          
     return temp;
}

// Return -1 on failure, 0 on success
unsigned short remove_timer(struct TIMER *kill)
{
     struct TIMER *cur;
     
     // Loop until we hit kill
     for(cur = timer; cur != kill; cur = cur->next) if(cur == NULL) return -1;
  
     // Change the links
     cur->prev->next = cur->next;
     cur->next->prev = cur->prev;
     if(cur == timer) timer = cur->next;
     
     // Set the structure to 0 for safety     
     cur->t = 0;
     cur->n = 0;
     cur->c = 0;
     
     // Free it
     free(cur);
     
     return 0;
}
     
