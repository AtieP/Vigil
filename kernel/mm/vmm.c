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

#include <stddef.h>
#include <stdint.h>
#include <cpu/msr.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <proc/mutex.h>
#include <tools/builtins.h>

#define MODULE_NAME "vmm"

static struct vmm_pagemap kernel_pagemap;

static uint64_t *get_next_level(uint64_t *table, size_t index) {
    if (!(table[index] & 1)) {
        table[index] = (uint64_t) pmm_calloc(1);
        table[index] |= 0b11; // rw, present
    }
    return (uint64_t *) ((table[index] & ~(0x1ff)) + MM_HIGHER_BASE);
}

struct vmm_pagemap vmm_new_pagemap() {
    struct vmm_pagemap new_pagemap;
    new_pagemap.pml4 = (uint64_t *) pmm_calloc(1);
    return new_pagemap;
}

void vmm_map_page(struct vmm_pagemap *pagemap, uintptr_t physical_address, uintptr_t virtual_address, uint64_t flags) {
    mutex_lock(&kernel_pagemap.mutex);
    size_t pml4_index = (size_t) (virtual_address & ((size_t) 0x1ff << 39)) >> 39;
    size_t pml3_index = (size_t) (virtual_address & ((size_t) 0x1ff << 30)) >> 30;
    size_t pml2_index = (size_t) (virtual_address & ((size_t) 0x1ff << 21)) >> 21;
    size_t pml1_index = (size_t) (virtual_address & ((size_t) 0x1ff << 12)) >> 12;

    uint64_t *pml3 = get_next_level(pagemap->pml4, pml4_index);
    uint64_t *pml2 = get_next_level(pml3, pml3_index);
    uint64_t *pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = physical_address | flags;
    mutex_unlock(&kernel_pagemap.mutex);
}

void vmm_unmap_page(struct vmm_pagemap *pagemap, uintptr_t virtual_address) {
    mutex_lock(&kernel_pagemap.mutex);
    size_t pml4_index = (size_t) (virtual_address & ((size_t) 0x1ff << 39)) >> 39;
    size_t pml3_index = (size_t) (virtual_address & ((size_t) 0x1ff << 30)) >> 30;
    size_t pml2_index = (size_t) (virtual_address & ((size_t) 0x1ff << 21)) >> 21;
    size_t pml1_index = (size_t) (virtual_address & ((size_t) 0x1ff << 12)) >> 12;

    uint64_t *pml3 = get_next_level(pagemap->pml4, pml4_index);
    uint64_t *pml2 = get_next_level(pml3, pml3_index);
    uint64_t *pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = 0;
    mutex_unlock(&kernel_pagemap.mutex);
}

void vmm_init(struct stivale2_struct_tag_memmap *memory_map) {
    (void) memory_map;

    kernel_pagemap.pml4 = (uint64_t *) pmm_calloc(1);

    for (uintptr_t i = 0; i < 0x80000000; i += MM_PAGE_SIZE) {
        vmm_map_page(&kernel_pagemap, i, i + MM_KERNEL_BASE, 0b11);
    }

    for (uintptr_t i = 0; i < 0x200000000; i += MM_PAGE_SIZE) {
        vmm_map_page(&kernel_pagemap, i, i + MM_HIGHER_BASE, 0b11);
    }

    vmm_switch_pagemap((uintptr_t) kernel_pagemap.pml4);
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Higher half and kernel mapped successfully");
}
