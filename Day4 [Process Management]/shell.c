#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "command.h"


// the command is an array to recieve the command 
char command[COMMAND_LENGTH] = {'\0'};

// this is an array of pointers to characters, and used to store the command after be separated
//char *token[OPTION_NUMBER];



int main()
{
	//this pointer will contain the array of string(tokens)
	char **token = NULL;
	
	//we will use this pointer for rellocation the memory on the heap
	char **temp  = NULL;
	
	char *ptr =NULL;
	
	// iterator to loop on number of given tokens
	int i = 0;
	
	//arg_num will have the number of the given tokens
	int arg_num = 0;

	//this function is helper for "phist" command to allocate dynamic memory for command's table 
	vdHelperForStat_initFunc();	
		
	while(1)
	{
		write(STDOUT, "BASHA MASR :) >> ",strlen("BASHA MASR :) >> "));

		//read the command from the user
		read(STDIN, command, COMMAND_LENGTH);
				
		/* this section is used to remove the newline '\n' from the command */
		ptr = index(command, '\n');
		*ptr = '\0';
				
		i = 0;
		//allocate only one token 		
		token = NULL;
		while(token == NULL)
                	{token  = (char **) malloc(sizeof(char *));}
	
	
		/* this section is used to separate the command into tokens */
		ptr = strtok(command," ");
		while(ptr!=NULL)
		{	
			token[i] = NULL;
			while(token[i] == NULL)
	               		{token[i]  = (char *) malloc(TOKEN_LENGTH);}
			
			//copy the content of the ptr to token[i]			
			strcpy(token[i++],ptr);
			
			//allocate the next token
			temp  = NULL;                    
                        while( temp == NULL )
                        	{temp =(char **)realloc(token, (i+2)*sizeof(char *));}
			token = temp;
			
			//get the next token from the command on the heap
			ptr = strtok(NULL," "); 
		}
		
		// to handle the case of user enter one token only
		if(i == 0){i++;}
		
		//assign the value of final number of given tokens
		arg_num = i;
		
		//if the user doesn't insert any thing(Press Enter) >> nothing will happen
		if(token[0] != NULL)
		{
			//Execute the command 	
			vdExcCommand(token,arg_num);
			
			//set all characters of the command with '\0'
			memset(command, '\0', COMMAND_LENGTH);
			
			//this section is used to free the memory in the heap
			for(int j = 0;j<arg_num;j++)
			{	
				free(token[j]);
			}
			free(token);
		}
			else{
			//nothing (Press Enter)
			}	
	}
}


