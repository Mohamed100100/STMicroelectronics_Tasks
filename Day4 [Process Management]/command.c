#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "command.h"

//have the environment variables
extern char** environ;


// Create array of string to store the available commands
static char *command_name[COMMAND_NUMBER]={
	    		"mycp",
	    		"myecho",
	    		"mymv",
	    		"exit",
	    		"mypwd",
	    		"myhelp",
			"cd",
			"envir",
			"type",
			"phist"}; 

//This array used to store the firsth char of the exit status of the process
static char Process_satatus[] = {'N','T','S','U'};



//I will use them to implement the function "phist" 
//which needs to store the children data
//so this data should be global variable
int   *ChildId    = NULL;
int   *ChildState = NULL;
char **ChildName  = NULL;

//the index of the child >> if ChildTblIndx = 3 so there are 3 childs are created 
static int ChildTblIndx   = 0;

//Max child number >> the code increases it according to the number of children (dynamicly)
static int MaxlengthChild = 5; 

/**
 * @brief Allocating  dynamic memory for the Table of children
 * This function attempts to help the phist function to allocate memory 
 * @param token : no input
 * @return : no return.
 */

void vdHelperForStat_initFunc(void)
{
	while(ChildId == NULL)
		ChildId    = (int *)   malloc(sizeof(int)*MaxlengthChild);
	while(ChildState == NULL)
		ChildState = (int *)   malloc(sizeof(int)*MaxlengthChild);
	while(ChildName == NULL)
		ChildName  = (char **) malloc(sizeof(char *)*MaxlengthChild);

	ChildName[0] = "gemy";
}

/**
 * @brief Excute the needed command
 *
 * This function attempts to abstract the user on  shell file to use the name of the command.
 * it calls the needed command according to the input user.
 *
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdExcCommand(char **token,int lenth)
{
	// index variable will contain the index of the command excuted in the command_name array
	int index = -1;
	
	// this flag will be true if matched input to the supported commands
	int flag = -1;
	int j = 0;
	for( j = 0;j<COMMAND_NUMBER;j++)
	{
		flag = strcmp(token[0], command_name[j]);
		if(flag == 0)
		{index = j;break;}
	}
	if(index == -1) 
	{
		index = intExcuteExternComm(token);
	}
	
	switch(index)
	{
		case 0 : //copy command call
		vdCopyFunc(token,lenth);break;
		case 1 : //Echo command call
		 vdEchoFunc(token,lenth);break;
		case 2 : //Move command call
		 vdMoveFunc(token,lenth);break;
		case 3 : //Exit command call
		 vdExitFunc(lenth);break;
		case 5 : //Help command call 
		vdHelpFunc(lenth);break;
		case 4 : //Pwd command call
		vdPwdFunc(lenth);break;
		case 6 : //cd command call
                vdCdFunc(token,lenth);break;
		case 7 : //envir command call
                vdPrintEnvFunc(token,lenth);break;
		case 8 : //type command call
                vdTypeFunc(token,lenth);break;
		case 9 : //phist command call
		vidPhistFunc(lenth);break;
		case -2: //External Command
		//do nothing
		break;
	default : // unsupported command 
	write(STDOUT, "error : Unsupported Command\n\n",strlen("error : Unsupported Command\n\n"));
        write(STDOUT, "you can write \"myhelp\" command for help with Internal Commands\n",strlen("you can write \"myhelp\" command for help with internal commands\n"));
	write(STDOUT, "you can write \"help\" command for help with External Commands\n",strlen("you can write \"help\" command for help with internal commands\n"));
	break;
	}
}	



/**
 * @brief Excute External Command
 *
 * This function attempts to Excute the External Command(make it a child process) if the Input command is External 
 * if this command is not external this function won't do anything
 * @param token : this contains array of string (the command separated).
 * @return : integer value refers to if this command is External OR  Unsupported 
 */
