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
#include <fs/vfs.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <tools/builtins.h>

#define MODULE_NAME "vfs"

static struct vfs_node *root;

static size_t strlen_slash(const char *string) {
    size_t counter = 0;
    while (*string != '\0' && *string != '/') {
        string++;
        counter++;
    };
    return counter;
}

void vfs_init() {
    root = kheap_calloc(sizeof(struct vfs_node));
    vector_create(&root->children, sizeof(struct vfs_node *));
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Successfully created root node");
}

struct vfs_node *vfs_node_append_child(struct vfs_node *parent, const char *name) {
    if (!parent) {
        parent = root;
    }
    struct vfs_node *node = kheap_calloc(sizeof(struct vfs_node));
    memcpy(node->name, name, strlen(name));
    node->parent = parent;
    node->fs = parent->fs;
    vector_create(&node->children, sizeof(struct vfs_node *));
    vector_push(&parent->children, node);
    return node;
}

void vfs_node_remove_child(struct vfs_node *parent, const char *name) {
    if (!parent) {
        parent = root;
    }
    for (size_t i = 0; i < parent->children.items; i++) {
        struct vfs_node *node = vector_get(&parent->children, i);
        if (!strcmp(node->name, name)) {
            // todo: recursively remove
            vector_remove(&parent->children, i);
            vector_delete(&node->children);
            kheap_free(node);
            return;
        }
    }
    kcon_log(KCON_LOG_WARN, MODULE_NAME, "Could not delete node %s", name);
}
