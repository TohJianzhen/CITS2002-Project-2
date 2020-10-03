#include "mergetars.h"
#include <dirent.h> 
// tar -cvf [CREATE NEW TAR FILE]
// tar -xvf [EXPAND THE FILE]
// tar -tvf [TO LIST FILE]

/**
//DATA STRUCTURE DEFINE IN GLOBALS?
struct {
    char *dirname;
} *dirnames = NULL;

int ndirnames = 0;    

struct {
    char *name;
    char *fullpath;
    char *tempdir;
    int lastmodified;
    int bytes;    
} *files = NULL;

int nfiles = 0;

**/
// processess

void add_directory(){
    char newdirname[100];

    strcpy(newdirname, TEMPLATE);
    mkdtemp(newdirname);
   /**
    // if want to use this need *newdirname instead 
    if(newdirname == NULL){
        perror("Error");
        exit(EXIT_FAILURE);
    }
    **/
    dirnames = realloc(dirnames, (ndirnames + 1) * sizeof(dirnames[0]));
    dirnames[ndirnames].dirname = strdup(newdirname);
    ndirnames++; 
}

//TO DO : blm di check
void add_files(char file[]){
    char *filename = strdup(file + lenTemplate);
    
    struct stat stat_buffer;
    if(stat(file, &stat_buffer) != 0) { // can we 'stat' the file's attributes?
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
            if(strcmp(files[i].name, filename) == 0){
                duplicate = true;
                int prevlastmodified = files[i].lastmodified;
                int prevbytes = files[i].bytes;
                                                                                
                if(prevlastmodified > newlastmodified){
                    return;
                }
                                                                                
                if(prevlastmodified < newlastmodified){
                    files[i].name = strdup(filename);
                    files[i].fullpath = strdup(file);
                    //files[i].tempdir = strdup(dirname);
                    files[i].lastmodified = newlastmodified;
                    files[i].bytes = newbytes;
                }
                                                                                
                else{
                    if(prevbytes > newbytes){
                        return;
                    }
                    else{
                        files[i].name = strdup(filename);
                        files[i].fullpath = strdup(file);
                        //files[i].tempdir = strdup(dirname);
                        files[i].lastmodified = newlastmodified;
                        files[i].bytes = newbytes;
                    }
                }
            }
        }
        if(duplicate == false){
            files = realloc(files, (nfiles+1) * sizeof(files[0]));
            files[nfiles].name = strdup(filename);
            files[nfiles].fullpath = strdup(file);
            //files[nfiles].tempdir = strdup(dirname);
            files[nfiles].lastmodified = newlastmodified;
            files[nfiles].bytes = newbytes;
            nfiles++;
        }
    }
}

// TO DO : hrs di kerjain... :(
void find_files(char dirname[]){
    
    //Check if file or directory
    struct stat  stat_buffer;

    struct dirent *pDirent;
    DIR *pDir;
    
    pDir = opendir(dirname);
    if(pDir == NULL){
        perror("Error");
        exit(EXIT_FAILURE);
    }
    printf("the current dirname is %s\n", dirname);
    printf("\n");    
    while((pDirent = readdir(pDir)) != NULL){
        char *filename = pDirent->d_name;
        
        if( (strcmp(filename, ".") == 0 || (strcmp(filename, "..")) == 0 || (*pDirent->d_name) == '.' )){
        }
    
        else{
            char *path = strdup(dirname);  
            path = realloc(path, strlen(path) + strlen(filename) + 2);
            strcat(path, "/");
            strcat(path, filename);
            printf("the path is %s\n", path);   
            stat(path, &stat_buffer);
            
            if(S_ISREG(stat_buffer.st_mode)){
                printf ("[%s] is a file \n", filename);
                printf("The path is %s\n", path);
                printf("The dirname is %s\n", dirname); 
                printf("\n");
                add_files(path);
            }

            else if(S_ISDIR(stat_buffer.st_mode)){
                printf ("[%s] is another directory \n", filename);
                printf("The path is %s\n", path);                               
                printf("The dirname is %s\n", dirname);
                printf("\n");
                find_files(path);
            }
            //Delete this
            else{ printf("[%s] failed...\n", filename);printf("\n");}
            free(path);
        }
    }
    closedir(pDir);
}


