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

#ifndef __FS_DEVFS_H__
#define __FS_DEVFS_H__

#include <fs/vfs.h>

struct devfs_dev {
    char name[VFS_FILE_LEN];
    int (*open)(struct vfs_opened_file *file, int mode);
    ssize_t (*read)(struct vfs_opened_file *file, void *buf, size_t count);
    ssize_t (*write)(struct vfs_opened_file *file, const void *buf, size_t count);
    int (*close)(struct vfs_opened_file *file);
};

void devfs_init();
void devfs_register(struct devfs_dev *device);
void devfs_unregister(struct devfs_dev *device);

#endif
