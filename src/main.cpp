#include "../h/riscv.hpp"
#include "../h/Thread.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/Scheduler.hpp"

extern "C" void traps(uint64 syscall_id ...);

extern void userMain();

extern void testMain();

struct ControlCard
{
    volatile bool userThreadDone = false;
};

void idle(void* arg)
{
    ControlCard* controlCard = (ControlCard*)arg;
    while (!controlCard->userThreadDone)
    {
        thread_dispatch();
    }
}

int main()
{
    // | 1 for vectored mode, see trap.S
    Riscv::w_stvec((uint64)traps | 1);

    _thread* main_thread = new _thread();

    _thread::running = main_thread;

    ControlCard controlCard;
    Thread* idleThread = new Thread(idle, reinterpret_cast<void*>(&controlCard));
    idleThread->start();

    Thread* userThread = new Thread((void(*)(void*))userMain, nullptr);
    userThread->start();
    userThread->join();
    controlCard.userThreadDone = true;
    idleThread->join();
    delete userThread;
    delete idleThread;

    return 0;
}