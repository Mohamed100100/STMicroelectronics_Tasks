#include <stdio.h>

extern int globalVar_ext ;
void file_func(void);

int main(){
	
	file_func();
        printf("globalVar_ext        =  %d\n",globalVar_ext);
	
}

