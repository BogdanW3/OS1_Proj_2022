#include "../h/MemoryAllocator.hpp"

MemoryAllocator& MemoryAllocator::getInstance()
{
    static MemoryAllocator instance;
    return instance;
}

MemoryAllocator::MemoryAllocator()
{
    // align "inwards" to MEM_BLOCK_SIZE from both ends
    auto memStart = align_up((size_t)HEAP_START_ADDR, MEM_BLOCK_SIZE);
    auto memEnd = align_down((size_t)HEAP_END_ADDR, MEM_BLOCK_SIZE);

    MemoryDescriptor *mem = (MemoryDescriptor*)memStart;
    mem->next = nullptr;
    mem->size = memEnd - memStart;

    head = mem;

}

// input size is in blocks
void* MemoryAllocator::alloc(size_t size)
{
    // convert to bytes, and add one more block for the descriptor
    size = (size + 1)*MEM_BLOCK_SIZE;


    MemoryDescriptor* prev = nullptr;
    MemoryDescriptor* curr = head;

    while(curr != nullptr)
    {
        if(curr->size >= size)
        {
            if(curr->size - size >= MEM_BLOCK_SIZE)
            {
                MemoryDescriptor* newDesc = (MemoryDescriptor*)((size_t)curr + size);
                newDesc->next = curr->next;
                newDesc->size = curr->size - size;
                curr->size = size;

                if(prev == nullptr)
                    head = newDesc;
                else
                    prev->next = newDesc;
            }
            else
            {
                if(prev == nullptr)
                    head = curr->next;
                else
                    prev->next = curr->next;
            }

            curr->next = nullptr;

            // return the start of the block that's available to the user
            return (void*)((size_t)curr + MEM_BLOCK_SIZE);
        }

        prev = curr;
        curr = curr->next;
    }

    return nullptr;
}

int MemoryAllocator::free(void* ptr)
{
    // subtract one block to arrive at the descriptor
    ptr = (void*)((size_t)ptr - MEM_BLOCK_SIZE);

    if (ptr < HEAP_START_ADDR || ptr >= HEAP_END_ADDR)
        return -1;

    MemoryDescriptor* prev = nullptr;
    MemoryDescriptor* curr = head;

    while (curr != nullptr)
    {
        if (curr == ptr)
            return -1;

        if (curr > ptr)
            break;

        prev = curr;
        curr = curr->next;
    }

    MemoryDescriptor* desc = (MemoryDescriptor*)ptr;

    if (prev == nullptr)
    {
        head = desc;
    }
    else
    {
        prev->next = desc;
    }

    desc->next = curr;

    tryMerge(desc, curr);
    tryMerge(prev, desc);
    
    return 0;
}

