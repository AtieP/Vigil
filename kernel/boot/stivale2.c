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
#include <boot/stivale2.h>
#include <mm/mm.h>

static uint8_t stack[MM_PAGE_SIZE * 32];

static struct stivale2_header_tag_framebuffer fb = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header header = {
    .entry_point = 0,
    .stack = (uintptr_t) &stack + sizeof(stack),
    .flags = 0,
    .tags = (uintptr_t) &fb,
};

void *stivale2_get_tag(struct stivale2_struct *root, uint64_t id) {
    struct stivale2_tag *tag = (struct stivale2_tag *) (root->tags + MM_HIGHER_BASE);
    while (tag) {
        if (tag->identifier == id) {
            return tag;
        }
        tag = (struct stivale2_tag *) (tag->next + MM_HIGHER_BASE);
    }
    return NULL;
}
