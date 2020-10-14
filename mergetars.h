#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <sys/wait.h>

#include  <sys/types.h>                                                         
#include  <sys/stat.h> 

//DEFINE ANY CONSTANTS
#define TEMPLATE "/tmp/mt-XXXXXX"
#define lenTemplate strlen(TEMPLATE) 



// Declare Gloal variables
struct Directory{
	char *dirname;
};
extern struct Directory *dirnames;                                                                         

extern int ndirnames;
                                                                          
struct File{
	
	char *name;
	char *fullname;                                                                 
    	char *fullpath;                                                             
    	char *dirname;                                                              
    	int lastmodified;                                                           
    	int bytes;
}; 
extern struct File *files;

extern int nfiles; 

