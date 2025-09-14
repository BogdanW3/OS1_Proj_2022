#ifndef _syscall_cpp
#define _syscall_cpp

#include "../h/syscall_c.h"

void *operator new(size_t);             // global namespace implied
void operator delete(void *);           // global namespace implied

class Thread
{
public:
    Thread(void (*body)(void *), void *arg);
    virtual ~Thread();

    int start();

    void join();

    static int getID();

    static void dispatch();
    static int sleep(time_t);

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
    static void runWrapper(void*);
};

class Semaphore
{
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();
    int wait();
    int signal();

private:
    sem_t myHandle;
};

class PeriodicThread : public Thread
{
public:
    void terminate();
    virtual ~PeriodicThread() override;

protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation() {}
    virtual void run() override;

private:
    time_t period;
};

class Console
{
public:
    static char getc();
    static void putc(char);
};

#endif