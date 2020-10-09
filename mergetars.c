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
// Do i remove this part? ^

/***********************************************************************************
 *  This function adds a new directory with the TEMPLATE in the mergetars header file
 *  This function does not take any arguments
 ***********************************************************************************/
void add_directory()
{
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
    //  Reallocates memory for the new directory
    dirnames = realloc(dirnames, (ndirnames + 1) * sizeof(dirnames[0]));
    dirnames[ndirnames].dirname = strdup(newdirname);
    ndirnames++;
}

/***********************************************************************************
 *  This function adds files to the temporary directory
 *  A file array with char type is taken as an argument
 *  TO DO : blm di check
 ***********************************************************************************/
void add_files(char file[])
{
    char *filename = strdup(file + lenTemplate);
    struct stat stat_buffer;

    //  Checks if the files contents are valid
    //  Can we 'stat' the file's attributes?
    if(stat(file, &stat_buffer) != 0)
    {
        perror("Error: The file entered is not valid." );
        exit(EXIT_FAILURE);
    }
    else
    {
        int newbytes = (int) stat_buffer.st_size;
        int newlastmodified = (int) stat_buffer.st_mtime;
        bool duplicate = false;

        /**
         * Iterate through the files to :
         * FIRST    : Check for duplicate files
         * SECOND   : Check for the latest file
         * THIRD    : Check for the larger file
         **/
        for(int i = 0; i < nfiles; i++)
        {

        //  FIRST
            if(strcmp(files[i].name, filename) == 0)
            {
                duplicate = true;
                int prevlastmodified = files[i].lastmodified;
                int prevbytes = files[i].bytes;

        //  SECOND
                if(prevlastmodified > newlastmodified)
                {
                    return;
                }

                if(prevlastmodified < newlastmodified)
                {
                    files[i].name = strdup(filename);
                    files[i].fullpath = strdup(file);
                    //files[i].tempdir = strdup(dirname);
                    files[i].lastmodified = newlastmodified;
                    files[i].bytes = newbytes;
                }

        //  THIRD
                else
                {
                    if(prevbytes > newbytes)
                    {
                        return;
                    }
                    else
                    {
                        files[i].name = strdup(filename);
                        files[i].fullpath = strdup(file);
                        //files[i].tempdir = strdup(dirname);
                        files[i].lastmodified = newlastmodified;
                        files[i].bytes = newbytes;
                    }
                }
            }
        }

        //  Reallocate memory for the new files
        if(duplicate == false)
        {
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

/***********************************************************************************
 *  This function finds files in a directory
 *  A directory name is taken as an argument
 *  TO DO : blm di check
 ***********************************************************************************/
void find_files(char dirname[])
{

    struct stat  stat_buffer;
    struct dirent *pDirent;
    DIR *pDir;

    pDir = opendir(dirname);

    //  Null pointer check
    if(pDir == NULL)
    {
        perror("Error: Directory Object not valid");
        exit(EXIT_FAILURE);
    }

    printf("the current dirname is %s\n", dirname);
    printf("\n");

    while((pDirent = readdir(pDir)) != NULL)
    {
        char *filename = pDirent->d_name;

        //  Debugging to show which file it failed at
        if( (strcmp(filename, ".") == 0 || (strcmp(filename, "..")) == 0 || (*pDirent->d_name) == '.' )){}

        else
        {
            //  Allocating memory to store the current path
            char *path = strdup(dirname);
            path = realloc(path, strlen(path) + strlen(filename) + 2);
            strcat(path, "/");
            strcat(path, filename);
            printf("the path is %s\n", path);
            stat(path, &stat_buffer);

            //  Checks if it's a REGULAR file
            //  Uses add_files() to add files to a directory
            if(S_ISREG(stat_buffer.st_mode))
            {
                printf ("[%s] is a file \n", filename);
                printf("The path is %s\n", path);
                printf("The dirname is %s\n", dirname);
                printf("\n");
                add_files(path);
            }

            //  Check if it's a DIRECTORY
            //  Uses find_files() to find the files in the directory
            else if(S_ISDIR(stat_buffer.st_mode))
            {
                printf ("[%s] is another directory \n", filename);
                printf("The path is %s\n", path);
                printf("The dirname is %s\n", dirname);
                printf("\n");
                find_files(path);
            }

            //  Debugging: Checks which files failed
            else{ printf("[%s] failed...\n", filename);printf("\n");}
            free(path);
        }
    }
    closedir(pDir);
}

/***********************************************************************************
 *  This function extracts the given files/directory and add it to a given directory
 *  A file name and a directory name are provided as arguments
 *  TO DO : blm di check
 ***********************************************************************************/
int expand_tar(char filename[], char dirname[])
{

    //  Create a temporary directory to expand the tar
    //  tar -xvf trialtar.tar
    int extract;

    extract = execlp("tar", "tar", "-C", dirname, "-xvf", filename, NULL);
    // OR execl("/usr/bin/tar","tar","-xvf","./testtar/trialtar.tar", NULL);
    return extract;
}

/**********************************************************************************************
 *  This function finds all the file stored in the Data Structure and stores it in a directory
 *  This function does not take any arguments
 *  TO DO : hrs di kerjain... :(
 *  Not sure if it works tho
 **********************************************************************************************/
char *copy_files()
{
    char *dir_name = mkdtemp(TEMPLATE);

    //  Iterates through all the files in the DS
    //  Allocates memory in the directory to store the files
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

/**********************************************************************************************
 *  This function removes a directory using execl
 *  This function takes a directory name as an argument
 **********************************************************************************************/
void remove_dir(char dirname[])
{
    int exit;

    //  Use execl to remove the dirname
    exit = execlp("rm", "rm", "-rf", dirname, NULL);
    perror("Error from remove dir");
}

/**********************************************************************************************
 *  This function removes a directory using execl
 *  This function takes a directory name as an argument
 *  TO DO : blm di check
 **********************************************************************************************/
int create_tar(char filename[])
{

    //copy files from the temp directory into the output file
    //create tar using execl
    int create;
    char *tempdir = copy_files();

    //I cant think of a better way to check if its tgz or tar
    char *file = filename;
    char *tar = ".tar";
    int len = strlen(file);
    const char *last_four = &file[len-4];

    if(strcmp(last_four, tar) == 0)
    {
        create = execlp("tar", "tar", "-cvf", filename, *tempdir, NULL);
    }

    else
    {
        create = execlp("tar", "tar", "-cvf", filename, *tempdir, NULL);
    }

        perror("Error from create tar");
   //will not work though cause after execlp
    remove_dir(tempdir);
    return 0;
}

/**********************************************************************************************
 *  This function cleans up the temporary inputs used
 *  This function does not take an argument
 *  TO DO : blm di check
 **********************************************************************************************/
void cleanup_inputs()
{
    for(int i = 0; i < ndirnames; i++)
    {
        int pid;
        int status;
        pid = fork();
        if(pid == 0)
        {
            remove_dir(dirnames[i].dirname);
        }
        else
        {
            wait(&status);
            
            //  Checks if the child process has been terminated
             if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
             {
                perror("Error: Child process has not been terminated");
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