static int intExcuteExternComm(char **token)
{
	int ret = -1;
	char *full_path = NULL;
	
	//call this function to  check if the command is External or not supported
	if(intIsExt(token,&full_path) == 1)
	{
		int retPid = fork();
		// this point called fork point at which child is created 
		// from here the code is separated into two codes 
		// one for parent code and "retPid" will equal to childId
		// and another code for created child and "retPid" will equal zero
	
		//parent region
		if (retPid > 0) // if the fork point is created successfully the retPid = +ve value(ChildId) fot the parent
		{ // so this section will be executed by the parent 
			ret = -2;
			
			int status =10;
						
			//any fork ends by wait()
			//the wait is called by the parent and the parent code will stuck untill the all created  children call exit function (end all children processes)
			wait(&status);
			
			//Normal Exit
			if(WIFEXITED(status) == 1)
			{
				ChildState [ChildTblIndx] = EXIT_NORM;
			}
			else{	//Termenation By Signal
				if(WIFSIGNALED(status) == 1)
				{
					ChildState [ChildTblIndx] = EXIT_TERM;
				}
				else
				{
					//Stopped By Signal
					if(WIFSTOPPED(status) == 1)
					{
						ChildState [ChildTblIndx] = EXIT_STOP;
					}
					else
					{	//Undetrmined >> I put it because it may be another kinds of signals
						ChildState [ChildTblIndx] = EXIT_UNDEF;
					}	
				}
			}

			//store the Child Id
			ChildId [ChildTblIndx]    = retPid;
	
			//store the name of the process		
			ChildName[ChildTblIndx] = (char *)malloc(strlen(token[0]));
			strcpy(ChildName[ChildTblIndx], token[0]);

			//Increment the Number of children
			ChildTblIndx++;
		
			// check if the size of dynamic array is full >> allocate larger Space
			if(MaxlengthChild == ChildTblIndx)
			{
				MaxlengthChild = MaxlengthChild + 5;
				 int *temp1  = NULL;
				while( temp1 == NULL )
					temp1 =(int *)realloc(ChildId,MaxlengthChild *sizeof(int));

				
				int *temp2  = NULL;
                                while( temp2 == NULL )
                                        temp2 =(int *) realloc(ChildState,MaxlengthChild *sizeof(int));

				
				char **temp3  = NULL;
                                while( temp3 == NULL )
                                       temp3 =(char **) realloc(ChildName,MaxlengthChild *sizeof(char *));
			
				
				ChildId    = temp1;
				ChildState = temp2;
				ChildName  = temp3;
			}
			else{//nothing
			}
		
		}


		//child region
		else if (retPid == 0)// if the fork point is created successfully the retPID = zero for the child
		{// so this section will be executed by the created child only
			
	
			execve(full_path,token, environ);
			exit(0);
		}
		else
		{
			perror("fork");
		}
	}
	
	//ret = -2 >> External command
	//ret = -1 >> Unsupported
	return (ret);
}


/**
 * @brief Show The info of  Children 
 *
 * This function attempts to display the name and status and id of all created children withen this shell
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return. 
 */
