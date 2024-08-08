#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


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
		
		const char *red   = "\033[31m";
		const char *blue  = "\033[34m";
		const char *green = "\033[35m";
		const char *reset = "\033[0m" ;
		
	while(1)
	{
		
		write(STDOUT,red,strlen(red));
		write(STDOUT,getenv("USER"),strlen(getenv("USER")));
		write(STDOUT,":",strlen(":"));
		write(STDOUT,blue,strlen(blue));
		write(STDOUT,getenv("PWD"),strlen(getenv("PWD")));
		write(STDOUT,green,strlen(green));
		write(STDOUT, " << BASHA MASR :) >> ",strlen(" << BASHA MASR :) >> "));
		write(STDOUT,reset,strlen(reset));

		//read the command from the user
		read(STDIN, command, COMMAND_LENGTH);
				
		/* this section is used to remove the newline '\n' from the command */
		ptr = index(command, '\n');
		if(ptr != NULL)
		*ptr = '\0';
				
		i = 0;
		//allocate only one token 		
		token = NULL;
		while(token == NULL)
                	{token  = (char **) malloc(sizeof(char *));}
	
	
		/* this section is used to separate the command into tokens */
		ptr = strtok(command," ");
		token[0] = NULL;
		while(ptr!=NULL)
		{
			while(token[i] == NULL)
	               		{token[i]  = (char *) malloc(TOKEN_LENGTH);}
			
			//copy the content of the ptr to token[i]			
			strcpy(token[i++],ptr);
			
			//allocate the next token
			temp  = NULL;                    
                        while( temp == NULL )
                        	{temp =(char **)realloc(token, (i+2)*sizeof(char *));}
			token = temp;
			
			//assign the new location to NULL
			token[i] = NULL;
			
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
			//check of piping
			//this if detect if the command has pipe or not
		        //if there pipe >> execute two commands 
		        //at the first we check at two conditions ( " | ls ") OR (" ls | ") one of the two commands are messing
			if(!((strcmp(token[arg_num-1], "|")==0)||(strcmp(token[0], "|")==0)))
        		{
     				//this variable count the number of "|" on the command
        			int pipcountr = 0;
        			
        			//this varible get the index of the "|" on the command
        			int pipelocat = 0;
               			for(int i = 0;i<arg_num;i++)
               			{
               				if(strcmp(token[i], "|")==0)
               				{	
               					pipcountr++;
						pipelocat = i;
					}               			
               			}
               			//incase of one pipe >> then start execute
               			if(pipcountr == 1)
               			{
               				//create the pipe
               				int pipeFd[2];
               				int ret = pipe(pipeFd);
               				
               				//first command will change the output to the pipe
               				close(1);
                                        dup2(pipeFd[1],1);
                                        close(pipeFd[1]);
                                        
                                        //put NULL at the "|" place to separate two commands
                                        token[pipelocat] = NULL;
                                        //execute first command
                                        vdExcCommand(token,pipelocat);
                                        
                                        //after that start to move the second command to be at the first 
                                        for(int i = 1;i<=arg_num-pipelocat;i++)
                                        {
                                        	token[i-1] = token[pipelocat+i];
                                        	token[pipelocat+i] = NULL;
					}
					
					//second command will change the input to the pipe
                                        close(0);
                                        dup2(pipeFd[0],0);
                                        close(pipeFd[0]);
                                       
                                       	//execute first command
                                        vdExcCommand(token,arg_num-pipelocat-1);
                                }
               			else{
               				//there is no piping
               				if(pipcountr == 0)
               				{
						//Execute the normal command 	
						vdExcCommand(token,arg_num);
					}
					//there is too much pipe "|"
					else{
               				write(STDERR, "error : too much pipe\n",strlen("error : too much pipe\n"));
               				write(STDERR, "you can write \"myhelp\" command for help with Commands\n",strlen("you can write \"myhelpg\" command for help with commands\n"));
               				}
               			}
       			}
        		else{
        		//incase of :-
        		//( " | ls ") OR (" ls | ") one of the two commands are messing
        		write(STDERR, "\nerror : Unsupported Command\n",strlen("error : Unsupported Command\n\n"));
        write(STDERR, "you can write \"myhelp\" command for help with Commands\n\n",strlen("you can write \"myhelpg\" command for help with commands\n"));
        		}	
			
			
			
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


