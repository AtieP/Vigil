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
