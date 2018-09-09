#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

#include "frames.h"
#include "paging.h"

extern uint32_t _kernel_end;

/* Start of the heap */
static void* const kheap_base = (void*)(&_kernel_end);

/* End of the heap */
static char* kheap_top;
/* End of the virtual address space */
static char* kheap_max;

void heap_init(void)
{
    kheap_max = (char*)(((uintptr_t)kheap_base) | (FRAME_SIZE - 1));
    kheap_top = (char*)kheap_base;
}

/* Sets the end of the heap to the value specified by addr,
   when that value is reasonable. On success returns 1, and
   returns -1 otherwise. */
int brk(void* addr)
{
    if(addr < kheap_base)
        return -1;

    /* Do we need to grow */
    if((char*)addr > kheap_max) {
        /* Figure out how many frames we need to get to addr */
        ptrdiff_t needed = (char*)addr - kheap_max;
        int nframes = (int)(needed / FRAME_SIZE) + 1;

        /* Grow the heap */
        for(int i = 0; i < nframes; i++) {
            frame_t frame = frame_alloc();
            if(!__frame_valid(frame))
                return -1;

            if(page_map(kheap_max + 1, frame))
                return -1;

            kheap_max += FRAME_SIZE;
        }
    }

    /* Do we need to shrink */
    else if((char*)addr < (kheap_max - FRAME_SIZE)) {
        ptrdiff_t extra = kheap_max - (char*)addr;
        int nframes = (int)(extra / FRAME_SIZE);

        for(int i = 0; i < nframes; i++) {
            char* last_page = (char*)((intptr_t)kheap_max & ~(FRAME_SIZE - 1));
            frame_t frame = page_unmap(last_page);
            if(!__frame_valid(frame))
                return -1;

            kheap_max = last_page - 1;
            frame_free(frame);
        }
    }

    kheap_top = addr;
    return 0;
}

/* Increments the heap by increment bytes. If increment is
   0 then the current top of the heap is returned. On success,
   return the previous top of the heap, otherwise return (void *) -1 */
void* sbrk(intptr_t increment)
{
    if(0 == increment)
        return (void*)kheap_top;

    void* old = (void*)kheap_top;

    if(increment > 0 ? ((uintptr_t)old + (uintptr_t)increment < (uintptr_t)old) :
                       ((uintptr_t)old + (uintptr_t)-increment))
        return (void*)-1;

    if(brk(kheap_top + increment))
        return (void*)-1;

    return old;
}
