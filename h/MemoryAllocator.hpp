#ifndef _MEMORYALLOCATOR_HPP
#define _MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
    public:
    static MemoryAllocator& getInstance();
    void* alloc(size_t size);
    int free(void* ptr);
    private:
    MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    class MemoryDescriptor {
        public:
        MemoryDescriptor* next;
        size_t size;
    };

    void tryMerge(MemoryDescriptor* prev, MemoryDescriptor* curr) {
        if(prev != nullptr && (size_t)prev + prev->size == (size_t)curr) {
            prev->size += curr->size;
            prev->next = curr->next;
        }
    }

    static constexpr size_t align_up(size_t size, size_t align) {
        return (size + align - 1) / align * align;
    }

    static constexpr size_t align_down(size_t size, size_t align) {
        return size / align * align;
    }

    MemoryDescriptor* head;
};

#endif // _MEMORYALLOCATOR_HPP