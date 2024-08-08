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

//this struct used for store the local variable data
typedef struct  
{	
	char *loc_name;
	char *loc_value;
}locVar;

//create one pointer to the struct of the local variable data
//loc_num contains the number of the local variable
static int loc_num = 0;
locVar *local_Var_arr = NULL;

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
			"phist",
			"myfree",
			"myuptime",
			"addenv",
			"addloc",
			"allVar"}; 

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
	int i =0;
	int newlen = 0;
	// index variable will contain the index of the command excuted in the command_name array
        int index = -1;

	//this array will contain the default standard of input and output
	char arrstd[100]= {'\0'};
        readlink("/proc/self/fd/2", arrstd, 100);

	int file = 0;
	int var = 0;

	//this if detect if the command has redirection or not
	//if there refirec >> index = -4
	//if not index = -3
	//at the first we check at two conditions ( " > file_name " OR " ls > ") there is no command or there is no file_name
	if(!((strcmp(token[lenth-1], ">")==0)||(strcmp(token[lenth-1], "<")==0)||(strcmp(token[lenth -1], "2>")==0)
	   ||(strcmp(token[0], ">")==0)||(strcmp(token[0], "<")== 0)||(strcmp(token[0], "2>")== 0)))
	{	
		for(i = 0;i<lenth;i++)
		{
			//redirect the output 
			//we check in this case >>ex> " ls > < " >> error
			if((strcmp(token[i], ">")==0)&&(strcmp(token[i+1], "<")!=0)&&(strcmp(token[i+1], "2>")!=0))
			{
				//if we reach to this point so the output redirection is written well
				//remove the redirection from the tokens
				token[i] = NULL;
				//open or ceate the file 
				file = creat(token[i+1], S_IRWXU);
				//if success
				if(file != -1)
				{
					// set the lenth of the command to the new lenth
					// ex >> "ls > output"
					// so the lenth = 3 and the newlen must = 1 after redirection
					if(newlen == 0){newlen = i;}

					//output redirection
					close(1);
       					dup2(file,1);
        				close(file);

					//remove the file from the tokens
					token[++i]=NULL;
					
					//incase of success redirection
					index = -4;//true redirect 
					
					//i put this variable to count the redirection 
					//for ex >> " ls > out1 file1 "
					// it must give error because undetermined behavior
					var = var +2;
					continue;

				}
				else{
					//in case failled to open
					index = -3;
					break;				
				}

			}
			//redirect the error 
                        //we check in this case >>ex> " ls 2> < " >> error
			if((strcmp(token[i], "2>")==0)&&(strcmp(token[i+1], "<")!=0)&&(strcmp(token[i+1], ">")!=0))
			{
				//if we reach to this point so the error redirection command is written well
                                //remove the redirection from the tokens
				token[i] = NULL;
				//open or ceate the file 
                                file = creat(token[i+1],S_IRWXU);
                                //if success
                                if(file != -1)
                                {
					// set the lenth of the command to the new lenth
                                        // ex >> "pwd 2> error"
                                        // so the lenth = 3 and the newlen must = 1 after redirection
                                        if(newlen == 0){newlen = i;}

					//error redirection
                                        close(2);
                                        dup2(file,2);
                                        close(file);

                                        //remove the file from the tokens
					token[++i]=NULL;

                                        //incase of success redirection
                                        index = -4;//true redirect 
					
					//i put this variable to count the redirection
                                        //for ex >> " pwd 2> err1 file1 "
                                        // it must give error because undetermined behavior
                        		var = var +2;
					
					continue;
                                }
                                else{
					//in case failled to open
                                        index = -3;
                                        break;                          
                                }

			}

			//redirect the input
			//we check in this case >>ex> " ls < 2> " >> error
			if((strcmp(token[i], "<")==0)&&(strcmp(token[i+1], "2>")!=0)&&(strcmp(token[i+1], ">")!=0))
			{
				//if we reach to this point so the input redirection command is written well
                                //remove the redirection from the tokens
				token[i] = NULL;
				//open or ceate the file
                                file = open(token[i+1], O_RDONLY,S_IRWXU);
                                //if success
				if(file != -1)
                                {

					// set the lenth of the command to the new lenth
                                        // ex >> "mypwd < input"
                                        // so the lenth = 3 and the newlen must = 1 after redirection
                                        if(newlen == 0){newlen = i;}
						
					//input redirection
                                        close(0);
                                        dup2(file,0);
					close(file);
					
					//remove the file from the tokens
                                        token[++i]=NULL;
					
					//incase of success redirection
                                        index = -4;//true redirect 
                                       
					//i put this variable to count the redirection
                                        //for ex >> " mypwd < input file1 "
                                        // it must give error because undetermined behavior
					var = var +2;
	
					continue;
                                }
                                else{
                                        perror(token[i+1]);
					
					//in case failled to open
					//this return will exit from this function 
					return ;
                                        break;
                                }

			}
		}

	}
	else{
		// in case of no redirection
		index = -3;
	}

	
	// this if use the variable var which created above to capture this error
	// " mypwd > output file "
	// the lenth of this command = 4
	// var variable = 2 (the number of redirection argument (> and output))
	// newlen variable = 1 (it counts before the redirection(mypwd only))
	// so the argument (file) will produce undetremined behavior
	// so our check >> if (newlen + var) == lenth >> the command is written well
	// the second part of the condition(index == -4) ensures that the command has redirection 
	if(((newlen + var) != lenth)&&(index == -4)) index = -3;

	// this condition set the lenth to the newlen
	if (newlen != 0) lenth = newlen;

	
		
	//this if do many things
	//if the index = -3 >> so there is a problem with the above redirection so don't execute the command
	//if the index = -1 so the command doesn't have redirection so the condion is true and execute the command
	//if the index = -4 so the command has successful redirecion and the condion is true and execute the command
	if(index != -3)
	{
		//the first part is searching on internal commands
		//if the command is internal the index var will contain the index of it on the array_command
		int flag = -1;
		int j = 0;
		for( j = 0;j<COMMAND_NUMBER;j++)
		{
			flag = strcmp(token[0], command_name[j]);
			if(flag == 0)
			{index = j;break;}
		}
		//if the index = -1 or -4 >> then it isn't internal command
		if(index < 0) 
		{
			// this function returns -1 incase of the command is not external (unsupported)
			// and also can return -2 if this command is external and in this case it executes it 
			index = intExcuteExternComm(token);
		}
	}
	else{
		// there is a problem
		index = -1;
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
		case 10://free command call
		vdFreeFunc(lenth);break;
		case 11://uptime command call
		vdUptimeFunc(lenth);break;
		case 12://addenv command call
		vdADDEnvVarFunc(token,lenth);break;
		case 13://addloc command call
		vdADDlocalVarFunc(token,lenth);break;
		case 14://allVar command call
		vdPrintALLVarFunc(lenth);break;
		case -2: //External Command
		//do nothing
		break;
	default : // unsupported command 
	write(STDERR, "\nerror : Unsupported Command\n",strlen("error : Unsupported Command\n\n"));
        write(STDERR, "you can write \"myhelp\" command for help with Commands\n\n",strlen("you can write \"myhelpg\" command for help with commands\n"));
	break;
	}

	//this section returns the input and output and error to the terminal again
	
        file = open(arrstd, O_RDWR, S_IRWXU);
        close(1);
        dup2(file,1);
       	
       	close(0);
      	dup2(file,0);
		
	close(2);
        dup2(file,2);
	close(file);
	
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
        	write(STDERR, "phist :error, too much argument\n",strlen("phist :error, too much argument\n"));
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
write(STDOUT, ">>>>>>>>> \" mycp <src path> <dest path> \"\n",strlen(">>>>>>>>>> \" mycp <src path> <dest path> \"\n"));
write(STDOUT, ">>>>>>>>> \" mycp -a <src path> <dest path> \" allow append option\n",strlen(">>>>>>>>> \" mycp -a <src path> <dest path> \" allow append option\n"));
write(STDOUT, "2- myecho : print massege on the screen\n",strlen("2- myecho : print massege on the scrfeen"));
write(STDOUT, ">>>>>>>>> \" myecho <massege>\" \n",strlen(">>>>>>>>> \" myecho <massege>\" \n"));
write(STDOUT, "3- mymv   : move file from directory to another directory\n",strlen("3- mymv   : move file from directory to another directoryf"));
write(STDOUT, ">>>>>>>>> \" mymv <src path> <dest path> \"\n",strlen(">>>>>>>>>> \" mycp <src path> <dest path> \"\n"));
write(STDOUT, ">>>>>>>>> \" mymv -f <src path> <dest path> \" allow force moving option if the file exists\n",strlen(">>>>>>>>>>> \"mycp -a <src path> <dest path> \" allow force moving option if the file exists\n"));

