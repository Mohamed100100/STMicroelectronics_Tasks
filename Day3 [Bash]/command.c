#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "command.h"

// Create array of string to store the available commands
char *command_name[COMMAND_NUMBER]={
	    		"cp",
	    		"echo",
	    		"mv",
	    		"exit",
	    		"pwd",
	    		"help"}; 

/**
 * @brief Excute the needed command
 *
 * This function attempts to abstract the user to shell file to use the name of the command.
 * it calls the needed command according to the input user.
 *
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user 
 * @return : no return.
 */
void vdExcCommand(char **token,int lenth)
{
	// index variable will contain the index of the command excuted in the command_name array
	int index = 0;
	
	// this flag will be true if matched input to the supported commands
	int flag = -1;
	int j = 0;
	for( j = 0;j<COMMAND_NUMBER;j++)
	{
		flag = strcmp(token[0], command_name[j]);
		if(flag == 0)
		{index = j;break;}
	}
	if(j ==COMMAND_NUMBER) index = -1;
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
	default : // unsupported command 
	write(STDOUT, "error, invalid command\n",strlen("error, invalid command\n"));
        write(STDOUT, "you can write help command for help\n",strlen("you can write help command for help\n"));
	break;
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
write(STDOUT, "Supported commands are :\n",strlen("Supported commands arhe :"));
write(STDOUT, "1- cp   : copy a file to another file\n",strlen("1- cp   : cfopy a file to another file"));
write(STDOUT, "2- echo : print massege on the screen\n",strlen("2- echo : print massege on the scrfeen"));
write(STDOUT, "3- mv   : move file from directory to another directory\n",strlen("3- mv   : move file from directory to another directoryf"));
write(STDOUT, "4- exit : exit from the program\n",strlen("4- exit : exit from the progradm"));
write(STDOUT, "5- pwd  : print working directory\n",strlen("5- pwd  : print working directorfy"));
write(STDOUT, "6- help : show supported commands\n",strlen("6- help : show supported commandbs"));	
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
