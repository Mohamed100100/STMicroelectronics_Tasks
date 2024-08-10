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
//i will create array of this structure to contain the children data
typedef struct
{
	char *ChildName  ;
	int   ChildPID	 ;
	int   ChildId    ;
	int   ChildState ;
	int   ChildExit  ;
}child_info;

child_info *child_info_arr = NULL;

//the index of the child >> if ChildTblIndx = 3 so there are 3 childs are created 
static int ChildTblIndx   = 0;

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
	//if there redirecion >> index = -4
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
	write(STDERR, "error : Unsupported Command\n",strlen("error : Unsupported Command\n"));
        write(STDERR, "you can write \"myhelp\" command for help with Commands\n",strlen("you can write \"myhelp\" command for help with commands\n"));
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
			
			vdAddChildInfo(getpid(),retPid,token[0],status);
		
		}
		//child region
		else if (retPid == 0)// if the fork point is created successfully the retPID = zero for the child
		{// so this section will be executed by the created child only
			
	
			execve(full_path,token, environ);
			exit(-1);
		}
		else
		{
			perror("fork");
		}
	}
	//unsuported command
	else{
		//do nothing
	}
	
	//ret = -2 >> External command
	//ret = -1 >> Unsupported
	return (ret);
}


/**
 * @brief Allocating  dynamic memory for the Table of children
 * This function attempts to help the phist function to allocate memory and store the info of this childrens
 * @param PID       : parent Id
 * @param ID        : Child Id
 * @param status    : exit status
 * @param childname : child name 
 * @return : no return.
 */