void vidPhistFunc(int lenth)
{	

	if(lenth == 1)
	{ 
		write(STDOUT, "\n The Children data : \n",strlen("\n The Children data : \n"));
		printf(" N >> Normal Exit\t T >> Terminated By Siganl\t S >> Stopped By Signal\t U >> Undetermined");
		for(int i = 0;(i<ChildTblIndx)&&(i<10);i++)
		{
		
		//	write(STDOUT, "\n The Children data : \n",strlen("\n The Children data : \n"));
			printf("\n Child %d \t ID : %d \t Status : %c",i,ChildId[i],Process_satatus[ChildState[i]]);
			printf("\tProcess Named : %s\n",ChildName[i]);
				
		}
		printf("\n");
	}
	else // this prevents that the phist command gets argument
        {
        	write(STDOUT, "phist :error, too much argument\n",strlen("phist :error, too much argument\n"));
        }

}
/**
 * @brief display the supported commands
 * This function attempts to display the supported commands to help the user
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdHelpFunc(int lenth)
{
	if(lenth == 1)
	{		
write(STDOUT, "Supported Internal Commands are :\n",strlen("Supported Enternal Commands are :\n"));
write(STDOUT, "1- mycp   : copy a file to another file\n",strlen("1- mycp   : cfopy a file to another file"));
write(STDOUT, "2- myecho : print massege on the screen\n",strlen("2- myecho : print massege on the scrfeen"));
write(STDOUT, "3- mymv   : move file from directory to another directory\n",strlen("3- mymv   : move file from directory to another directoryf"));
write(STDOUT, "4- myexit : exit from the program\n",strlen("4- myexit : exit from the progradm"));
write(STDOUT, "5- mypwd  : print working directory\n",strlen("5- mypwd  : print working directorfy"));
write(STDOUT, "6- myhelp : show supported commands\n",strlen("6- myhelp : show supported commandbs"));	
write(STDOUT, "7- cd     : change the working directory\n",strlen("7- cd     : change the working directory\n"));
write(STDOUT, "8- envir  : print all  enviornment variables\n",strlen("8- envir  : print all  enviornment variables\n"));
write(STDOUT, "9- type   : show the type of the command [Internal , External , UnSupported]\n",strlen("9- type   : show the type of the command [Internal , External , UnSupported]\n"));
write(STDOUT, "10- phist : list the process with thier status \n",strlen("10- phist : list the process with thier status \n"));


	}
	else // this prevents that the help command gets argument
	{	
	write(STDOUT, "help :error, too much argument\n",strlen("helrp :error, too much argument"));
	}
}

/**
 * @brief exit from the shell program
 * This function attempts to terminate the shell program
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdExitFunc(int lenth)
{
	if(lenth == 1)
        {	
	write(STDOUT, "Good Bye :)\n",strlen("Good Beye :)"));
	_exit(0);	
	}
	else // this prevents that the exit command gets argument
        {
        write(STDOUT, "exit :error, too much argument\n",strlen("helrp :error, too much argument"));
        }
}

/**
 * @brief print the working directory
 * This function attempts to print the current directory
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdPwdFunc(int lenth)
{
        if(lenth == 1)
	{
		char buff[100] = {'\0'};
		//get the current directory and store it in buffer
		getcwd(buff, 100);
		
		//disply this buffer
		write(STDOUT, buff,strlen(buff));
		write(STDOUT, "\n",strlen("\n"));
		}
        else // this prevents that the exit command gets argument
        {
        write(STDOUT, "pwd :error, too much argument\n",strlen("helrp :error, too much argument"));
        }

}

/**
 * @brief display the input
 *
 * This function attempts to print the input from the user on the screen.
 *
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdEchoFunc(char **token,int lenth)
{
	//iterate on the input of the user and print it 
	if(token[1] !=NULL)
	{	
	for(int i = 1;i<lenth;i++)
	{
		write(STDOUT, token[i],strlen(token[i]));
		write(STDOUT, " ",strlen(" "));
	}
	write(STDOUT, "\n",strlen("\n"));
	}
	else//empty echo >> "echo   "
	{
		write(STDOUT, "\n",strlen("\n"));
	}
}

/**
 * @brief copy file from dir to another dir
 *
 * copy a file to another file (cp sourcePath targetPath)
 * Print error if the target file exists
 * if the user provide the target file name then use it, 
 * if the user provide the target path  without file name ,use the  same source file name
 * -a option, to append the source content to the end of the target file 
 *
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdCopyFunc(char **token,int lenth)
{
	//check if we will excute option or not 
	int flag = strcmp(token[1], "-a");
	
	char *srcpath  = NULL;
	char *destpath = NULL;
	// if excute option(append) >> need_arg = 4
	// if not (normal copy)     >> need_arg = 3
	
	int need_arg = 0;
	int mode = 0; 

	if(flag == 0)
	{	//copy with append
		srcpath  = token[2];
		destpath = token[3];
		need_arg = 4;
		mode = O_CREAT|O_WRONLY|O_APPEND ;

	}
	else
	{ 	//Normal copy
		srcpath  = token[1];
                destpath = token[2];
		need_arg = 3;
		mode = O_CREAT|O_WRONLY|O_EXCL;
	}

	if(lenth <= need_arg)
        {
		int file_desc_src = 0;
		file_desc_src = open(srcpath, S_IRUSR|S_IWUSR);
		
		//check on successfull opening or not for src file
		if(file_desc_src == -1) 
		{
			write(STDOUT, "(src path) : ",strlen("(src path) : "));
			perror(token[0]);
		}
		else
		{
			int file_desc_dest = 0;
			file_desc_dest = open(destpath,mode, S_IWUSR|S_IRUSR);
			
			//this condion to avoid "This path is directory"
			//and create file with the same name of the srcfile 
			if(errno == EISDIR)
                        {
            	             char *ptr = rindex(srcpath, '/');
                             strcat(destpath, ptr);
                             file_desc_dest = open(destpath,mode, S_IWUSR|S_IRUSR);
                        }
			
			//check on successfull opening or not for dst file
			if(file_desc_dest==-1)
 		        {
 		        	write(STDOUT, "(dest path) : ",strlen("(dest path) : "));
				perror(token[0]);				
               		}
                	else
                	{
				//if all files open well, start copying	
				int size = 0;
				char buff[100] = {'\0'};
				
				do{
					size = read(file_desc_src, buff, 100);
					write(file_desc_dest, buff, size);
					memset(buff, '\0', 100);
				}while(size == 100);
				
				close(file_desc_src);	
				close(file_desc_dest);						
			}	

		}

	}
}


/**
 * @brief move file from dir to another dir
 *
 * move a file to another place (mv sourcePath targetPath)
 * Print error if the target file exists
 * if the user provide the target file name then use it 
 * if the user provide the target path without file name ,use the same source file name
 * -f option, to force overwriting the target file if exists
 *
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdMoveFunc(char **token,int lenth)
{
	//check if we will excute option or not
	int flag = strcmp(token[1], "-f");
        if(flag == 0)
        {       //force moving
		token[1] = token[2];
		token[2] = token[3];
		vdCopyFunc(token , 3);
		if(errno == EEXIST)
		{
			//if dest file is already exists,remove it
			remove(token[2]);
			//call again
			vdCopyFunc(token , 3);
			write(STDOUT, "But Force Moving\n",strlen("But Force Movindg"));

			
		}
       		remove(token[1]);

        }
        else
        {       //Normal move            
		vdCopyFunc(token , 3);
		if(errno == EEXIST){ 
		//nothing to avoid deleting src file if dest file is already exists
		}
		else 	{remove(token[1]);}

        }

}




/**
 * @brief change the current working directory
 * This function attempts to change the working directory which seen by the bash
 * @param lenth : this contains the number of tokens from the user
 * @param token : this contains array of string (the command separated).
 * @return : no return.
 */
