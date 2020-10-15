#include "mergetars.h"
#include <sys/time.h>
#include <stdbool.h>

/***********************************************************************************
 *  This function adds a new directory with the TEMPLATE in the mergetars header file
 *  This function does not take any arguments
 ***********************************************************************************/
void add_directory()
{
    char newdirname[1000];

    strcpy(newdirname, TEMPLATE);
    mkdtemp(newdirname);

    //  Reallocates memory for the new directory
    dirnames = realloc(dirnames, (ndirnames + 1) * sizeof(dirnames[0]));
    dirnames[ndirnames].dirname = strdup(newdirname);
    ndirnames++;
}

/***********************************************************************************
 *  This function adds files to the temporary directory
 *  A file array with char type is taken as an argument
 *  TO DO : Needs Checking
 ***********************************************************************************/
void add_files(char file[], char individualname[], char directories[])
{
    char *filename = strdup(file + lenTemplate);
    char* dirname = strdup(directories + lenTemplate);
    struct stat stat_buffer;

    //  Checks if the files contents are valid
    //  Can we 'stat' the file's attributes?
    if(stat(file, &stat_buffer) != 0)
    {
        perror("ERROR: Fail to stat() the file \n");
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
            if(strcmp(files[i].fullname, filename) == 0)
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
                    files[i].name = strdup(individualname);
                    files[i].fullname = strdup(filename);
                    files[i].fullpath = strdup(file);
                    files[i].dirname = strdup(dirname);
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
                        files[i].name = strdup(individualname);
                        files[i].fullname = strdup(filename);
                        files[i].fullpath = strdup(file);
                        files[i].dirname = strdup(dirname);
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
            files[nfiles].name = strdup(individualname);
            files[nfiles].fullname = strdup(filename);
            files[nfiles].fullpath = strdup(file);
            files[nfiles].dirname = strdup(dirname);
            files[nfiles].lastmodified = newlastmodified;
            files[nfiles].bytes = newbytes;
            nfiles++;
        }
    }
}

/***********************************************************************************
 *  This function finds files in a directory
 *  A directory name is taken as an argument
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
        perror("Error: Fail to open directory \n");
        exit(EXIT_FAILURE);
    }

    while((pDirent = readdir(pDir)) != NULL)
    {
        char *filename = pDirent->d_name;

        //  Debugging to show which file it failed at
        if((strcmp(filename, ".") == 0 || (strcmp(filename, "..")) == 0 || (*pDirent->d_name) == '.')){}

        else
        {
            //  Allocating memory to store the current path
            char *path = strdup(dirname);
            path = realloc(path, strlen(path) + strlen(filename) + 2);
            strcat(path, "/");
            strcat(path, filename);
            stat(path, &stat_buffer);

            //  Checks if it's a REGULAR file
            //  Uses add_files() to add files to a directory
            if(S_ISREG(stat_buffer.st_mode))
            {
                add_files(path, filename, dirname);
            }

            //  Check if it's a DIRECTORY
            //  Uses find_files() to find the files in the directory
            else if(S_ISDIR(stat_buffer.st_mode))
            {
                find_files(path);
            }

            //  Debugging: Checks which files failed
            else
            {
                printf("[%s] failed...\n", filename);
                printf("\n");
            }
            free(path);
        }
    }
    closedir(pDir);
}


/**********************************************************************************************
 *  This function recursively creates a folder in order to copy the files into the folder
 *  A path pointer and a mode variable to provide file permission are provided as arguments
 *  This function is void and does not return anything
 **********************************************************************************************/
void recursive_mkdir(char *path, mode_t mode)
{

    char *p = NULL;
    int pathlen = strlen(path) + 1;
    char temp[pathlen];
    struct stat stat_buffer;
    size_t len = strnlen(path, pathlen);

    //  Copies len characters from path to temp
    memcpy(temp, path, len);
    temp[len] = '\0';
    len = strlen(temp);

    if(temp[len-1] == '/')
    {
        temp[len - 1] = 0;
    }

    //  Recursive mkdir
    for(p = temp + 1; *p; p++)
    {
        if(*p == '/')
        {
            *p = 0;
            if(stat(temp, &stat_buffer) != 0)
            {
                if(mkdir(temp, mode) < 0)
                {
                    exit(EXIT_FAILURE);
                }
            }
            *p = '/';
        }
    }

    //  Tests the given path
    if(stat(temp, &stat_buffer) != 0)
        {
        if(mkdir(temp, mode) < 0)
        {
            exit(EXIT_FAILURE);
        }
    }
}

/**********************************************************************************************
 *  This function modifies the time of a given file and updates it to a new file
 *  A old file pointer and a new file pointer with type char is taken as an argument
 *  This function does not return anything as it is a void type
 **********************************************************************************************/
