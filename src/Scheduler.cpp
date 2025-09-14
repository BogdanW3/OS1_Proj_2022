#include "../h/Scheduler.hpp"

#include "../h/Thread.hpp"

Scheduler& Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

void Scheduler::addReady(thread_t thread)
{
    thread_t prev = nullptr;
    thread_t curr = ready;
    while(curr != nullptr)
    {
        prev = curr;
        curr = curr->next;
    }

    if(prev == nullptr)
    {
        ready = thread;
    }
    else
    {
        prev->next = thread;
    }
    thread->next = nullptr;
}

void Scheduler::addSleeping(thread_t thread, time_t time)
{
    // Adds the thread to the sleeping list, sorted by time
  
    thread_t prev = nullptr;
    thread_t curr = sleeping;
    while(curr != nullptr && curr->sleeping_time <= time)
    {
        prev = curr;
        curr = curr->next;
    }

    if(prev == nullptr)
    {
        sleeping = thread;
    }
    else
    {
        prev->next = thread;
    }
    thread->next = curr;
    thread->sleeping_time = time;

}

thread_t Scheduler::getReady()
{
    thread_t ret = ready;
    if(ret != nullptr)
    {
        ready = ready->next;
        ret->next = nullptr;
    }
    return ret;
}

void Scheduler::updateSleeping()
{
    thread_t prev = nullptr;
    thread_t curr = sleeping;
    while(curr != nullptr)
    {
        thread_t next = curr->next;
        if(curr->sleeping_time == 0)
        {
            if(prev == nullptr)
            {
                sleeping = next;
            }
            else
            {
                prev->next = next;
            }
            curr->ready = true;
            addReady(curr);
        }
        else
        {
            curr->sleeping_time--;
            prev = curr;
        }
        curr = next;
    }
}