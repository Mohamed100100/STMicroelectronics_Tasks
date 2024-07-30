#ifndef COMMAND_H_
#define COMMAND_H_

#define STDIN  	 0
#define STDOUT	 1
#define STDERR	 2

#define COMMAND_LENGTH 1000
#define COMMAND_NUMBER	10
#define TOKEN_LENGTH	50

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

static int intIsExt(char **token,char **retpath);
static int intExcuteExternComm(char **token);


#endif //COMMAND_H_
