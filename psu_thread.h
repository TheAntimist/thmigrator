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
//pthread_t pthd;
int curr_mode;
//ucontext_t context;

static char st1[8192];

int sock_fd;

typedef struct psu_thread_info{
    ucontext_t ctx;
//    char stack[8192];
} psu_thread;

static psu_thread psut;


void psu_thread_setup_init(int mode)
{
	//Read from a file to set up the socket connection between the client and the server

    printf("Initializing psu_thread\n");
    curr_mode = mode;
    if(mode == 0){
        printf("Setting up Client..\n");

        sock_fd = setup_new_socket(mode, "130.203.16.31");
        //char * msg = "Hi";
        //int count = send(sock_fd, msg, sizeof(msg), 0);
        //printf("Bytes sent: %d\n", count);
        //exit(0);
    }
    else{
        printf("Setting up server\n");
        
        sock_fd = setup_new_socket(mode, "NULL");
        
        //char msg[4];
        //int count = recv(sock_fd, msg, 4, 0);

        //printf("Received: %d bytes\n", count);
        //printf("Message: %s\n", msg);
        //exit(0); 

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
    if(curr_mode == 0){
      // pthread_create(&ptid, NULL, execute_function, NULL);
      // pthread_join(ptid, NULL);
      
             
      
      getcontext(&(psut.ctx));
      psut.ctx.uc_stack.ss_size=sizeof(st1);
      psut.ctx.uc_stack.ss_sp=st1;
      psut.ctx.uc_link=NULL;
      psut.ctx.uc_stack.ss_flags = 0;
    
      makecontext(&psut.ctx, user_func, 0);
      setcontext(&psut.ctx);
    
    }
    else{


        int count = recv(sock_fd, &psut, sizeof(psu_thread), 0);
        printf("Received %d bytes\n", count);
       
        greg_t eip = psut.ctx.uc_mcontext.gregs[REG_EIP];
        getcontext(&(psut.ctx)); 
        psut.ctx.uc_stack.ss_size=sizeof(st1);
        psut.ctx.uc_stack.ss_sp=st1;
        psut.ctx.uc_link=NULL;
        psut.ctx.uc_stack.ss_flags = 0;

        makecontext(&(psut.ctx), user_func, 0);
        psut.ctx.uc_mcontext.gregs[REG_EIP] = eip;
        setcontext(&(psut.ctx));
    
	}
	return 0; 
}

void psu_thread_migrate(const char *hostname)
{
	//thread Migration related code
    getcontext(&(psut.ctx));
    psut.ctx.uc_mcontext.gregs[REG_EIP] = __builtin_return_address(0);

    send(sock_fd, &psut, sizeof(psu_thread), 0);

    exit(0);

}
