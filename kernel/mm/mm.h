/*  
    This file is part of Vigil.

    Vigil is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vigil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vigil.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __MM_MM__
#define __MM_MM__

#include <stddef.h>
#include <stdint.h>
#include <boot/stivale2.h>
#include <proc/mutex.h>

#define MM_PAGE_SIZE (4096)
#define MM_KERNEL_BASE (0xffffffff80000000)
#define MM_HIGHER_BASE (0xffff800000000000)

/*
    Physical memory manager
*/
struct pmm_memory {
    size_t total;
    size_t usable;
};

void pmm_init(struct stivale2_struct_tag_memmap *memory_map);
void pmm_print_memmap(struct stivale2_struct_tag_memmap *memory_map);
void *pmm_alloc(size_t pages);
void *pmm_calloc(size_t pages);
void *pmm_realloc(void *old, size_t newpages, size_t oldpages);
void pmm_free(void *ptr, size_t pages);
struct pmm_memory pmm_get_memory();

/*
    Virtual memory manager
*/

struct vmm_pagemap {
    uint64_t *pml4;
    struct mutex mutex;
};

struct vmm_pagemap vmm_new_pagemap();
void vmm_map_page(struct vmm_pagemap *pagemap, uintptr_t physical_address, uintptr_t virtual_address, uint64_t flags);
void vmm_unmap_page(struct vmm_pagemap *pagemap, uintptr_t virtual_address);
static inline void vmm_switch_pagemap(struct vmm_pagemap *pagemap) {
    asm volatile("mov %0, %%cr3" :: "r"((uintptr_t) pagemap->pml4) : "memory");
}
struct vmm_pagemap *vmm_get_kernel_pagemap();
void vmm_init(struct stivale2_struct_tag_memmap *memory_map);

/*
    Kernel heap memory manager
*/
void kheap_walkthrough();
void kheap_init();
void *kheap_alloc(size_t size);
void *kheap_calloc(size_t size);
void *kheap_realloc(void *old, size_t newsize, size_t oldsize);
void kheap_free(void *ptr);

#endif
