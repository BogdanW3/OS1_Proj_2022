#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/Thread.hpp"
#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"
#include "../h/Scheduler.hpp"
#include "../h/Semaphore.hpp"
#include "../h/printing.hpp"

extern "C" void internal_handler()
{
    volatile uint64 sepc = Riscv::r_sepc();
    volatile uint64 sstatus = Riscv::r_sstatus();
    uint64 scause = Riscv::r_scause();

    if (scause == 8 || scause == 9)
    {
        sepc += 4;
        size_t call = Riscv::r_user_reg<size_t>(A0);
        switch (call)
        {
            case MEM_ALLOC:
            {
                size_t size = Riscv::r_user_reg<size_t>(A1);
                void* ret = MemoryAllocator::getInstance().alloc(size);
                Riscv::w_user_reg(A0, ret);
                break;
            }
            case MEM_FREE:
            {
                void* ptr = Riscv::r_user_reg<void*>(A1);
                int ret = MemoryAllocator::getInstance().free(ptr);
                Riscv::w_user_reg(A0, ret);
                break;
            }
            case THREAD_CREATE:
            {
                thread_t* handle = Riscv::r_user_reg<thread_t*>(A1);
                thread_body_t start_routine = Riscv::r_user_reg<thread_body_t>(A2);
                void* arg = Riscv::r_user_reg<void*>(A3);
                void* stack = Riscv::r_user_reg<void*>(A4);
                if (handle == nullptr || start_routine == nullptr || stack == nullptr)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    _thread* t = new _thread(start_routine, arg, stack);
                    *handle = t;
                    Riscv::w_user_reg(A0, 0);
                }
                break;
            }
            case THREAD_EXIT:
            {
                _thread::running->finished = true;
                // add all waiting threads back to ready queue in sched
                thread_t curr = _thread::running->waiting;
                while (curr != nullptr)
                {
                    thread_t next = curr->next;
                    curr->ready = true;
                    Scheduler::getInstance().addReady(curr);
                    curr = next;
                }
                _thread::dispatch();
                _printString("Thread exit error\n");
                Riscv::w_user_reg(A0, -1);
                break;
            }
            case THREAD_DISPATCH:
            {
                _thread::dispatch();
                break;
            }
            case THREAD_JOIN:
            {
                thread_t handle = Riscv::r_user_reg<thread_t>(A1);
                if (handle == nullptr || handle->finished)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    handle->join();
                    _thread::dispatch();
                    Riscv::w_user_reg(A0, 0);
                }
                break;
            }
            case THREAD_GETID:
            {
                Riscv::w_user_reg(A0, _thread::running->id);
                _thread::dispatch();
                break;
            }
            case SEM_OPEN:
            {
                sem_t* handle = Riscv::r_user_reg<sem_t*>(A1);
                unsigned init = Riscv::r_user_reg<unsigned>(A2);
                if (handle == nullptr)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    *handle = new _sem(init);
                    Riscv::w_user_reg(A0, 0);
                }
                break;
            }
            case SEM_CLOSE:
            {
                sem_t handle = Riscv::r_user_reg<sem_t>(A1);
                if (handle == nullptr)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    
                    delete handle;
                    Riscv::w_user_reg(A0, 0);
                }
                break;
            }
            case SEM_WAIT:
            {
                sem_t handle = Riscv::r_user_reg<sem_t>(A1);
                if (handle == nullptr)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    if (--handle->val < 0)
                    {
                        _thread::running->ready = false;
                        thread_t prev = nullptr;
                        thread_t curr = handle->waiting;
                        while (curr != nullptr)
                        {
                            prev = curr;
                            curr = curr->next;
                        }
                        if (prev == nullptr)
                        {
                            handle->waiting = _thread::running;
                        }
                        else
                        {
                            prev->next = _thread::running;
                        }
                        _thread::dispatch();
                    }
                    Riscv::w_user_reg(A0, _thread::running->interrupted ? 0 : -1);
                    _thread::running->interrupted = false;
                }
                break;
            }
            case SEM_SIGNAL:
            {
                sem_t handle = Riscv::r_user_reg<sem_t>(A1);
                if (handle == nullptr)
                {
                    Riscv::w_user_reg(A0, -1);
                }
                else
                {
                    if (handle->val++ < 0)
                    {
                        thread_t curr = handle->waiting;
                        handle->waiting = curr->next;
                        curr->ready = true;
                        curr->interrupted = false;
                        Scheduler::getInstance().addReady(curr);
                    }
                    Riscv::w_user_reg(A0, 0);
                }
                break;
            }
            case TIME_SLEEP:
            {
                time_t time = Riscv::r_user_reg<time_t>(A1);
                _thread::running->ready = false;
                Scheduler::getInstance().addSleeping(_thread::running, time);
                _thread::dispatch();
                Riscv::w_user_reg(A0, 0);
                break;
            }
            case GETC:
            {
                char c = __getc();
                Riscv::w_user_reg(A0, c);
                break;
            }
            case PUTC:
            {
                char c = Riscv::r_user_reg<char>(A1);
                __putc(c);
                break;
            }
        }
    }
    else
    {
        _printString("Unhandled exception: SEPC = ");
        _printInteger(sepc, 16);
        _printString(", SCAUSE = ");
        _printInteger(scause);
        _printString("\n");

        while(1){asm("wfi");};
    }

    Riscv::w_sepc(sepc);
    Riscv::w_sstatus(sstatus);
}

extern "C" void timer_handler()
{
    
    volatile uint64 sepc = Riscv::r_sepc();
    volatile uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sip(Riscv::SIP_SSIP);
    Scheduler::getInstance().updateSleeping();
    _thread::time++;
    if (_thread::time >= DEFAULT_TIME_SLICE)
    {
        _thread::time = 0;
        _thread::dispatch();
    }
    Riscv::w_sepc(sepc);
    Riscv::w_sstatus(sstatus);
}

extern "C" void external_handler()
{
    console_handler();
}