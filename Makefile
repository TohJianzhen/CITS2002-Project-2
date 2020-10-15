# Makefile to build our 'Project 2' project                                     

PROJECT = mergetars                                                             
HEADERS = $(PROJECT).h                                                          
OBJ             = file_list.o globals.o mergetars.o processes.o                 
                                                                                
C99     = cc -std=c99                                                           
CFLAGS  = -Wall -pedantic -Werror                                               
                                                                                
$(PROJECTS) : $(OBJ)                                                            
	$(C99) $(CFLAGS) -o $(PROJECT) \ $(OBJ) -lm                                            
                                                                                
globals.o : globals.c $(HEADERS)
	$(C99) $(CFLAGS) -c globals.c                                           
                                                                                
mergetars.o : $(PROJECT).c $(HEADERS)
	$(C99) $(CFLAGS) -c $(PROJECT).c                                        
                                        
