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

#include <dev/storage/mem.h>
#include <fs/devfs.h>
#include <fs/fd.h>
#include <fs/vfs.h>
#include <mm/mm.h>

static struct pmm_memory memory;

static int mem_open(struct vfs_opened_file *file, int mode) {
    (void) mode;
    file->file_size = memory.total;
    file->seek = 0;
    return fd_create(file->pid, file);
}

static ssize_t mem_read(struct vfs_opened_file *file, void *buf, size_t count) {
    uint8_t *buf_uint8 = (uint8_t *) buf;
    size_t i;
    for (i = 0; i < count; i++) {
        if (file->seek > file->file_size) {
            break;
        }
        buf_uint8[i] = *((uint8_t *) (file->seek++ + MM_HIGHER_BASE));
    }
    return i;
}

static ssize_t mem_write(struct vfs_opened_file *file, const void *buf, size_t count) {
    size_t i;
    for (i = 0; i < count; i++) {
        if (file->seek > file->file_size) {
            break;
        }
        *((uint8_t *) (file->seek++ + MM_HIGHER_BASE)) = ((uint8_t *) buf)[i];
    }
    return i;
}

static int mem_close(struct vfs_opened_file *file, int fd) {
    return fd_destroy(file->pid, fd);
}

struct devfs_dev mem_dev = {
    .name = "mem",
    .open = mem_open,
    .read = mem_read,
    .write = mem_write,
    .close = mem_close
};

void dev_mem_init() {
    memory = pmm_get_memory();
    devfs_register(&mem_dev);
}
