#ifndef COMMAND_H_
#define COMMAND_H_

#define STDIN  	 0
#define STDOUT	 1
#define STDERR	 2

#define OPTION_NUMBER	7
#define COMMAND_LENGTH 1000
#define COMMAND_NUMBER	6




void vdExcCommand(char **token,int lenth);
void vdHelpFunc(int lenth);
void vdExitFunc(int lenth);
void vdPwdFunc(int lenth);
void vdEchoFunc(char **token,int lenth);
void vdCopyFunc(char **token,int lenth);
void vdMoveFunc(char **token,int lenth);

#endif //COMMAND_H_
