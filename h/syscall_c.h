#ifndef _SYSCALL_C_H
#define _SYSCALL_C_H

#include "../lib/hw.h"

enum calls 
{
    MEM_ALLOC = 0x01, MEM_FREE,
    THREAD_CREATE = 0x11, THREAD_EXIT, THREAD_DISPATCH, THREAD_JOIN, THREAD_GETID,
    SEM_OPEN = 0x21, SEM_CLOSE, SEM_WAIT, SEM_SIGNAL,
    TIME_SLEEP = 0x31,
    GETC = 0x41, PUTC
};

void* mem_alloc(size_t size);
int mem_free(void*);

class _thread;
typedef _thread* thread_t;

typedef void (*thread_body_t)(void*);

int thread_create(thread_t* handle, thread_body_t start_routine, void* arg);
int thread_exit();
void thread_dispatch();
void thread_join(thread_t handle);
int thread_getID();

class _sem;
typedef _sem* sem_t;

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id); 

int time_sleep(time_t);

const int EOF = -1;
char getc();
void putc(char);


#endif // _SYSCALL_C_H