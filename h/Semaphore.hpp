#ifndef _SEMAPHORE_HPP
#define _SEMAPHORE_HPP

class _thread;

class _sem
{
public:
    _sem(unsigned init = 1);
    ~_sem();

    int val;
    _thread* waiting = nullptr;
};

#endif // _SEMAPHORE_HPP