void vdCdFunc(char **token,int lenth)
{
	//In Case of the user Enter cd only it will change the directory to the user directory(HOME env)
	if(lenth == 1)
	{
		token[1] = getenv("HOME");
		lenth++;
	}
	
	//In Case of the user Enter "cd .." >> it will go to the parent directory
	if(token[1] == "..")
        {
        	//get the current working directory and remove from it the last direcrory name
		getcwd(token[1],strlen(token[1]));
		char *ptr = rindex(token[1], strlen(token[1]));
                *ptr = '\0';
        }

	//ensure that the input arguments are two only
	if(lenth == 2)
	{
		//IN Case of the user Enter "cd ." >> same directory
		if(token[1] == ".")
		{
			write(STDOUT, "Current Working Directory : ",strlen("Current Working Directory : "));
                        vdPwdFunc(1);	
		}		
		//InCase the user enter path
		else{	
			int flag = chdir(token[1]);			
			//Incase of the user enter wrong abslute path
			if(flag == -1)
			{
				perror(token[0]);
			}
			//Incase of right abslute path
			else{
				write(STDOUT, "Current Working Directory : ",strlen("Current Working Directory : "));
				vdPwdFunc(1);
			}
		}
	}
	// Incase of more that two tokens 
	else{
		write(STDOUT, "cd : too much argument\n",strlen("cd : too much argument\n"));
	}
}


