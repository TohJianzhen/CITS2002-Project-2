#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include  <sys/types.h>                                                         
#include  <sys/stat.h>                                                          
#include  <time.h>  
#include <stdbool.h>  

#define TEMPLATE        "/tmp/mt-XXXXXX"

struct{                                                                         
    char *dirname;                                                              
} *dirnames = NULL;                                                             
                                                                                
int ndirnames = 0;                                                              
                                                                                
struct{                                                                         
    char *file;                                                                 
    char *tempdir;                                                              
    int lastmodified;                                                           
    int bytes;                                                                  
} *files = NULL;                                                                
                                                                                
int nfiles = 0;


void add_files(char *file, char dirname[]){                                     
    struct stat stat_buffer;                                                    
                                                                                
    if(stat(file, &stat_buffer) != 0) { // can we 'stat' the file's attributes? {
        perror("Error " );                                                      
        exit(EXIT_FAILURE);                                                     
    }                                                                           
    // dont really need the else so just delete?                                
    else{                                                                       
                                                                                
        int newbytes = (int) stat_buffer.st_size;                                      
        int newlastmodified = (int) stat_buffer.st_mtime;                       
        bool duplicate = false;                                                 
                                                                                
        //check for duplicates                                                  
        for(int i = 0; i < nfiles; i++){                                        
            if(strcmp(files[i].file, file) == 0){                               
                duplicate = true;                                               
                int prevlastmodified = files[i].lastmodified;                   
                int prevbytes = files[i].bytes;                                 
                                                                                
                if(prevlastmodified > newlastmodified){                         
                    return;                                                     
                }                                                               
                                                                                
                if(prevlastmodified < newlastmodified){                         
                files[i].file = strdup(file);                                   
                files[i].tempdir = strdup(dirname);                             
                files[i].lastmodified = newlastmodified;                        
                files[i].bytes = newbytes;                                      
                }                                                               
                                                                                
                else{                                                           
                    if(prevbytes > newbytes){                                   
                        return;                                                 
                    }                                                           
                    else{                                                       
                        files[i].file = strdup(file);                           
                        files[i].tempdir = strdup(dirname);                     
                        files[i].lastmodified = newlastmodified;                
                        files[i].bytes = newbytes;                              
                    }                                                           
                }                                                               
            }                                                                   
        }
        if(duplicate == false){                                                 
            files = realloc(files, (nfiles+1) * sizeof(files[0]));              
            files[nfiles].file = strdup(file);                                  
            files[nfiles].tempdir = strdup(dirname);                            
            files[nfiles].lastmodified = newlastmodified;                       
            files[nfiles].bytes = newbytes;                                     
            nfiles++;                                                           
        }                                                                       
    }                                                                           
}                                                                               
   
void add_directory(){

    char newdirname[100];

    strcpy(newdirname, TEMPLATE);
    mkdtemp(newdirname);
    dirnames = realloc(dirnames, (ndirnames + 1) * sizeof(dirnames[0]));
    dirnames[ndirnames].dirname = strdup(newdirname);
    ndirnames++; 

}

void expand_tar(char filename[], char dirname[]){                                                
                                                                                
    //Create a temporary directory to expand the tar                            
    // tar -xvf trialtar.tar                                                    
    int extract;                                                                
    extract = execlp("tar", "tar", "-C", dirname, "-xvf", filename, NULL);   
    printf("WILL THIS PRINT??\n ");
// OR execl("/usr/bin/tar","tar","-xvf","./testtar/trialtar.tar", NULL);    
      
    /**                                                                          
        perror("Failed to extract file");                                       
        exit(EXIT_FAILURE);                                                     
    **/                                                                         
                                                                                
    //find_files(newdirname);                                                     
}                                                                               
  
void remove_dir(char dirname[]){                                                
    //use execl to remove the dirname                                           
    int exit; 
    exit = execlp("rm", "rm", "-rf", dirname, NULL);                                                            
//        perror("Error ");                                                       
//        exit(EXIT_FAILURE);                         
                                                                        
}                                                                               

void cleanup_inputs(){                                                          
    for(int i = 0; i < ndirnames; i++){                                         
        printf("dir removed are %s\n", dirnames[i].dirname);
        int branch = fork();
        if(branch == 0){ 
            remove_dir(dirnames[i].dirname);                                        
        }
        else{
            wait(&branch);
        }
    }                                                      
}                                                                               
     
   
int main(int argc, char *argv[]){
/**    
    for(int i = 0; i < argc; i++){                                         
        printf("dir removed are %s/n", argv[i]);                    
        int branch = fork();                                                    
        if(branch == 0){                                                        
            remove_dir(argv[i]);                                    
        }                                                                       
        else{                                                                   
            wait(&branch);                                                      
        }                                                                       
    }
**/     
    
    for(int i = 1; i < argc-1; i++){                                            
        add_directory();
        
        char  *newdirname = dirnames[ndirnames-1].dirname;
        printf("The tar will be in: %s\n", newdirname); 
        
        int branch = fork();

        if(branch == 0){
            printf("The file %s is untar\n", argv[i]);
            expand_tar(argv[i], newdirname);
            addfiles()  
      }
        else{
            wait(&branch);
       }
    }
    
    for(int i = 0; i < ndirnames; i++){
        printf("the dirnames : %s \n", dirnames[i].dirname);
    }
    printf("the no of dir is : %d\n", ndirnames);
//    cleanup_inputs(); 
     return 0;
}