write(STDOUT, "4- exit   : exit from the program\n",strlen("4- myexit : exit from the progradm"));
write(STDOUT, "5- mypwd  : print working directory\n",strlen("5- mypwd  : print working directorfy"));
write(STDOUT, "6- myhelp : show supported commands\n",strlen("6- myhelp : show supported commandbs"));	
write(STDOUT, "7- cd     : change the working directory\n",strlen("7- cd     : change the working directory\n"));
write(STDOUT, "8- envir  : print all  enviornment variables\n",strlen("8- envir  : print all  enviornment variables\n"));
write(STDOUT, "9- type   : show the type of the command [Internal , External , UnSupported]\n",strlen("9- type   : show the type of the command [Internal , External , UnSupported]\n"));
write(STDOUT, "10- phist : list the process with thier status \n",strlen("10- phist : list the process with thier status \n"));
write(STDOUT, "11- free  : print RAM info (total , free , used) and swap info (total , free , used) \n",strlen("11- free  : print RAM info (total , free , used) and swap info (total , free , used) \n"));
write(STDOUT, "12- myuptime : print the uptime on the system and idle time\n",strlen("12- myuptime : print the uptime on the system and idle time\n"));
write(STDOUT, "13- addloc: create new local variable\n",strlen("13- addloc: create new local variable\n"));
write(STDOUT, ">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n",strlen(">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n"));
write(STDOUT, "14- addenv: create new environment variable\n",strlen("13- addloc: create new environment variable\n"));
write(STDOUT, ">>>>>>>>>> \" addenv <Var_Name> = <Var_Value>> \"\n",strlen(">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n"));
write(STDOUT, "15- allVar: print environment variable and local\n",strlen("14- allVar: print environment variable and local\n"));
write(STDOUT, "16- output redirection \" {command}  >  {output_file} \"\n",strlen("15- output redirection \" {command}  >  {output_file} \"\n"));
write(STDOUT, "17- input redirection  \" {command}  <  {input_file} \"\n",strlen("16- input redirection   \" {command}  <  {input_file} \"\n"));
write(STDOUT, "18- error redirection  \" {command} 2>  {error_file} \"\n",strlen("17- error redirection   \" {command} 2>  {error_file} \"\n"));
write(STDOUT, "19- pipe way  \" {command_1} | {command_2} \"\n",strlen("18- pipe way  \" {command_1} | {command_2} \"\n"));

	}
	else // this prevents that the help command gets argument
	{	
	write(STDERR, "help :error, too much argument\n",strlen("helrp :error, too much argument"));
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
        write(STDERR, "exit :error, too much argument\n",strlen("helrp :error, too much argument"));
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
        write(STDERR, "mypwd :error, too much argument\n",strlen("mypwd :error, too much argument"));
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
	if(lenth == 1)
	{
		write(STDERR, "mycp :error, missing src path and dest path\n",strlen("mycp :error, missing src path and dest path\n"));
		return ;
	}
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
			write(STDERR, "(src path) : ",strlen("(src path) : "));
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
				strcat(destpath, "/");
            	        	char *ptr = rindex(srcpath, '/');
			    	if(ptr == NULL) ptr = srcpath;
                           	strcat(destpath, ptr);
                             	file_desc_dest = open(destpath,mode, S_IWUSR|S_IRUSR);
                        }
			
			//check on successfull opening or not for dst file
			if(file_desc_dest==-1)
 		        {
 		        	write(STDERR, "(dest path) : ",strlen("(dest path) : "));
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

	if(lenth == 1)
        {
                write(STDERR, "mymv :error, missing src path and dest path\n",strlen("mycp :error, missing src path and dest path\n"));
                return ;
        }

	//check if we will excute option or not
	int flag = strcmp(token[1], "-f");
        if(flag == 0)
        {       //force moving
		token[1] = token[2];
		token[2] = token[3];
		token[3] = NULL;
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
	
	//the chdir syscall handle many cases :-
	//In Case of the user Enter "cd .." 
	//In Case of the user Enter "cd <abslute path>" 
	//In Case of the user Enter "cd ." 
	//In Case of the user Enter "cd <relative path>" 
	//In Case of the user Enter "cd <directory>"

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
				
				//after i change the current working directory , i start to change the value 
				//of PWD env to the new directory
				vdPwdFunc(1);
				char cwd[100];
				setenv("PWD", getcwd(cwd, 100), 1);

			}
		}
	}
	// Incase of more that two tokens 
	else{
		write(STDERR, "cd : too much argument\n",strlen("cd : too much argument\n"));
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

				write(STDERR, "envir : ",strlen("envir : "));
				write(STDERR, token[1],strlen(token[1]));
				write(STDERR, "doesn't exist in environment variables\n",strlen("doesn't exist in environment variables\n"));
			}
			else{
				//nothing 
			}
		}
	 	else //Incase of user Enters more than two argument(token)
        	{
        		write(STDERR, "envir : too much argument\n",strlen("envir : too much argument\n"));
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
			write(STDERR, "type : where is the command!!\n",strlen("type : where is the command!!\n"));
		}
		else //Incase of user Enters more than two argument(token)
                {
                        write(STDERR, "type : too much argument\n",strlen("type : too much argument\n"));
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



/**
 * @brief display the RAM info
 * This function attemps to print the RAM info(total size,used and free) and Swap area info(total size ,used and free) 
 * @param lenth : this contains the number of tokens from the user
 * @return : no return.
 */
void vdFreeFunc(int lenth)
{
	//Incase the user Enter two tokens
        if(lenth == 1)
        {	//I noticed that the meminfo file contains from lines and each line has 28 characters
		//so i will create array of characters to contain each line
		char line[28] = {'\0'};
		FILE  *file = fopen("/proc/meminfo", "r");
		//create two variable for Mem info
		int memfree = 0;
		int memtot = 0;
		
		//create two variable for Swap info
		int swaptot = 0;
		int swapfree = 0;

		//this pointer will be used to contain the tokens
		char *ptr = NULL;
		
		//now i will start to get the needded info from this file
		//the info about the memory locates at the begining of the file
		
		//get the memtotal value
		fread(line,sizeof(char),28,file);
		//after get the line info i will get the number only "FREE         321323 kb"
		//then i will convert the string to integer value
		ptr = strtok(line," ");
		ptr = strtok(NULL," ");
		memtot = atoi(ptr);

		//get the memfree vlaue
		fread(line,sizeof(char),28,file);
                ptr = strtok(line," ");
                ptr = strtok(NULL," ");
                memfree = atoi(ptr);

		//change the cursor location to the swap info (the number(12) is set according to the file)
		fseek(file, 12*28, SEEK_CUR);

		//get the swaptotal value
		fread(line,sizeof(char),28,file);
                ptr = strtok(line," ");
                ptr = strtok(NULL," ");
		swaptot = atoi(ptr);

		//get the swapfree value
		fread(line,sizeof(char),28,file);
                ptr = strtok(line," ");
                ptr = strtok(NULL," ");
                swapfree = atoi(ptr);

		printf("MEM_Total      :%i\n",memtot);
		printf("MEM_Used       :%i\n",memtot-memfree);
		printf("MEM_Free       :%i\n",memfree);
		printf("=========================\n");
		printf("SWAP_Total     :%i\n",swaptot);
                printf("SWAP_Used      :%i\n",swaptot-swapfree);
                printf("SWAP_Free      :%i\n",swapfree);

	}
	else //Incase of user Enters more than one argument(token)
        {
   	     write(STDERR, "myfree : too much argument\n",strlen("type : too much argument\n"));
        }
}


/**
 * @brief display the time of the system
 * This function attemps to print the uptime and idle time for the system
 * @param lenth : this contains the number of tokens from the user
 * @return : no return.
 */
void vdUptimeFunc(int lenth)
{
        //Incase the user Enter one token
        if(lenth == 1)
        {      	//this array will contain the content of the file
                char content[100] = {'\0'};
                FILE  *file = fopen("/proc/uptime", "r");

		//the file contains two numbers only 
		//the first for uptime 
		//the second for idletime 
                double uptime   = 0;
                double idletime = 0;
		
	       	//the two numbers on the file are written on this formule "2323.3  32323.3"
                //so i will get them by the strtok function 
		//then convert the text to float number

                char *ptr = NULL;

                //get the file content
                fread(content,sizeof(char),100,file);

                ptr = strtok(content," ");
		uptime = strtod(ptr,NULL);
                
		ptr = strtok(NULL," ");
                idletime = strtod(ptr,NULL);

		
		//then i will start to convert it to (hours,minutes,seconds) 
		int uptime_hour = (int)(uptime/3600);
		int uptime_minute = ((int)uptime%3600)/60;
		double uptime_sec = uptime - uptime_hour*3600 - uptime_minute*60;

		int idletime_hour = (int)(idletime/3600);
                int idletime_minute = ((int)idletime%3600)/60;
                double idletime_sec = idletime - idletime_hour*3600 - idletime_minute*60;

		printf("UP_Time      :%0.2fs\n",uptime);
		printf("UP_Time   >> Hours   :%dh\n",uptime_hour);
		printf("          >> Minutes :%dm\n",uptime_minute);
		printf("          >> Seconds :%0.2fs\n",uptime_sec);
		
		printf("===============================\n");

		printf("IDLE_Time    :%0.2fs\n",idletime);
		printf("IDLE_Time >> Hours   :%dh\n",idletime_hour);
                printf("          >> Minutes :%dm\n",idletime_minute);
                printf("          >> Seconds :%0.2fs\n",idletime_sec);

		}

	else //Incase of user Enters more than one argument(token)
        {
             write(STDERR, "myuptime : too much argument\n",strlen("type : too much argument\n"));
        }
}


/**
 * @brief add environment variable
 * This function attemps to add env var
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user
 * @return : no return.
 */
void vdADDEnvVarFunc(char **token,int lenth)
{
        //this function will work only if the user enter >> "addenv <VarName> = <VarValue>"
	//so the lenth must = 4
        if(lenth == 4)
        {   	
		//the equal operator must be written
		if((strcmp(token[2],"="))==0)
		{
			setenv(token[1],token[3] , 1);
		}
		else{
			write(STDERR, "addenv : missing \"=\" operator\n",strlen("addenv : missing \"=\" operator\n"));
			write(STDERR, "correct way >> \"addenv <VarName> = <VarValue>\n",strlen("correct way >> \"addenv <VarName> = <VarValue>\n"));
		}
	}
	//incase of incorrect way to add env variable
	else{
		write(STDERR, "addenv : incorrect way to assign environment variable\n",strlen("addenv : incorrect way to assign environment variable\n"));
		write(STDERR, "correct way >> \"addenv <VarName> = <VarValue>\n",strlen("correct way >> \"addenv <VarName> = <VarValue>\n"));
	}
}

/**
 * @brief add local variable
 * This function attemps to add local var to the array of struct
 * @param token : this contains array of string (the command separated).
 * @param lenth : this contains the number of tokens from the user
 * @return : no return.
 */
void vdADDlocalVarFunc(char **token , int lenth)
{
	//this function will work only if the user enter >> "addloc <VarName> = <VarValue>"
        //so the lenth must = 4
	 if(lenth == 4)
        {
		//the equal operator must be written
                if((strcmp(token[2],"="))==0)
                {
			//check if the variable is already exist
			for(int i = 0;i<loc_num;i++)
                	{
                	       	 if((strcmp(token[1],local_Var_arr[i].loc_name))==0)
				 {
				 	write(STDERR, "addloc : this variable is already exist\n",strlen("addloc : this variable is already exist\n"));
					return;
				 }
               	 	}

			//incase of first add local variable
			if(loc_num == 0)
			{	
				//allocate one struct of local_Var_arr
				local_Var_arr = malloc(sizeof(locVar));

				//allocate the two strings of the struct local_Var_arr
				local_Var_arr[loc_num].loc_name = malloc(sizeof(token[1]));
				local_Var_arr[loc_num].loc_value = malloc(sizeof(token[3]));

				//assign the two string of the struct local_Var_arr
				strcpy(local_Var_arr[loc_num].loc_name,token[1]);
				strcpy(local_Var_arr[loc_num].loc_value,token[3]);
				loc_num++;
			}
			else{
				//incase of it isn't the first allocate
				//allocate the next index of the array of the struct
				//allocate extra one struct of local_Var_arr
				local_Var_arr = realloc(local_Var_arr,(loc_num+1)*sizeof(locVar));

				//allocate the two strings of the struct local_Var_arr
                                local_Var_arr[loc_num].loc_name = malloc(sizeof(token[1]));
                                local_Var_arr[loc_num].loc_value = malloc(sizeof(token[3]));

				//assign the two string of the struct local_Var_arr
                                strcpy(local_Var_arr[loc_num].loc_name,token[1]);
                                strcpy(local_Var_arr[loc_num].loc_value,token[3]);
                                loc_num++;
			
			}	
                }
                else{
			//incase of incorrect way to add local variable
                        write(STDERR, "addloc : missing \"=\" operator\n",strlen("addloc : missing \"=\" operator\n"));
			write(STDERR, "correct way >> \"addloc <VarName> = <VarValue>\n",strlen("correct way >> \"addenv <VarName> = <VarValue>\n"));
                }
        }
        else{
		//incase of incorrect way to add local variable
                write(STDERR, "addloc : incorrect way to add local variable\n",strlen("addenv : incorrect way to add local variable\n"));
                write(STDERR, "correct way >> \"addloc <VarName> = <VarValue>\n",strlen("correct way >> \"addenv <VarName> = <VarValue>\n"));
        }

}

/**
 * @brief print all variable
 * This function attemps to print all variable (env and local)
 * @param lenth : this contains the number of tokens from the user
 * @return : no return.
 */
void vdPrintALLVarFunc( int lenth)
{
         if(lenth == 1)
        {
		//loop on the array and print the local variable then print the env variable
                for(int i = 0;i<loc_num;i++)
		{
			printf("%s=%s\n",local_Var_arr[i].loc_name,local_Var_arr[i].loc_value);
		}
		printf("==================================================\n");

		vdPrintEnvFunc(NULL,lenth);
	}
        else{
                write(STDERR, "allVar : too much argument \n",strlen("allVar : too much argument \n"));
        }

}
