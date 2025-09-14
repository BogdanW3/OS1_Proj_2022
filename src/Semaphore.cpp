#include "../h/Semaphore.hpp"

#include "../h/Scheduler.hpp"
#include "../h/Thread.hpp"

_sem::_sem(unsigned init): val(init)
{
}

_sem::~_sem()
{
    thread_t curr = waiting;
    while (curr != nullptr)
    {
        thread_t next = curr->next;
        curr->ready = true;
        curr->interrupted = true;
        Scheduler::getInstance().addReady(curr);
        curr = next;
    }
}