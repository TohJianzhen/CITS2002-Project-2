#include "mergetars.h"

// CITS2002 Project 2 2020
// Names:               Jianzhen Toh, Nathania Chrestella Teruna
// Student numbers :            ,22642853

// tar -cvf [CREATE NEW TAR FILE]
// tar -xvf [EXPAND THE FILE]
// tar -tvf [TO LIST FILE]

int main(int argc, char *argv[])
{

    //Check arguments
    // ./mergetars input_tarfile1 [input_tarfile2 ...] output_tarfile
    if(argc < 3)
    {
        printf("Usage: mergetars input_tarfile1 [input_tarfile2 ...] output_tarfile\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i < argc-1; i++)
    {
        add_directory();
        char  *newdirname = dirnames[ndirnames-1].dirname;
        int pid;
        int status;
        pid = fork();

        if(pid == -1)
        {
            perror("ERROR: Fail to fork  \n");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            execlp("tar","tar", "-C", newdirname, "-xvf", argv[i], NULL);
            printf("ERROR: Fail extract tar file \n");
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(&status);

            if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                cleanup_inputs();
                perror("ERROR: Fail to terminate child process \n");
                exit(EXIT_FAILURE);
            }
        }
     }

    for(int i = 0; i < ndirnames; i++)
    {
        find_files(dirnames[i].dirname);
    }
    
    //Copy the files to a temporary directory
    copy_files();

    //Create the output tar file
    create_tar(argv[argc-1]);

    //Remove the temporary directory created
    cleanup_inputs();
    
    //Free the structures    
    free(files);
    free(dirnames);
    exit(EXIT_SUCCESS);

}
