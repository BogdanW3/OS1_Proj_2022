#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "../lib/hw.h"
#include "syscall_c.h"

class _thread {
    public:
        static _thread* running;
        static size_t time;
        _thread(thread_body_t start_routine, void* arg, void* stack);
        _thread();
        ~_thread();
        _thread(const _thread&) = delete;
        _thread& operator=(const _thread&) = delete;

        static void dispatch();

        void join();

        // for linking this thread in lists
        thread_t next = nullptr;
        // threads waiting for this thread to finish
        thread_t waiting = nullptr;

        void* stack = nullptr;

        size_t sleeping_time = 0;
        bool ready = true;
        bool interrupted = false;
        bool finished = false;

        const size_t id = next_id++;

        struct Context
        {
            uint64 sp;
            uint64 ra;
        };

        thread_body_t start_routine = nullptr;
        void* arg = nullptr;
    private:

        
        Context context;

        static void contextSwitch(Context *oldContext, Context *runningContext);
        
        static void execWrapper();

        static size_t next_id;

};

#endif // _THREAD_HPP