int expand_tar(char filename[], char dirname[]){

    //Create a temporary directory to expand the tar 
    // tar -xvf trialtar.tar                                                    
    int extract;                                                                
    
    extract = execlp("tar", "tar", "-C", dirname, "-xvf", filename, NULL);   
    // OR execl("/usr/bin/tar","tar","-xvf","./testtar/trialtar.tar", NULL);
    return extract;
}

// TO DO : hrs di kerjain... :(
// Not sure if it works tho
char *copy_files(){
    char *dir_name = mkdtemp(TEMPLATE);
    for(int i = 0; i < nflies; i++)
    {
        dir_name = opendir(".");
        char *path = strdup(dirname);
        path = malloc(strlen(path) + strlen(filename) + 2);
        path = strcpy(files[i], path);
    }
    closedir(path);
    return dir_name;
}

 
void remove_dir(char dirname[]){
    //use execl to remove the dirname
    int exit;
    exit = execlp("rm", "rm", "-rf", dirname, NULL);
                                                       
    perror("Error from remove dir");                       
}

//TO DO : blm di check 
int create_tar(char filename[]){

    //copy files from the temp directory into the output file
    //create tar using execl
    char *tempdir = copy_files();
    int create;
    
    //I cant think of a better way to check if its tgz or tar
    char *file = filename;
    char *tar = ".tar";
    int len = strlen(file);
    const char *last_four = &file[len-4];

    if(strcmp(last_four, tar) == 0){
        create = execlp("tar", "tar", "-cvf", filename, *tempdir, NULL); 
    }
    
    else{
        create = execlp("tar", "tar", "-cvf", filename, *tempdir, NULL);
    }
    
        perror("Error from create tar");                                                       
   //will not work though cause after execlp 
    remove_dir(tempdir);
    return 0;
}

void cleanup_inputs(){
    for(int i = 0; i < ndirnames; i++){
        int pid;
        int status;
        pid = fork();
        if(pid == 0){
            remove_dir(dirnames[i].dirname);
        }
        else{
            wait(&status);
             if(WIFEXITED(status) && WEXITSTATUS(status) != 0){                  
                perror("Error");                                                
                exit(EXIT_FAILURE);                                             
            }                      
        }
    }
}


int main(int argc, char *argv[]){

    //Check arguments
    // ./mergetars input_tarfile1 [input_tarfile2 ...] output_tarfile
    if(argc < 3){
        printf("Usage: mergetars input_tarfile1 [input_tarfile2 ...] output_tarfile\n");
        exit(EXIT_FAILURE);
    }
    
    for(int i = 1; i < argc-1; i++){
        add_directory();
        char  *newdirname = dirnames[ndirnames-1].dirname;
        int pid; 
        int status;
        pid = fork();
        if(pid == -1){
            perror("Error");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            execlp("tar","tar", "-C", newdirname, "-xvf", argv[i], NULL);        
            printf("Error: ");
            exit(EXIT_FAILURE);                                                        
        }
        else{
            wait(&status);
            if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
                cleanup_inputs();
                perror("Error");
                exit(EXIT_FAILURE);
            }
        }
     }

//    copy_files();
//    create_tar(argv[argc-1]);
//    cleanup_inputs();
    printf("the no of dir is : %d\n", ndirnames);  

    for(int i = 0; i < ndirnames; i++){
        find_files(dirnames[i].dirname);
    }    
    for(int i =0; i< nfiles; i++){                                              
        printf("the files are %s\n", files[i].name);                            
    }
    free(files);
    free(dirnames);
    exit(EXIT_SUCCESS);

}


