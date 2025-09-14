#include "../h/syscall_c.h"

template <typename T> T ecall(int id, ...)
{
    T ret;
    asm volatile ("ecall");
    asm volatile ("mv %0, a0" : "=r"(ret));
    return ret;
}

template <> void ecall<void>(int id, ...)
{
    asm volatile ("ecall");
}

void* mem_alloc(size_t size)
{
    // round up to block size fitting these bytes
    return ecall<void*>(MEM_ALLOC, (size + MEM_BLOCK_SIZE - 1)/ MEM_BLOCK_SIZE);
}

int mem_free(void* ptr)
{
    return ecall<int>(MEM_FREE, ptr);
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)
{
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    return ecall<int>(THREAD_CREATE, handle, start_routine, arg, stack);
}

int thread_exit()
{
    return ecall<int>(THREAD_EXIT);
}

void thread_dispatch()
{
    ecall<void>(THREAD_DISPATCH);
}

void thread_join(thread_t handle)
{
    ecall<void>(THREAD_JOIN, handle);
}

int thread_getID()
{
    return ecall<int>(THREAD_GETID);
}

int sem_open(sem_t* handle, unsigned init)
{
    return ecall<int>(SEM_OPEN, handle, init);
}

int sem_close(sem_t handle)
{
    return ecall<int>(SEM_CLOSE, handle);
}

int sem_wait(sem_t id)
{
    return ecall<int>(SEM_WAIT, id);
}

int sem_signal(sem_t id)
{
    return ecall<int>(SEM_SIGNAL, id);
}

int time_sleep(time_t time)
{
    return ecall<int>(TIME_SLEEP, time);
}

char getc()
{
    return ecall<char>(GETC);
}

void putc(char c)
{
    ecall<void>(PUTC, c);
}