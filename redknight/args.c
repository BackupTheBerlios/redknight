#include "args.h"
#include "elbot.h"


unsigned short int verbose=0;     // Verbose mode

void getargs(int argc, char **argv)
{    
     for(*argv++; *argv != 0; *argv++) {
           if(*argv[0] == '-')
           { 
                 if(!strcmp(*argv, "-i")) // Set Input
                 {
                       strcpy(fname, *(++argv));
                       continue;
                 }
                 if(!strcmp(*argv, "-d")) // Debug
                 {
                       debug = atoi(*(++argv));
                       if(debug > 3) debug = 3;
                       if(debug < 0) debug = 0;
                       continue;
                 }
                 if(!strcmp(*argv, "-v")) // Verbose
                 {
                       verbose = 1;
                       continue;
                 }
                 if(!strcmp(*argv, "-f")) // Compile Fortune
                 {
                       txt2bin(*(++argv));
                       continue;
                 }
                 if(!strcmp(*argv, "-V") || !strcmp(*argv, "--version")) // Print Version Info
                 {
                       printf("Version: ELbot %d.%d-%d\n", version_major, version_minor, patch_version);
                       exit(0);
                 }
                 if(!strcmp(*argv, "-h") || !strcmp(*argv, "--help")) // Print Help Info
                 {
                       printf("Usage: ELbot [arguments]\n\n");
                       printf("  -d \t\t\tSet the debug level to the next argument.\n\t\t\t Possibilities are: 0 (None), 1, 2, 3(High).\n");
                       printf("  -i [filename]\t\tSet the input file to [filename]\n");
                       printf("  -h --help\t\tDisplay this information\n");
                       printf("  -f [filename]\t\tCompile the fortune file from [filename]\n");
                       printf("  -V --version\t\tDisplay version information\n");
                       printf("  -v \t\t\tDisplay the info to log whenever log_data()\n\t\t\t and/or log_error() are called.\n");
                       exit (0);
                 }
           } 
           else
           { 
                 printf("Unknown argument: %s\nUse redknight --help for more details", *argv);    // This shouldn't happen
                 exit(0);
           }
     }
     printf("Returning ... ");
     return 1;
}          

