#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

#include "../h/syscall_c.h"

class Scheduler
{
public:
    static Scheduler& getInstance();

    void addReady(thread_t thread);
    void addSleeping(thread_t thread, time_t time);

    thread_t getReady();

    void updateSleeping();

private:
    Scheduler() {};
    thread_t ready = nullptr;
    thread_t sleeping = nullptr;
};

#endif // _SCHEDULER_HPP