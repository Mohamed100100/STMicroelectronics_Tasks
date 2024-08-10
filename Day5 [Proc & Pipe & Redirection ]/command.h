#ifndef COMMAND_H_
#define COMMAND_H_

#define STDIN  	 0
#define STDOUT	 1
#define STDERR	 2

#define COMMAND_LENGTH 1000
#define COMMAND_NUMBER	15


//this macros is used for assign the status of the process
#define EXIT_NORM	0
#define EXIT_TERM	1
#define EXIT_STOP	2
#define EXIT_UNDEF	3


void vdExcCommand(char **token,int lenth);
void vdHelpFunc(int lenth);
void vdExitFunc(int lenth);
void vdPwdFunc(int lenth);
void vdEchoFunc(char **token,int lenth);
void vdCopyFunc(char **token,int lenth);
void vdMoveFunc(char **token,int lenth);
void vdCdFunc(char **token,int lenth);
void vdPrintEnvFunc(char **token ,int lenth);
void vdTypeFunc(char **token ,int lenth);
void vdHelperForStat_initFunc(void);
void vidPhistFunc(int lenth);
void vdFreeFunc(int lenth);
void vdUptimeFunc(int lenth);
void vdADDlocalVarFunc(char **token , int lenth);
void vdADDEnvVarFunc(char **token,int lenth);
void vdPrintALLVarFunc( int lenth);
void vdPipeExecFunc(int pipenum ,char **token , int lenth);
int intIsRedirec(char **token,int lenth);
void vdAddChildInfo(int PID,int ID,char *childname,int status);






static int intIsExt(char **token,char **retpath);
static int intExcuteExternComm(char **token);


#endif //COMMAND_H_
