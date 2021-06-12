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

#include <fs/fd.h>
#include <fs/vfs.h>
#include <proc/sched.h>
#include <tools/nummap.h>

int fd_create(pid_t pid, struct vfs_opened_file *file) {
    struct sched_process *process = sched_get_process(pid);
    if (!process) {
        return -1;
    }
    return nummap_add(&process->fds, file);
}

int fd_dup(pid_t pid, int oldfd) {
    struct sched_process *process = sched_get_process(pid);
    if (!process) {
        return -1;
    }
    struct vfs_opened_file *file = nummap_get(&process->fds, oldfd);
    if (!file) {
        return -1;
    }
    return nummap_add(&process->fds, file);
}

int fd_destroy(pid_t pid, int fd) {
    struct sched_process *process = sched_get_process(pid);
    if (!process) {
        return -1;
    }
    return nummap_remove(&process->fds, fd) ? 0 : -1;
}