
#include <stdio.h>
#include <string.h>
 #include <unistd.h>

#define OPTION_NUMBER	7
#define COMMAND_LENGTH 1000
#define COMMAND_NUMBER	6

char *command_name[COMMAND_NUMBER]={
	    		"cp",
	    		"echo",
	    		"mv",
	    		"exit",
	    		"pwd",
	    		"help"}; 


#define STDIN  	 0
#define STDOUT	 1
#define STDERR	 2


void vdExcCommand(char **token,int lenth);


char command[COMMAND_LENGTH] = {'\0'};
char *token[OPTION_NUMBER];
int main()
{
	int i = 0;
	int arg_num = 0;
	while(1)
	{
	write(STDOUT, "BASH MASR >> ",strlen("BASH MASR >> "));
	read(STDIN, command, COMMAND_LENGTH);
	i = 0;
	token[i] = strtok(command," ");
	while(token[i]!=NULL)
	{
		token[++i] = strtok(NULL," ");
		
	}
	arg_num = i;
	token[arg_num-1][strlen(token[arg_num-1])-2] = '\0';
/*
//	char *strcat(char *dest, const char *src);
	printf("%d\n",arg_num);
	for(int j = 0;j<arg_num;j++)
	{
		write(STDOUT, token[j],strlen(token[j]));
		write(STDOUT, "\n",strlen("\n"));
	}
	*/
	if(token[0] != NULL)
	{
	vdExcCommand(token,arg_num);
	memset(command, '\0', COMMAND_LENGTH);
	}
	else{
	//nothing
	}
	}

}
void vdExcCommand(char **token,int lenth)
{

	int index = 0;
	int flag = -1;
	for(int j = 0;j<COMMAND_NUMBER;j++)
	{
		flag = strcmp(token[0], command_name[j]);
		if(flag == 0)
		{index = j;break;}
	}
	switch(index)
	{
		case 5 :
		case 2 :
	default :
	write(STDOUT, "error, invalid command\n",strlen("error, invalid command\n"));
        write(STDOUT, "you can write help command for help\n",strlen("you can write help command for help\n"));
	break;
	}
/*

	if(token[0]!= NULL)
	{
	write(STDOUT, "error, invalid command\n",strlen("error, invalid command\n"));
	write(STDOUT, "you can write help command for help\n",strlen("you can write help command for help\n");
	}
	else
	{
	write(STDOUT, "error, invalid command\n",strlen("error, invalid command\n"));
        write(STDOUT, "you can write help command for help\n",strlen("you can write help command for help\n");

	}
	*/
}	


void vdHelpFunc(char **token,int lenth)
{
	if(lenth == 1)
	{
		
	}


}


