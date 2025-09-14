#include "../h/riscv.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrc sstatus, %0" :: "r"(Riscv::SSTATUS_SPP));
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}