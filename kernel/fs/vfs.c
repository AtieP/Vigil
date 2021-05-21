#include <stddef.h>
#include <fs/vfs.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <mp/mutex.h>
#include <tools/builtins.h>
#include <tools/panic.h>

#define MODULE_NAME "vfs"

static struct vfs_node *root_node = NULL;

static size_t strlen_slash(const char *string) {
    size_t len = 0;
    while (*string != '/' && *string != '\0') {
        string++;
        len++;
    }
    return len;
}

void vfs_init() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Initializing");
    root_node = kheap_calloc(sizeof(struct vfs_node));
    root_node->name[0] = '/';
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Finished initializing");
}

struct vfs_node *vfs_get_node(const char *path) {
    // okay don't complicate our life too much if the user just wants the root node
    if (*path == '/' && *(path + 1) == '\0') {
        return root_node;
    }
    struct vfs_node *node = root_node->child;
    while (node) {
        path++;
        if (*path == '\0') {
            break;
        }
        mutex_lock(&node->mutex);
        // item: one of the parts of the path
        // for example, lol in /hello/lol/yes/ is an item
        size_t item_length = strlen_slash(path);
        if (!strncmp(node->name, path, strlen(node->name))) {
            path += item_length;
            mutex_unlock(&node->mutex);
            if (*path == '\0') {
                return node;
            }
            node = node->child;
            continue;
        }
        path += item_length;
        mutex_unlock(&node->mutex);
        node = node->next;     
    }
    return NULL;
}

struct vfs_node *vfs_create_node(struct vfs_node *parent_node, const char *name) {
    mutex_lock(&parent_node->mutex);
    struct vfs_node *new_node = kheap_calloc(sizeof(struct vfs_node));
    // place the node somewhere
    struct vfs_node *node = parent_node->child;
    if (!node) {
        parent_node->child = new_node;
        node = new_node;
    } else {
        while (node->next) {
            node = node->next;
        }
        node->next = new_node;
    }
    memcpy(node->name, name, strlen(name));
    mutex_unlock(&parent_node->mutex);
    return new_node;
}

int vfs_delete_node(const char *name) {
    (void) name;
    panic(MODULE_NAME, "Not implemented yet");
}
