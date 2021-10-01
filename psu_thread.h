#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_GNU
#include <ucontext.h>
#include <pthread.h>
#include "socket.h"

//client - 0 ; server - 1
//client starts initially, server get it
static ucontext_t context;
pthread_t pthd;
int curr_mode;

static char st1[8192];

typedef struct psu_thread_info{
    ucontext_t context;
    char stack[8192];
    void* eip;
} psu_thread_info_t;

void psu_thread_setup_init(int mode)
{
	//Read from a file to set up the socket connection between the client and the server

    printf("Initializing psu_thread\n");
    curr_mode = mode;
    if(mode){
        printf("Setting up Client\n");
    }
    else{
        printf("Setting up server\n");
    }

	return;
}


void execute_function(ucontext_t ctx, void * eip, void *(*user_func)(void*), void *user_args){
    getcontext(&ctx);
    ctx.uc_stack.ss_size=sizeof(st1);
    ctx.uc_stack.ss_sp=st1;
    ctx.uc_link=NULL;
    ctx.uc_stack.ss_flags=0;

    makecontext(&ctx, user_func, 0);
    ctx.uc_mcontext.gregs[REG_EIP] = eip;
    setcontext(&ctx);
        

}


int psu_thread_create(void * (*user_func)(void*), void *user_args)
{
	// make thread related setup
	// create thread and start running the function based on *user_func

    //client
    if(modes == 0){
       pthread_create(&ptid, NULL, execute_function, NULL);
       pthread_join(ptid, NULL);
    }
    else{
        getcontext(&a); 
        a.uc_stack.ss_size=sizeof(st1);
        a.uc_stack.ss_sp=st1;
        a.uc_link=NULL;
        a.uc_stack.ss_flags = 0;

        makecontext(&a, user_func, 0);
        a.uc_mcontext.gregs[REG_EIP] = 0x804884e;
        setcontext(&a);
    
	}
	return 0; 
}

void psu_thread_migrate(const char *hostname)
{
	//thread Migration related code
    getcontext(&a);
    void * addr = __builtin_return_address(0);

    if(modes== 0){
        pthread_exit(0);
    }    
    else{
    printf("Returning\n");
    return;
    }
    return;
}
