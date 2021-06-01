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
#include <proc/mutex.h>
#include <tools/builtins.h>
#include <tools/vector.h>

#define MODULE_NAME "vfs"

static struct vfs_node *root;
static struct mutex vfs_mutex;

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


struct vfs_node *vfs_node_get(struct vfs_node *parent, const char *path) {
    mutex_lock(&vfs_mutex);
    struct vfs_node *parent_node;
    struct vfs_node *child_node;
    if (!parent) {
        parent_node = root;
    } else {
        parent_node = parent;
    }
    while (1) {
next:
        if (*path == '\0') {
            mutex_unlock(&vfs_mutex);
            return NULL;
        }
        for (size_t i = 0; i < parent_node->children.items; i++) {
            child_node = vector_get(&parent_node->children, i);
            // item in this context: one of the elements of the path
            size_t item_length = strlen_slash(path);
            if (strlen(child_node->name) != item_length) {
                continue;
            }
            if (!strncmp(child_node->name, path, item_length)) {
                path += item_length;
                if (*path == '\0') {
                    mutex_unlock(&vfs_mutex);
                    return child_node;
                }
                parent_node = child_node;
                goto next;
            }
        }
        mutex_unlock(&vfs_mutex);
        return NULL;
    }
}

struct vfs_node *vfs_node_append_child(struct vfs_node *parent, const char *name) {
    mutex_lock(&vfs_mutex);
    if (!parent) {
        parent = root;
    }
    struct vfs_node *node = kheap_calloc(sizeof(struct vfs_node));
    memcpy(node->name, name, strlen(name));
    node->parent = parent;
    node->fs = parent->fs;
    vector_create(&node->children, sizeof(struct vfs_node *));
    vector_push(&parent->children, node);
    mutex_unlock(&vfs_mutex);
    return node;
}

void vfs_node_remove_child(struct vfs_node *parent, const char *name) {
    mutex_lock(&vfs_mutex);
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
            mutex_unlock(&vfs_mutex);
            return;
        }
    }
    mutex_unlock(&vfs_mutex);
    kcon_log(KCON_LOG_WARN, MODULE_NAME, "Could not delete node %s", name);
}
