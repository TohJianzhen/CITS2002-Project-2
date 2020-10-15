#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include  <sys/stat.h>
#include <time.h>
#include <sys/wait.h>


// CONSTANTS
#define TEMPLATE "/tmp/mt-XXXXXX"
#define lenTemplate strlen(TEMPLATE) 

// DECLARE GLOBAL FUNCTIONS

extern void add_directory();
extern void find_files(char *);
extern void copy_files();
extern int create_tar(char[]);
extern void cleanup_inputs();


// DECLARE GLOBAL VARIABLES
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

