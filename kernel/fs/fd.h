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

#ifndef __FS_FD_H__
#define __FS_FD_H__

#include <fs/vfs.h>
#include <proc/sched.h>

int fd_create(pid_t pid, struct vfs_opened_file *file);
int fd_dup(pid_t pid, int oldfd);
int fd_destroy(pid_t pid, int fd);

#endif
