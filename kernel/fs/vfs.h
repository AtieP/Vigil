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

#ifndef __FS_VFS_H__
#define __FS_VFS_H__

#include <stdint.h>
#include <mp/mutex.h>
#include <tools/vector.h>

#define VFS_FILE_LEN 8

typedef signed long ssize_t;

struct vfs_fs {
    char id[VFS_FILE_LEN];
    int (*open)(const char *path, int mode);
    ssize_t (*read)(int fd, void *buf, size_t count);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    int (*close)(int fd);
};

struct vfs_node {
    char name[VFS_FILE_LEN];
    struct vfs_node *parent;
    struct vector children;
    struct vfs_fs *fs;
};

void vfs_init();
struct vfs_node *vfs_node_append_child(struct vfs_node *parent, const char *name);
void vfs_node_remove_child(struct vfs_node *parent, const char *name);

#endif
