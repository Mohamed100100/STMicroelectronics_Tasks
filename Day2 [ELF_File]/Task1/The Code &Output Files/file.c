#include <stdio.h>

int globVar_init = 10;
int globVar_uninit ;

const int globVar_const_init = 20;
const int globVar_const_uninit;

static int globVar_stat = 20;

int globalVar_ext ;



void file_func(void){
	
	static int localvar_static = 10;
        const int localVar_const = 20;
        int localVar = 3;

	printf("globVar_init         =  %d\n",globVar_init);
        printf("globVar_uninit       =  %d\n",globVar_uninit);

        printf("globVar_const_init   =  %d\n",globVar_const_init);
        printf("globVar_const_uninit =  %d\n",globVar_const_uninit);

        printf("globVar_stat         =  %d\n",globVar_stat);

        printf("localvar_static      =  %d\n",localvar_static);
        printf("localVar_const       =  %d\n",localVar_const);
        printf("localVar             =  %d\n",localVar);


}


