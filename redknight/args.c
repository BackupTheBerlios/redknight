#include "args.h"
#include "elbot.h"

/*Macros*/
//Make this a little easier on the eyes.
//These are used inside the function, never for function use.
//This way, we can easily clean up all the loose ends.
#define INCREMENT_ARGS() *argv++; i++
#define IS_ARG args=1

int i=0;
unsigned short int verbose=0;//Verbose mode


int process_args(int argc, const char **argv)
{
  int args=0;
    
  //strcpy(inname,argv[0]);
  sprintf(inname,"%s.dat",argv[0]);

  //Process args -->Someday, set this in a separate source file and set it up so
  //that any and all parameters may be changed
  for (i=0; i < argc; *argv++) {
    args=0;//Re-set this
    if(!strcmp(*argv,"--help") || !strcmp(*argv,"-H")) {
      IS_ARG;
      printf("Usage: ELbot [arguments]\n\n");
      printf("  -D --debug\t\tSet the debug level to the next argument.\n\t\t\t Possibilities are: 0 (None), 1, 2, 3(High).\n");
      printf("  -I --input [filename]\tSet the input file to [filename]\n");
      printf("  -H --help\t\tDisplay this information\n");
      printf("  -V --version\t\tDisplay version information\n");
      printf("  -v --verbose\t\tDisplay the info to log whenever log_data()\n\t\t\t and/or log_error() are called.\n");
      exit (0);
    }
    if(!strcmp(*argv,"-V") || !strcmp(*argv,"--version"))
    {
      printf("Version: ELbot %d.%d-%d\n", version_major, version_minor, patch_version);
      exit(0);
    }
    if((!strcmp(*argv,"--debug")) || (!strcmp(*argv,"-D")))
    {
        IS_ARG;
        INCREMENT_ARGS();
        switch(**argv) {
            case '0': debug=DEBUG_NONE;
                break;
            case '1': debug=DEBUG_LOW;
                break;
            case '2': debug=DEBUG_MEDIUM;
                break;
            case '3': debug=DEBUG_HIGH;
                break;
            default: printf("Unknown debug level: %s.\n Please use one of the following levels:0, 1, 2 or 3\n", *argv);
                exit(0);
        }    
    }          
    if((!strcmp(*argv,"--input")) || (!strcmp(*argv,"-I")))
    {
        IS_ARG;
        INCREMENT_ARGS();
        strcpy(fname, *argv);//Copy next argument
    }    
    if((!strcmp(*argv,"-v")) || (!strcmp(*argv,"--verbose")))
    {
        IS_ARG;
        verbose=1;//Not set up yet
    }
    if((!strcmp(*argv,"-f")) || (!strcmp(*argv,"--fortune")))
    {
        args = 2;
        txt2bin("fortune");
    }          
    //End processing...advance counter and check if it's a real value
    if(args == 0 && i > 0)
    {
      printf("Unknown argument: %s", *argv);
      return 0;
    }
    if(args == 2)return 0;
    //Increment i...VERY IMPORTANT
    i++;    
  }
  //End Arg Processing
  return 1;
}  

