// Customization MUST happen here
#include "includes.h"

// Map Path Definitions

/*
Underworld - 98, 171 * 18
IP - 22, 21 * 441
WS - 720, 137 * 1014 
DP - 360, 331 * 1884
*/


// Also declared in pathfinder.h as extern, is you
// need to lengthen the array...
ENTRY_MAP bot_path[4]={
     {98, 171, 18},
     {22, 21, 441},
     {720, 137, 1014},
     {360, 331, 1884},
};


// Here, we check which map we're on...
// Same junk as I wrote for the client:
int check_map(char *name)
{
    if(!strcmp("mapunderworld1.elm", name)) return 0;
    else if(!strcmp("startmap.elm", name)) return 1;
    else if(!strcmp("map2.elm", name)) return 2;
    else if(!strcmp("map3.elm", name)) return 3;
    else if(!strcmp("map5nf.elm", name)) return 1337;                                                                                                            
    else {
         log_error("Map %s shouldn't be here!", name);
         send_raw_text("#beam me");    // Don't have a clue where we are... 
         return -5;                    // Expect to change again soon...
    }
}