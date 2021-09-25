#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_GNU
#include <ucontext.h>
#include <pthread.h>

//you write the code here


static ucontext_t ctx;
pthread_t ptid, pthid;
int modes;

static char st1[8192];

//typedef struct psu_thread_info psu_thread_info_t;

void psu_thread_setup_init(int mode)
{
	//Read from a file to set up the socket connection between the client and the server

   // printf("Initializing psu_thread\n");

    if(mode == 1){
        printf("init: client");
        modes = 1;
    }
    else{
        printf("Setting up server\n");
        modes = 0;
    }

	return;
}

int psu_thread_create(void * (*user_func)(void*), void *user_args)
{
	// make thread related setup
	// create thread and start running the function based on *user_func
	
    if(modes == 0){
        pthread_create(&ptid, NULL, user_func, NULL);
        pthread_join(ptid, NULL);
    }
    else{
        printf("Going to wait on server request now\n");
 
        printf("Setting context now\n");
        a.uc_link = 0;
        a.uc_stack.ss_sp=st1;
        a.uc_stack.ss_flags = 0;
        makecontext(&a, user_func, 0);
        setcontext(&a);
    
	}
	return 0; 
}

void psu_thread_migrate(const char *hostname)
{
	//thread Migration related code

    getcontext(&a);
    a.uc_link = 0;
    a.uc_stack.ss_sp=st1;
    a.uc_stack.ss_flags = 0;

    printf("Value of EIP is: %d\n", a.uc_mcontext.gregs[REG_EIP]);
   
    printf("Here in migration\n");
 
    FILE *outfile;

    outfile = fopen("data.txt", "wb");

    if(outfile == NULL){
        printf("\nError opened file\n");
        exit(1);
    }

    fwrite(&a, sizeof(ucontext_t), 1, outfile);

    if(fwrite != 0)
        printf("Contents written successfully\n");
    else
        printf("Error writing file\n");

    fclose(outfile);
    pthread_exit(NULL);
    return;
}
