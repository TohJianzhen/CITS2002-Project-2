#include "mergetars.h"

// tar -cvf [CREATE NEW TAR FILE]
// tar -xvf [EXPAND THE FILE]
// tar -tvf [TO LIST FILE]


//DATA STRUCTURE DEFINE IN GLOBALS?

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
void add_files(char *file){
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
            if(strcmp(files[i].file, file) == 0){
                duplicate = true;
                int prevlastmodified = files[i].lastmodified;
                int prevbytes = files[i].bytes;
                                                                                
                if(prevlastmodified > newlastmodified){
                    return;
                }
                                                                                
                if(prevlastmodified < newlastmodified){
                files[i].file = strdup(file);
                //files[i].tempdir = strdup(dirname);
                files[i].lastmodified = newlastmodified;
                files[i].bytes = newbytes;
                }
                                                                                
                else{
                    if(prevbytes > newbytes){
                        return;
                    }
                    else{
                        files[i].file = strdup(file);
                        //files[i].tempdir = strdup(dirname);
                        files[i].lastmodified = newlastmodified;
                        files[i].bytes = newbytes;
                    }
                }
            }
        }
        if(duplicate == false){
            files = realloc(files, (nfiles+1) * sizeof(files[0]));
            files[nfiles].file = strdup(file);
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
    
    while((pDirent = readdir(pDir)) != NULL){
        char *filename = pDirent->d_name;

        if(dirname == NULL){
            printf("cannot find file with name[%s]\n", filename);
            exit(EXIT_FAILURE);
        }
        
        if( (strcmp(filename, ".") == 0 || (strcmp(filename, "..")) == 0 || (*pDirent->d_name) == '.' )){
        }
    
        else{
            strcat(dirname, "/");
            strcat(dirname, filename);
            printf("%s\n", dirname);
            
            stat(dirname, &stat_buffer);
            
            if(S_ISREG(stat_buffer.st_mode)){
                printf ("[%s] is a file \n", filename);
                add_files(dirname);
            }

            else if(S_ISDIR(stat_buffer.st_mode)){
                printf ("[%s] is another directory \n", filename);
                find_files(dirname);
            }
            //Delete this
            else{ printf("[%s] failed...", filename);}
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
char *copy_files(){
    char *ptr = "test";
    return ptr;
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
    exit(EXIT_SUCCESS);

}


