#ifndef __FS_VFS_H__
#define __FS_VFS_H__

#define VFS_MAX_NAME_LEN 8

#include <mp/mutex.h>

struct vfs_node {
    char name[VFS_MAX_NAME_LEN];
    struct vfs_node *parent;
    struct vfs_node *next;
    struct vfs_node *child;
    struct mutex mutex;
};

void vfs_init();
struct vfs_node *vfs_get_node(const char *path);
struct vfs_node *vfs_create_node(struct vfs_node *parent_node, const char *name);
int vfs_delete_node(const char *name);

#endif
