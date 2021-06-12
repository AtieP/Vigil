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

#include <fs/devfs.h>
#include <fs/vfs.h>
#include <mm/mm.h>

// Note: the devices take care of the file descriptors.

static struct vfs_node *dev_node;

static int devfs_open(struct vfs_opened_file *file, const char *path, int mode) {
    struct vfs_node *node = vfs_node_get(dev_node, path);
    if (!node) {
        return -1;
    }
    file->node = node;
    return ((struct devfs_dev *) node->fs_specific_data)->open(file, mode);
}

static ssize_t devfs_read(struct vfs_opened_file *file, void *buf, size_t count) {
    return ((struct devfs_dev *) file->node->fs_specific_data)->read(file, buf, count);
}

static ssize_t devfs_write(struct vfs_opened_file *file, const void *buf, size_t count) {
    return ((struct devfs_dev *) file->node->fs_specific_data)->write(file, buf, count);
}

static int devfs_close(struct vfs_opened_file *file, int fd) {
    return ((struct devfs_dev *) file->node->fs_specific_data)->close(file, fd);   
}

static struct vfs_fs devfs_fs = {
    .open = devfs_open,
    .read = devfs_read,
    .write = devfs_write,
    .close = devfs_close
};

void devfs_init() {
    dev_node = vfs_node_mount(&devfs_fs, NULL, "dev");
}

void devfs_register(struct devfs_dev *device) {
    struct vfs_node *node = vfs_node_append_child(dev_node, device->name);
    node->fs_specific_data = device;
}

void devfs_unregister(struct devfs_dev *device) {
    vfs_node_remove_child(dev_node, device->name);
}
