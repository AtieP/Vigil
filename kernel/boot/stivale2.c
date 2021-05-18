#include <stddef.h>
#include <stdint.h>
#include <boot/stivale2.h>
#include <mm/mm.h>

static uint8_t stack[MM_PAGE_SIZE * 16];

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