/**
 * @brief dispaly the evironment variable 
 * This function attempts to dispaly the evironment variable which seen by the current bash
 * @param lenth : this contains the number of tokens from the user
 * @param token : this contains array of string (the command separated).
 * @return : no return.
 */
void vdPrintEnvFunc(char **token ,int lenth)
{

	//Incase the user Enter "envir" only
	if(lenth == 1)
	{
		int i =0 ;
		//loop on the environ array and display it
		while(environ[i] !=NULL)
		{
			printf("%s\n", environ[i++]);
		}
	}else{
		//Incase the user Enter two tokens "envir variable"
		if(lenth == 2)
		{
			int i =0 ;
			int flag = 0;
			int len=0;
			char *ptr = NULL;
			//loop on the environ array and search on the varible(input from user) in it
	                while(environ[i] !=NULL)
        	        {
				//this section get the length of the environment variable name
				ptr = strrchr(environ[i], '=');
				len = ptr-environ[i];
				
				//it will be true in case of equal length {token[1] & input variable} and mathching in the characters}
				if(((strncasecmp(environ[i], token[1], len)) == 0)&&(len == strlen(token[1])))
				{
					//Incase of find it 
					write(STDOUT, environ[i],strlen(environ[i]));
					write(STDOUT,"\n" , strlen("\n"));
					flag = 1;
					break;
				}
				else{
				// nothing >> continue
				}
                	        i++;
                	}
			//In case of the user Enter variable which doesn't exist in environ array 
			if(flag == 0)
			{
				printf("envir : %s doesn't exist in environment variables\n" , token[1]);
			}
			else{
				//nothing 
			}
		}
	 	else //Incase of user Enters more than two argument(token)
        	{
        		write(STDOUT, "envir : too much argument\n",strlen("envir : too much argument\n"));
       		}
	}
}


/**
 * @brief get the type of the command
 * This function attempts to display the type of command {Internal , External , Unsupported} command
 * @param lenth : this contains the number of tokens from the user
 * @param token : this contains array of string (the command separated).
 * @return : no return.
 */