void vdAddChildInfo(int PID,int ID,char *childname,int status)
{
	if(ChildTblIndx == 0)
	{
        	//allocate one struct of child_info
                child_info_arr = malloc(sizeof(child_info));

	}
	else{
		//incase of it isn't the first allocate
		//allocate the next index of the array of the struct
		//allocate extra one struct of ChildTblIndx
		child_info_arr = realloc(child_info_arr,(ChildTblIndx+1)*sizeof(child_info));
	}
	
	//allocate the memory for the member of this struct
        child_info_arr[ChildTblIndx].ChildName   =  malloc(sizeof(childname));

	//assign values for the members of the structure
	strcpy(child_info_arr[ChildTblIndx].ChildName,childname);
	child_info_arr[ChildTblIndx].ChildPID  = PID;
	child_info_arr[ChildTblIndx].ChildId   = ID ;
//	child_info_arr[ChildTblIndx].ChildExit = status;

	
	if(WIFEXITED(status) == 1)
	{       
		child_info_arr[ChildTblIndx].ChildState = EXIT_NORM;
		child_info_arr[ChildTblIndx].ChildExit  = WEXITSTATUS(status) ;
	}
               
       	else{   //Termenation By Signal
       
		if(WIFSIGNALED(status) == 1)
		{              
			child_info_arr[ChildTblIndx].ChildState = EXIT_TERM;
			child_info_arr[ChildTblIndx].ChildExit  = WIFSIGNALED(status) ;
		}
		else         
		{
			//Stopped By Signal
			if(WIFSTOPPED(status) == 1)
			{
				child_info_arr[ChildTblIndx].ChildState = EXIT_STOP;
				child_info_arr[ChildTblIndx].ChildExit  = WIFSTOPPED(status);
			}
			else
			{       //Undetrmined >> I put it because it may be another kinds of signals
				child_info_arr[ChildTblIndx].ChildState = EXIT_UNDEF;
			}
		}
	}
	ChildTblIndx++;
	
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
		for(int i = 0;(i<ChildTblIndx);i++)
		{
		
		//	write(STDOUT, "\n The Children data : \n",strlen("\n The Children data : \n"));
			printf("\n Child %d \t ID : %d \t PID : %d \t Status : %c %d",i,child_info_arr[i].ChildId,child_info_arr[i].ChildPID, Process_satatus[child_info_arr[i].ChildState],child_info_arr[i].ChildState);
			printf("\tProcess Named : %s\n",child_info_arr[i].ChildName);				
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
write(STDOUT, "11- myfree: print RAM info (total , free , used) and swap info (total , free , used) \n",strlen("11- myfree: print RAM info (total , free , used) and swap info (total , free , used) \n"));
write(STDOUT, "12- myuptime : print the uptime on the system and idle time\n",strlen("12- myuptime : print the uptime on the system and idle time\n"));
write(STDOUT, "13- addloc: create new local variable\n",strlen("13- addloc: create new local variable\n"));
write(STDOUT, ">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n",strlen(">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n"));
write(STDOUT, "14- addenv: create new environment variable\n",strlen("13- addloc: create new environment variable\n"));
write(STDOUT, ">>>>>>>>>> \" addenv <Var_Name> = <Var_Value>> \"\n",strlen(">>>>>>>>>> \" addloc <Var_Name> = <Var_Value>> \"\n"));
write(STDOUT, "15- allVar: print environment variable and local\n",strlen("14- allVar: print environment variable and local\n"));
write(STDOUT, "16- output redirection \" {command}  >  {output_file} \"\n",strlen("15- output redirection \" {command}  >  {output_file} \"\n"));
write(STDOUT, "17- input redirection  \" {command}  <  {input_file} \"\n",strlen("16- input redirection   \" {command}  <  {input_file} \"\n"));
write(STDOUT, "18- error redirection  \" {command} 2>  {error_file} \"\n",strlen("17- error redirection   \" {command} 2>  {error_file} \"\n"));
write(STDOUT, "NOTE : you can do many redirection at the same command with different order [don't forget the spaces] \n",strlen("NOTE : you can do many redirection at the same command with different order [don't forget the spaces] \n"));
write(STDOUT, "ex>>>>\" {command} 2> {error_file} > {output_file} < {input_file} \" \n",strlen("ex>>>>\" {command} 2> {error_file} > {output_file} < {input_file} \" \n"));
write(STDOUT, "19- pipe way  \" {command_1} | {command_2} | {command_3} | ... | .....  \"\n",strlen("19- pipe way  \" {command_1} | {command_2} | {command_3} | ... | .....  \"\n"));
write(STDOUT, "NOTE : You can do multi pipes but don't forget the separating spaces\n",strlen("NOTE : You can do multi pipes but don't forget the separating spaces\n"));
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
	// to avoid "mycp only"
	if(lenth == 1)
	{
		write(STDERR, "mycp :error, missing src path and dest path\n",strlen("mycp :error, missing src path and dest path\n"));
		return ;
	}

	if(lenth == 2)
        {
                write(STDERR, "mycp :error, missing dest path\n",strlen("mycp :error, missing dest path\n"));
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
			if((errno == EISDIR)||(errno == EEXIST))
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
				write(STDERR,token[0],strlen(token[0]));				
				write(STDERR, " : file is existing\n",strlen(" : file is existing\n"));
				errno = -1;
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
	else{
		// Incase of too much tokens
		write(STDERR,token[0],strlen(token[0]));
                write(STDERR, " : too much argument\n",strlen(" : too much argument\n"));
        

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

	if(lenth == 2)
        {
        	write(STDERR, "mymv :error, missing dest path\n",strlen("mycp :error, missing dest path\n"));
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
		if(errno == -1)
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
		 if(errno == -1){ 
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
				write(STDERR, " : doesn't exist in environment variables\n",strlen(" : doesn't exist in environment variables\n"));
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
		// I found that when i write " type / " it will produce >> external command
		// so i avoid this corner case by this if statment
		if(strcmp(token[1],"/") == 0)
		{
			write(STDOUT, token[1],strlen(token[1]));
                        write(STDOUT, " : Unsupported command\n",strlen(": wUnsupported command\n"));
			return;
		}
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

	//this flag is used for determine if the command is external or unsupported
        int flag = 0;

	//this array used to contain the full path of the excutable file of the command
        char *full_path = NULL;

	//if the input command is already full path 
	//for ex >> " /usr/bin/ls "  OR "./shell.exe" so excute it directly 
	//if not so search for it on PATH env
	if((access(token[0], F_OK)) == 0)
        {
        	if(open(token[0],O_DIRECTORY)==-1)
		{
			flag = 1;
			full_path = token[0];
		}
		else{
			write(STDERR,token[0],strlen(token[0]));
			write(STDERR," : Is a directory\n",strlen(" : Is a directory\n"));
		}
    	}   
	else{
	//I create this array of characters to contain the PATH variable value
	 char path_env[300] ={'\0'};

        //I make copy from the PATH variable to avoid overwrite on it
        strcpy(path_env, getenv("PATH"));

        //this array of character is used to contain the tokens from PATH variable(separated by ':')
        char *token_path = NULL;
        token_path = strtok(path_env,":");

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
				 	//write(STDERR, "addloc : this variable is already exist\n",strlen("addloc : this variable is already exist\n"));
					local_Var_arr[i].loc_value = realloc(local_Var_arr[i].loc_value,sizeof(token[3]));
					memset(local_Var_arr[i].loc_value,'\0',strlen(local_Var_arr[i].loc_value));
					strcpy(local_Var_arr[i].loc_value,token[3]);
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

/**
 * @brief Execute the Pipe command 
 * This function attemps to execute the pipe command with differnt number of the pipes
 * @param lenth   : this contains the number of tokens from the user
 * @param token   : this contains array of string (the command separated).
 * @param pipenum : the number of pipes on the input command 
 * @return : no return.
 */
void vdPipeExecFunc(int pipenum ,char **token , int lenth)
{
	
	int strcmd = 0;
	
	char *full_path = NULL;

	int p =0;

	// i will allocate an array of integer to contain the file discreptors
	// for ex >> if input command is " ls | grep out | sort "
	// so we have two pipes so we will create two pipes p1{p1[0] > input AND p1[1] > output} and p2{p2[0] > input AND p2[1] > output}
	// after that we will store the values of the file discriptors in form of :-
	// [input for command 1, output from command 1,input for command 2, output from command ,input for command 3, output from command 3]
	// =======================================================================
	// so 	   | INComm1 | OUTComm1 | INComm2 | OUTComm2 | INComm3 | OUTComm3|
	// =======================================================================
	// pipeFd = [STDIN  ,  p1[1]   ,  p1[0]  ,  p2[1]   ,  p2[0]  ,  STDOUT ]
	// =======================================================================
	// to make it general case 
	// at the first i allocate array of integers of lenth {2*numberOfPipes + STDIN + STDOUT}
	int *pipeFd = (int *)malloc(sizeof(int)*(2*pipenum +2));

	// this array used to extract the default STDIN and STDOUT
	char arrstd[100]= {'\0'};
        readlink("/proc/self/fd/2", arrstd, 100);
	
	//now i will assign the start and the end of the array to have STDIN and STDOUT file discreptors
	pipeFd[2*pipenum +1] = open(arrstd,O_RDWR,S_IRWXU);
	pipeFd[0] = pipeFd[2*pipenum +1];
	
	//now i will start to create the pipes according to the input argument of the function "pipenum"
	//this for creates the pipes and also stores its file discreptors at the array "pipeFd" in the right order {which mentioned above}
	int temp = 0;
	int k = 1;
	for(int i =0;i<pipenum;i++)
	{
		pipe(&pipeFd[k]);
		temp = pipeFd[k] ;
		pipeFd[k] = pipeFd[k+1];
		pipeFd[k+1] = temp;
		k=k+2;
	}

	// Now we will start to loop on the all tokens 
	// don't forget according to the above example " ls | grep out | sort "
	// =====================================================================================
        // so      | token[0] | token[1] | token[2] | token[3] | token[4] | token[5] | token[6]|
        // =====================================================================================
        //  	   [   "ls"   ,   "|"    ,  "grep"  ,  "out"   ,    "|"   ,  "sort"  ,   NULL  ]
        // =====================================================================================
	// we have three commands >>> "ls"       and its lenth = 1
	// 			  >>> "grep out" and its lenth = 2
	// 			  >>> "sort"     and its lenth = 1
	// we will put instead of the "|" NULL to separete this commands 			  

	for(int j=0;j<=lenth;j++)
	{
		// i put this if to avoid segmantaion fault when strcmp function access the last token
		// in our example it is token[6] which equal to NULL
		if(j == lenth) {
		
			//i put at it any address and when enter the next if it will return again to be equal NULL
			char *gemy = "|";
			token[j] = gemy;
		}
		//we enter this if statment at the end of the command and execute it in a child using fork
		if((strcmp(token[j],"|") == 0)||(j == lenth))
		{
			//we replace the "|" by NULL 
			token[j] = NULL;
			
			// Now we will start forking and if we have two pipes >> three commands 
			// so we will have 3 commands with 3 forks 
			int retPid = fork();

			//child part
			if(retPid == 0)
			{	
				//redirection part
				//redirect the input 
				//varible "p" is an index to loop on the file discreptor array
				//i create it above to be see on the parent and children also
				//i increase it at the parent part to be seen on the next child
				//the "strcmd" varible has the index of the start command 
				//look at the above example
				//its value will be >> 0 , 2 . 5
				close(0);
                                dup2(pipeFd[p],0);
                                close(pipeFd[p]);

				//redirection the output
				close(1);
                                dup2(pipeFd[++p],1);
                                close(pipeFd[p]);

				// after that we will close all extra opened file discreptors and pipes {on this child scope}
				// and if we open "/proc/ {this child id} / fd"
				// we will find also the three opened file discreptors 0,1,2 with suitable link
				for(int i =1;i<=2*pipenum+1;i++)
			        {
              				 close(pipeFd[i]);
        			}
				
				//i do this if to separete the external command to avoid extra forking 
				//if the command is external the "intIsExt" function will return the full path of the command on the "full_path" variable
				//internal or unsupported
				if(intIsExt(&token[strcmd],&full_path) != 1)
				{
					//this function takes the lenth of the command 
					//i calculate it by subtracting the current index "|" and the index of the start command  
					vdExcCommand(&token[strcmd],j-strcmd);
					exit(0);
				}
				//external command
				else
				{
					// intIsPipe function checks if there is redirection " > OR < OR 2> " or not 
					// if there is piping it will redirect it and return 1
					// if there is not it will return 0
					// if there is redirection and the file is not exist {Incase of the input redirecion} it will print error and return -1
					if(intIsRedirec(&token[strcmd],j-strcmd) != -1)
					{
						execve(full_path,&token[strcmd], environ);
 						exit(0);
					}
					else{
						exit(-1);
					}
				}
			}
			//parent part
			else{
				// the "j" variable refers to the "|" 
				// and "strcmd" have the index of the command so it will be equal (j+1)
				strcmd = j + 1;
				//we said that "p" varible loops on the file discreptors array 
				//we increase it on the parent to be seen on the parent and the children
                        	p = p+2;
				//continue fork the other child
			}
		}
	}
	
	
	// after that we will close all extra opened file discreptors and pipes {on this parent scope}
        // and if we open "/proc/ {parent id} / fd"
        // we will find also the three opened file discreptors 0,1,2 with suitable link >> its default values
	for(int i =1;i<=2*pipenum+1;i++)
	{
		close(pipeFd[i]);
	}
	//loop untill all children finish thier work
	while(wait(NULL)!=-1);

}



/**
 * @brief Check on the redirection
 * This function attemps to chick if the command has redirection or not 
 * if it has redirection it will execute it and return 1
 * if not it will return 0
 * if there is redirection and the file is not exist {Incase of the input redirecion} it will print error and return -1
 * @param lenth   : this contains the number of tokens from the user
 * @param token   : this contains array of string (the command separated).
 * @return : no return.
 */
int intIsRedirec(char **token,int lenth)
{

	int flag = 0;
	int var = 0;
	int newlen = 0;
	int file = 0;

	//this if detect if the command has redirection or not
        //if there redirecion >> flag = 1
        //if not flag = 0
        //at the first we check at two conditions ( " > file_name " OR " ls > ") there is no command or there is no file_name
        if(!((strcmp(token[lenth-1], ">")==0)||(strcmp(token[lenth-1], "<")==0)||(strcmp(token[lenth -1], "2>")==0)
           ||(strcmp(token[0], ">")==0)||(strcmp(token[0], "<")== 0)||(strcmp(token[0], "2>")== 0)))
        {
                for(int i = 0;i<lenth;i++)
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
                                        flag = 1;//true redirect 

                                        //i put this variable to count the redirection 
                                        //for ex >> " ls > out1 file1 "
                                        // it must give error because undetermined behavior
                                        var = var +2;
                                        continue;

                                }
                                else{
                                        //in case failled to open
                                        flag = -1;
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
                                        flag = 1;//true redirect 

                                        //i put this variable to count the redirection
                                        //for ex >> " pwd 2> err1 file1 "
                                        // it must give error because undetermined behavior
                                        var = var +2;

                                        continue;
                                }
                                else{
                                        //in case failled to open
                                        flag = -1;
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
                                        flag = 1;//true redirect 

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
                                        flag = -1 ;
                                        break;
                                }

                        }
                }

        }
        else{
                // in case of no redirection
                flag = 0 ;
        }


	// this if use the variable var which created above to capture this error
	// " mypwd > output file "
	// the lenth of this command = 4
	// var variable = 2 (the number of redirection argument (> and output))
	// newlen variable = 1 (it counts before the redirection(mypwd only))
	// so the argument (file) will produce undetremined behavior
	// so our check >> if (newlen + var) == lenth >> the command is written well
	// the second part of the condition(flag == a) ensures that the command has redirection 
	if(((newlen + var) != lenth)&&(flag == 1)) flag = 0;

	// this condition set the lenth to the newlen
	if (newlen != 0) lenth = newlen;

	return flag;
}

