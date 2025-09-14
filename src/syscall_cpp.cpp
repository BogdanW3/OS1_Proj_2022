#include "../h/syscall_cpp.hpp"


void* operator new(size_t size)
{
    return mem_alloc(size);
}

void operator delete(void* ptr)
{
    mem_free(ptr);
}

Thread::Thread(void (*body)(void *), void *arg) : myHandle(0)
{
    this->body = body;
    this->arg = arg;
}

Thread::~Thread()
{
    if (!myHandle) return;
    thread_join(myHandle);
}

int Thread::start()
{
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch()
{
    thread_dispatch();
}

void Thread::join()
{
    thread_join(myHandle);
}

int Thread::getID()
{
    return thread_getID();
}

int Thread::sleep(time_t time)
{
    return time_sleep(time);
}

void Thread::runWrapper(void* arg)
{
    Thread* t = (Thread*)arg;
    t->run();
}

Thread::Thread(): myHandle(0)
{
    body = runWrapper;
    arg = this;
}

Semaphore::Semaphore(unsigned init)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    if (!myHandle) return;
    sem_close(myHandle);
    myHandle = 0;
}

int Semaphore::wait()
{
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    return sem_signal(myHandle);
}


PeriodicThread::PeriodicThread(time_t period) : Thread()
{
    this->period = period;
}

PeriodicThread::~PeriodicThread()
{
    terminate();
}

void PeriodicThread::terminate()
{
    period = (time_t)-1;
}

void PeriodicThread::run()
{
    do
    {
        periodicActivation();
        Thread::sleep(period);
    } while (period != (time_t)-1);
}

char Console::getc()
{
    return ::getc();
}

void Console::putc(char c)
{
    ::putc(c);
}
