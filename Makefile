#Makefile to build our 'Project 2' project

PROJECT	= mergetars
HEADERS = $(PROJECT).h
OBJ 	= mergetars.o globals.o processes.o

C99	= cc -std=c99
CFLAGS	= -Wall -pedantic -Werror

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ)

mergetars.o : mergetars.c $(HEADERS)
	$(C99) $(CFLAGS) -c mergetars.c

globals.o : globals.c $(HEADERS)
	$(C99) $(CFLAGS) -c globals.c

processes.o : processes.c $(HEADERS)
	$(C99) $(CFLAGS) -c processes.c

clean: 
	rm -f $(PROJECT) $(OBJ)
