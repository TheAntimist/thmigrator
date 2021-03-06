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
int curr_mode;
int sock_fd = -1;

typedef struct psu_context_s {
    ucontext_t ctx;
    char stack[16834];
} psu_context;

static psu_context psut;

typedef struct psu_thread {
    void *(*user_func)(void *);
    void *user_args;
    int ptid;
    int retval;
} psu_thread_t;

static psu_thread_t thread_obj;

void psu_thread_setup_init(int mode) {
    //Read from a file to set up the socket connection between the client and the server

    curr_mode = mode;
    if (mode == 0) {
        printf(" [info] Setting up Client..\n");
    } else {
        printf(" [info] Setting up server\n");
        sock_fd = setup_new_socket(mode, "NULL");
    }
}

void * norm_thread() {

    ucontext_t thread_ctx;
    getcontext(&(psut.ctx));
    psut.ctx.uc_stack.ss_size = sizeof(psut.stack);
    psut.ctx.uc_stack.ss_sp = psut.stack;
    psut.ctx.uc_link = NULL;
    psut.ctx.uc_stack.ss_flags = 0;

    makecontext(&psut.ctx, thread_obj.user_func, 1, thread_obj.user_args);
    if (swapcontext(&thread_ctx, &(psut.ctx)) == -1) {
        thread_obj.retval = -1;
        return NULL;
    }
    thread_obj.retval = 0;
    return NULL;
}

void * migrated_thread() {

    ucontext_t thread_ctx;
    greg_t eip = psut.ctx.uc_mcontext.gregs[REG_EIP];
    greg_t ebp = psut.ctx.uc_mcontext.gregs[REG_EBP];
    greg_t esp = psut.ctx.uc_mcontext.gregs[REG_ESP];
    getcontext(&(psut.ctx));
    psut.ctx.uc_stack.ss_size = sizeof(psut.stack);
    psut.ctx.uc_stack.ss_sp = psut.stack;
    psut.ctx.uc_link = &thread_ctx;
    psut.ctx.uc_stack.ss_flags = 0;

    makecontext(&(psut.ctx), thread_obj.user_func, 1, thread_obj.user_args);
    psut.ctx.uc_mcontext.gregs[REG_EIP] = eip;
    psut.ctx.uc_mcontext.gregs[REG_EBP] = ebp;
    psut.ctx.uc_mcontext.gregs[REG_ESP] = esp;
    if (swapcontext(&thread_ctx, &(psut.ctx)) == -1) {
        thread_obj.retval = -1;
        return NULL;
    }
    thread_obj.retval = 0;
    return NULL;

}


int psu_thread_create(void *(*user_func)(void *), void *user_args) {
    // make thread related setup
    // create thread and start running the function based on *user_func
    thread_obj.user_func = user_func;
    thread_obj.user_args = user_args;

    // client
    if (curr_mode == 0) {
        pthread_create(&thread_obj.ptid, NULL, norm_thread, &thread_obj);
    } else {

        ssize_t size = sizeof(psu_context);
        size_t length = 0;
        size_t n = 0;
        const char *p = &psut;
        while (n != size) {
            length = recv(sock_fd, p, size, 0);
            if (length <= 0) {
                printf(" [error] Error in receiving\n");
                return -1;
            }
            p += length;
            n += length;
            printf(" [debug] Reading %d bytes\n", length);
        }

        pthread_create(&thread_obj.ptid, NULL, migrated_thread, &thread_obj);
    }
    // TODO: Should we block here or return?
    pthread_join(thread_obj.ptid, NULL);
    printf(" [debug] Thread exited with value: %d\n", thread_obj.retval);
    return 0;
}

void proc_exit(int retval) {
    if (sock_fd > -1) shutdown(sock_fd, 2);
    thread_obj.retval = retval;
    pthread_exit(0);

}

int psu_thread_migrate(const char *hostname) {
    getcontext(&(psut.ctx));
    printf("EBP: %x\n", psut.ctx.uc_mcontext.gregs[REG_EBP]);
    void ** ebp = (greg_t *) psut.ctx.uc_mcontext.gregs[REG_EBP];
    psut.ctx.uc_mcontext.gregs[REG_EBP] = *(ebp);
    psut.ctx.uc_mcontext.gregs[REG_EIP] = *(ebp + 1);
    psut.ctx.uc_mcontext.gregs[REG_ESP] = ebp + 4;
    if (curr_mode == 0) {
        sock_fd = setup_new_socket(curr_mode, hostname);

        ssize_t n;
        const char *p = &psut;
        size_t length = sizeof(psut);
        while (length > 0) {
            n = send(sock_fd, p, length, 0);
            if (n <= 0) {
                printf(" [error] Error occurred while sending data over socket.\n");
                proc_exit(-1);
            }
            p += n;
            length -= n;
            printf(" [debug] Sent %d bytes over the socket\n", n);
        }
        proc_exit(0);
    }

    // Server mode
    return -1;
}
