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



int main()
{
	//this pointer will contain the array of string(tokens)
	char **token = NULL;
	
	
	char *ptr =NULL;
	
	// iterator to loop on number of given tokens
	int i = 0;
	
	//arg_num will have the number of the given tokens
	int arg_num = 0;
	
		
		const char *red   = "\033[31m";
		const char *blue  = "\033[34m";
		const char *green = "\033[35m";
		const char *reset = "\033[0m" ;
		
	while(1)
	{
		// this section is just for decoration and more organization		
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
				
		// the dynamic memory allocation of the tokens
		// i start to allocate one token >> only one string >> only one array of char
		// next i use strtok funcion to know if there is another token 
		// if there is , i will allocate another array of char to the next token and so on 
		// i put extra token and make it equal NULL
		// ex  >>  if the input command is "cp file1 file2"
		// =====================================================
        	// so      | token[0] | token[1] | token[2] | token[3] |
        	// =====================================================
        	//         [   "cp"   , "file1"  , "file2"  ,   NULL   ]
        	// =====================================================

		i = 0;
		//allocate only one token 		
		token = NULL;
                token  = (char **) malloc(sizeof(char *));
		
		//if there is a problem when allocating memory on the heap
		//print error and exit from the program
		if(token == NULL)
		{
			write(STDERR," error : there is problem while storing command on the heap\n",strlen(" error : there is problem while storing command on the heap\n"));
			exit(0);
		}

	
		/* this section is used to separate the command into tokens */
		ptr = strtok(command," ");
		token[0] = NULL;
		
		while(ptr!=NULL)
		{
			// Icase of there is anew token 
			// we will allocate new array of chars 
			// and copy to it the content of this token
	               	token[i]  = (char *) malloc(strlen(ptr));
			if(token[i] == NULL)
	                {
        	                write(STDERR," error : there is problem while storing command on the heap\n",strlen(" error : there is problem while storing command on the heap\n"));
                	        exit(0);
               		 }

			//copy the content of the ptr to token[i]			
			strcpy(token[i++],ptr);
			

			//allocate the next token			              
                       	token= (char **)realloc(token, (i+2)*sizeof(char *));
			if(token == NULL)
                        {
                                write(STDERR," error : there is problem while storing command on the heap\n",strlen(" error : there is problem while storing command on the heap\n"));
                                exit(0);
                         }
 
			
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
			
		        //after i get the command separated on the token array 
			//first check will b check on the pipe 
			
			//we have many corner cases on the pipes we should avoid it (generally missing one of the command)
		        //for ex >>  ( " | ls ") OR (" ls | ")  there is messing command
			//also   >>  ( " ls | | grep out ") >> error (missing command)
			
			//at the first , i will check on this case ( " ls | | grep out ") 
		        int flag = 1;
		        for(int i = 0;i<arg_num-1;i++)
               		{
               			if((strcmp(token[i], "|")==0)&&(strcmp(token[i+1], "|")==0))
               			{	
               				flag = 0;
               				break;
				}               			
               		}

		        //here is the final check on the above corner cases
			if(!((strcmp(token[arg_num-1], "|")==0)||
			     (strcmp(token[0], "|")==0)||
			     (flag == 0)))
        		{
     				//this variable count the number of "|" on the command
        			int pipcountr = 0;
        			
        			//check of piping
				//this for loop detects if the command has pipe or not
				//if there is pipe >> it will get the number of them
               			for(int i = 0;i<arg_num;i++)
               			{
               				if(strcmp(token[i], "|")==0)
               				{	
               					pipcountr++;
					}               			
               			}
               			
               			//incase of pipe >> then start execute
               			if(pipcountr > 0)
               			{
               				//Execute the pipe command with each length
                                	vdPipeExecFunc(pipcountr ,token ,arg_num);
                                }
               			else{
               				//pipcountr = 0
               				//there is no piping
               				//Execute the normal command 	
					vdExcCommand(token,arg_num);
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



