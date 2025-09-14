#include "../h/Thread.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Scheduler.hpp"
#include "../h/riscv.hpp"

_thread* _thread::running = nullptr;
size_t _thread::time = 0;
size_t _thread::next_id = 0;

_thread::_thread(void (*start_routine)(void*), void* arg, void* stack): stack(stack),
    start_routine(start_routine), arg(arg),
    context({(uint64)stack + DEFAULT_STACK_SIZE, (uint64)execWrapper})
{
    Scheduler::getInstance().addReady(this);
}

_thread::_thread()
{
}

_thread::~_thread()
{
    if (stack != nullptr)
    {
        MemoryAllocator::getInstance().free(stack);
    }
}

void _thread::dispatch()
{
    _thread *old = _thread::running;
    if (old->ready && !old->finished)
        Scheduler::getInstance().addReady(old);
    running = Scheduler::getInstance().getReady();

    contextSwitch(&old->context, &running->context);
}

void _thread::join()
{
    running->ready = false;
    running->next = waiting;
    waiting = running;
}

void _thread::execWrapper()
{
    Riscv::popSppSpie();
    _thread::running->start_routine(_thread::running->arg);
    thread_exit();
}