void modify_time(char* oldfile, char *newfile)
{
    struct stat stat_old;

    //  Error check for the old file
    if(stat(oldfile, &stat_old) == -1)
    {
        perror("ERROR: Fail to stat() old file \n");
        exit(EXIT_FAILURE);
    }

    struct timeval new_times[2];
    struct stat stat_new;

    //  Error check for the new file
    if(stat(newfile, &stat_new) == -1)
    {
        perror("ERROR: Fail to stat() new file \n");
        exit(EXIT_FAILURE);
    }

    new_times[0].tv_sec = stat_old.st_mtime;
    new_times[1].tv_sec = stat_old.st_mtime;

    //  Error check for failed modification of time
    if(utimes(newfile, new_times) == -1)
    {
        perror("ERROR: Fail to reset time \n");
        exit(EXIT_FAILURE);
    }
}

/**********************************************************************************************
 *  This function finds all the file stored in the Data Structure and stores it in a directory
 *  This function does not take any arguments
 *  Not sure if it works tho
 **********************************************************************************************/
void copy_files()
{
    //  Setup where the files will be copied to
    add_directory();
    char *newdirname = dirnames[ndirnames-1].dirname;
    DIR *outDir = opendir(newdirname);

    if(outDir == NULL)
    {
        perror("ERROR: Fail to create directory \n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < nfiles; i++)
    {
        //  Open the source file to read
        struct stat stat_buffer;
        char *fullpath = strdup(files[i].fullpath);

        if(stat(fullpath, &stat_buffer) != 0)
        {
            perror("ERROR: Fail to stat() the file \n");
            exit(EXIT_FAILURE);
        }
        int src_fd = open(fullpath, O_RDONLY);

        if(src_fd == -1)
        {
            perror("ERROR: Fail to open the source file \n");
            exit(EXIT_FAILURE);
        }

        //  Create the new file
        char *directories = strdup(newdirname);
        directories = realloc(directories, strlen(directories) + strlen(files[i].dirname) + 1);
        strcat(directories, files[i].dirname);

        mode_t mode = stat_buffer.st_mode;
//        recursive_mkdir(directories,  S_IRWXU);
        recursive_mkdir(directories,  S_IRWXU);

        char *filename = strdup(newdirname);
        filename = realloc(filename, strlen(filename) + strlen(files[i].fullname) + 1);
        strcat(filename, files[i].fullname);
        int out_fd = open(filename, O_CREAT | O_WRONLY, mode);
//        int out_fd = open(filename, O_CREAT | O_WRONLY, 0600);

        //  Error check if the outfile cannot be accessed
        if(out_fd == -1)
        {
            perror("ERROR: Fail to open the outfile \n");
            exit(EXIT_FAILURE);
        }

        //  Read from old file, write to the new file and change the utime
        int bytes = (int) stat_buffer.st_size;

        while(1)
        {
            unsigned char buffer[bytes];
            int err = read(src_fd, buffer, bytes);

            if(err == -1)
            {
                perror("ERROR: Fail to read the old file \n");
                exit(EXIT_FAILURE);
            }
            int n = err;

            if(n == 0) break;
            err  = write(out_fd, buffer, n);

            if(err == -1)
            {
                perror("ERROR: Fail to write in the new file \n");
                exit(EXIT_FAILURE);
            }
        }

        modify_time(fullpath, filename);

        close(src_fd);
        close(out_fd);

        free(directories);
        free(filename);
    }
    closedir(outDir);
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
    perror("Error: Fail to remove the directories \n");
}

/**********************************************************************************************
 *  This function removes a directory using execl
 *  This function takes a directory name as an argument
 **********************************************************************************************/
int create_tar(char filename[])
{

    //  copy files from the temp directory into the output file
    //  create tar using execl
    int create;
    char *tempdir = dirnames[ndirnames-1].dirname;
    char *file = filename;
    char *tar = ".tar";
    int len = strlen(file);
    const char *last_four = &file[len-4];

    int pid;
    int status;
    pid = fork();

    //  Error check if fork fails
    if(pid == -1)
    {
        perror("ERROR: Fail to fork \n");
        exit(EXIT_FAILURE);
    }

    //  Error check if new tar cannot be created
    else if(pid == 0)
    {
        if(strcmp(last_four, tar) == 0)
        {
            create = execlp("tar", "tar", "-cvf", filename, tempdir, NULL);
            perror("ERROR: Fail to create tar \n");
            exit(EXIT_FAILURE);
        }
        else
        {
            create = execlp("tar", "tar", "-czvf", filename, tempdir, NULL);
            perror("ERROR: Fail to create tar \n");
            exit(EXIT_FAILURE);
        }
    }

    //  Error check if exit fails
    else
    {
        wait(&status);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            perror("ERROR: Fail to terminate child process \n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

/**********************************************************************************************
 *  This function cleans up the temporary inputs used
 *  This function does not take an argument
 **********************************************************************************************/
void cleanup_inputs()
{
    for(int i = 0; i < ndirnames; i++)
    {
        char *dirname = dirnames[i].dirname;
        int pid;
        int status;
        pid = fork();

        if(pid == 0)
        {
            execlp("rm", "rm", "-rf", dirname, NULL);
        }
        else
        {
            wait(&status);

            //  Checks if the child process has been terminated
             if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
             {
                perror("Error: Fail to terminate child process \n");
                exit(EXIT_FAILURE);
             }
        }
    }
}