void vdTypeFunc(char **token ,int lenth)
{
        //Incase the user Enter two tokens
        if(lenth == 2)
        {
		int exist = 0;
		int flag = -1;
		//loop on the internal command and compare with input user command{token[1]}
	        int j = 0;
        	for( j = 0;j<COMMAND_NUMBER;j++)
       		 {	
                	flag = strcmp(token[1], command_name[j]);
                	if(flag == 0)
                	{exist = 1;break;}
       		 }
		//In case the command is internal
      		 if(exist == 1)
		 {
		 	write(STDOUT, token[1],strlen(token[1]));
			write(STDOUT, " : Internal Command\n",strlen(" : Internal Command\n"));
		 }
		 //In case the command is not internal
		 else{	
			//I create this array of characters to contain the PATH variable value
			char path_env[300] ={'\0'}; 
			
			//I make copy from the PATH variable to avoid overwrite on it
			strcpy(path_env, getenv("PATH"));
			
			//this flag is used for determine if the command is external or unsupported
			int flag = 0;

			//this array of character is used to contain the tokens from PATH variable(separated by ':')
			char *token_path = NULL;
			token_path = strtok(path_env,":");

			//this array used to contain the full path of the excutable file of the command
			char *full_path = NULL;

			//this array , i used it to make the content of token is const because when i used it direct 
			//the strtoken doesn't work correctly
			char temp[300] = {'\0'};
			
			//loop on all tokens(pathes on the PATH variables)
			while(token_path !=NULL)
               		{
				//copy the content of token string to new variable to make strtok function  work well
				strcpy(temp, token_path);
				
				//add the command name to the path 
				full_path = strcat(temp, "/");	
				full_path = strcat(full_path, token[1]);
			
				//if the file path is exists >> conditon is true
				if((access(full_path, F_OK)) == 0)
				{
					flag = 1;
					break;
				}
				else{}

				//reset all arrays contents to NULL terminator
				memset(token_path,'\0',strlen(token_path));
				memset(temp,'\0',strlen(temp));
				memset(full_path,'\0',strlen(full_path));
				
				//get the next token(path)
				token_path = strtok(NULL,":");
           		}
		//	write(STDOUT,full_path,strlen(full_path));
			//In case the command is external
			if(flag == 1)
			{
				write(STDOUT, token[1],strlen(token[1]));
                                write(STDOUT, " : External Command\n",strlen(": Extuernal Command\n"));
			
			}	 						 		 	
		         //Un supported command	
			else{
			 	write(STDOUT, token[1],strlen(token[1]));
                        	write(STDOUT, " : Unsupported command\n",strlen(": wUnsupported command\n"));
			 }
		 }
	}
	else
	{	// In case the user forgets Enterring the command
		if(lenth == 1)
		{
			write(STDOUT, "type : where is the command!!\n",strlen("type : where is the command!!\n"));
		}
		else //Incase of user Enters more than two argument(token)
                {
                        write(STDOUT, "type : too much argument\n",strlen("type : too much argument\n"));
                }
	}
	
}



/**
 * @brief check the commad (External OR Unsupported)
 * This function attemps to make sure that the input command is external commadn 
 * @param retpath : this pointer will be used to return string of the full path file if the user Enter External Command
 * @param token   : this contains array of string (the command separated).
 * @return : no return.
 */
static int intIsExt(char **token,char **retpath)
{
	int ret = 0;
	//I create this array of characters to contain the PATH variable value
	 char path_env[300] ={'\0'};

        //I make copy from the PATH variable to avoid overwrite on it
        strcpy(path_env, getenv("PATH"));

        //this flag is used for determine if the command is external or unsupported
        int flag = 0;

        //this array of character is used to contain the tokens from PATH variable(separated by ':')
        char *token_path = NULL;
        token_path = strtok(path_env,":");

        //this array used to contain the full path of the excutable file of the command
        char *full_path = NULL;

        //this array , i used it to make the content of token is const because when i used it direct 
        //the strtoken doesn't work correctly
        char temp[300] = {'\0'};

        //loop on all tokens(pathes on the PATH variables)

        while(token_path !=NULL)
        {
        	//copy the content of token string to new variable to make strtok function  work well
                strcpy(temp, token_path);
		
                //add the command name to the path 
                full_path = strcat(temp, "/");
		full_path = strcat(full_path, token[0]);

                //if the file path is exists >> conditon is true
                if((access(full_path, F_OK)) == 0)
                {
                	flag = 1;
                        break;
                }
                        else{}

                //reset all arrays contents to NULL terminator
                memset(token_path,'\0',strlen(token_path));
                memset(temp,'\0',strlen(temp));
                memset(full_path,'\0',strlen(full_path));

                //get the next token(path)
                token_path = strtok(NULL,":");
	}
	if(flag == 1)
        {      
		ret = 1;
		*retpath = full_path;
        }
	return (ret);
}




