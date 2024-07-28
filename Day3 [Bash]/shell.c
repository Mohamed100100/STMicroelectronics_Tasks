#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "command.h"

// the command is an array to recieve the command 
char command[COMMAND_LENGTH] = {'\0'};

// this is an array of pointers to characters, and used to store the command after be separated
char *token[OPTION_NUMBER];

int main()
{
	// iterator to loop on number of given tokens
	int i = 0;
	
	//arg_num will have the number of the given tokens
	int arg_num = 0;
	
	while(1)
	{
		write(STDOUT, "BASHA MASR :) >> ",strlen("BASHA MASR :) >> "));
		//read the command from the user
		read(STDIN, command, COMMAND_LENGTH);
		
		/* this section is used to remove the newline from the command */
		char *ptr = index(command, '\n');
		*ptr = '\0';
		
		/* this section is used to separate the command into tokens */
		i = 0;
		token[i] = strtok(command," ");
		while(token[i]!=NULL)
		{
			token[++i] = strtok(NULL," ");
			
		}
		
		// to handle the case of user enter one token only
		if(i == 0){i++;}
		
		//assign the value of final number of given tokens
		arg_num = i;
		
		//if the user doesn't insert any thing(Press Enter) >> nothing will happen
		if(token[0] != NULL)
		{
			vdExcCommand(token,arg_num);
			memset(command, '\0', COMMAND_LENGTH);	
		}
			else{
			//nothing (Press Enter)
			}	
	}
}


