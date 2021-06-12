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
#include <proc/mutex.h>
#include <proc/sched.h>
#include <tools/vector.h>

#define VFS_FILE_LEN 8

typedef signed long ssize_t;

struct vfs_opened_file {
    pid_t pid;
    size_t seek;
    size_t file_size;
    struct vfs_node *node;
};

struct vfs_fs {
    char id[VFS_FILE_LEN];
    int (*open)(struct vfs_opened_file *file, const char *path, int mode);
    ssize_t (*read)(struct vfs_opened_file *file, void *buf, size_t count);
    ssize_t (*write)(struct vfs_opened_file *file, const void *buf, size_t count);
    int (*close)(struct vfs_opened_file *file, int fd);
};

struct vfs_node {
    char name[VFS_FILE_LEN];
    struct vfs_node *parent;
    struct vector children;
    struct vfs_fs *fs;
    void *fs_specific_data;
};

void vfs_init();
struct vfs_node *vfs_node_get(struct vfs_node *parent, const char *path);
struct vfs_node *vfs_node_append_child(struct vfs_node *parent, const char *name);
void vfs_node_remove_child(struct vfs_node *parent, const char *name);
struct vfs_node *vfs_node_mount(struct vfs_fs *fs, struct vfs_node *parent, const char *name);

int vfs_open(pid_t pid, const char *parent, const char *path, int mode);
ssize_t vfs_read(pid_t pid, int fd, void *buf, size_t count);
ssize_t vfs_write(pid_t pid, int fd, const void *buf, size_t count);
int vfs_close(pid_t pid, int fd);

#endif
