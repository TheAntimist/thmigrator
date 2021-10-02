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
static ucontext_t ctx_main;
//pthread_t pthd;
int curr_mode;

int sock_fd;

typedef struct psu_thread_info {
    ucontext_t ctx;
    char stack[8192];
} psu_thread;

static psu_thread psut;


void psu_thread_setup_init(int mode) {
    //Read from a file to set up the socket connection between the client and the server

    printf("Initializing psu_thread\n");
    curr_mode = mode;
    if (mode == 0) {
        printf("Setting up Client..\n");
    } else {
        printf("Setting up server\n");

        sock_fd = setup_new_socket(mode, "NULL");

    }

    return;
}


void execute_function(ucontext_t ctx, void *eip, void *(*user_func)(void *), void *user_args) {
    getcontext(&ctx);
    //ctx.uc_stack.ss_size=sizeof(st1);
    //ctx.uc_stack.ss_sp=st1;
    ctx.uc_link = NULL;
    ctx.uc_stack.ss_flags = 0;

    makecontext(&ctx, user_func, 0);
    ctx.uc_mcontext.gregs[REG_EIP] = eip;
    setcontext(&ctx);


}


int psu_thread_create(void *(*user_func)(void *), void *user_args) {
    // make thread related setup
    // create thread and start running the function based on *user_func

    //client
    if (curr_mode == 0) {
        // pthread_create(&ptid, NULL, execute_function, NULL);
        // pthread_join(ptid, NULL);
        //TODO: Maybe incorporate threads


        getcontext(&(psut.ctx));
        psut.ctx.uc_stack.ss_size = sizeof(psut.stack);
        psut.ctx.uc_stack.ss_sp = psut.stack;
        psut.ctx.uc_link = NULL;
        psut.ctx.uc_stack.ss_flags = 0;

        makecontext(&psut.ctx, user_func, 1, user_args);
        setcontext(&psut.ctx);

    } else {

        ssize_t size = sizeof(psu_thread);
        size_t length = 0;
        size_t n = 0;
        const char *p = &psut;
        while (n != size) {
            length = recv(sock_fd, p, size, 0);
            if (length <= 0) {
                printf("Error in receiving\n");
                return -1;
            }

            p += length;
            n += length;
            printf("Reading %d bytes\n", length);

        }

        greg_t eip = psut.ctx.uc_mcontext.gregs[REG_EIP];
        greg_t ebp = psut.ctx.uc_mcontext.gregs[REG_EBP];
        greg_t esp = psut.ctx.uc_mcontext.gregs[REG_ESP];
        getcontext(&(psut.ctx));
        psut.ctx.uc_stack.ss_size = sizeof(psut.stack);
        psut.ctx.uc_stack.ss_sp = psut.stack;
        psut.ctx.uc_link = &ctx_main;
        psut.ctx.uc_stack.ss_flags = 0;

        makecontext(&(psut.ctx), user_func, 0);
        psut.ctx.uc_mcontext.gregs[REG_EIP] = eip;
        psut.ctx.uc_mcontext.gregs[REG_EBP] = ebp;
        psut.ctx.uc_mcontext.gregs[REG_ESP] = esp;
        swapcontext(&ctx_main, &(psut.ctx));

    }
    return 0;
}

int psu_thread_migrate(const char *hostname) {
    //thread Migration related code
    getcontext(&(psut.ctx));
    printf("EBP: %x\n", psut.ctx.uc_mcontext.gregs[REG_EBP]);
    greg_t *ebp = (greg_t *) psut.ctx.uc_mcontext.gregs[REG_EBP];
    psut.ctx.uc_mcontext.gregs[REG_EBP] = (void *) *(ebp);
    psut.ctx.uc_mcontext.gregs[REG_EIP] = __builtin_return_address(0);
    psut.ctx.uc_mcontext.gregs[REG_ESP] = ((void *) ebp) + 8;
    if (curr_mode == 0) {
        sock_fd = setup_new_socket(curr_mode, hostname);

        ssize_t n;
        const char *p = &psut;
        size_t length = sizeof(psut);
        while (length > 0) {
            n = send(sock_fd, p, length, 0);
            if (n <= 0) {
                printf("Error in send\n");
                return -1;
            }
            p += n;
            length -= n;
            printf("Size sent: %d\n", n);
        }
        exit(0);
    } else {
        return;
    }

}
