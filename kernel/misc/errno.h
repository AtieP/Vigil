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

#ifndef __MISC_ERRNO_H__
#define __MISC_ERRNO_H__

#include <cpu/locals.h>

#define errno \
    locals_cpu_get()->current_thread->errno

#define E2BIG 1 // Argument list is too big
#define EACCES 2 // Permission denied
#define EADDRINUSE 3 // Address in use
#define EADDRNOTAVAIL 4 // Address not available
#define EAFNOSUPPORT 5 // Address family not supported
#define EAGAIN 6 // Resource not available, try again
#define EALREADY 7 // Connection already in process
#define EBADF 8 // Bad file descriptor
#define EBADMSG 9 // Bad message
#define EBUSY 10 // Device or resource busy
#define ECANCELED 11 // Operation canceled
#define ECHILD 12 // No child processes
#define ECONNABORTED 13 // Connection aborted
#define ECONNREFUSED 14 // Connection refused
#define ECONNRESET 15 // Connection reset
#define EDEADLK 16 // Dead lock
#define EDESTADDRREQ 17 // Destination address required
#define EDOM 18 // Maths argument out of domain of function
#define EDQUOT 19 // Reserved
#define EEXIST 20 // File exists
#define EFAULT 21 // Bad address
#define EFBIG 22 // File too big
#define EHOSTUNREACH 23 // Could not reach host
#define EIDRM 24 // Identifier removed
#define EILSEQ 25 // Invalid byte sequence
#define EINPROGRESS 26 // Operation in progress
#define EINTR 27 // Interrupted function
#define EINVAL 28 // Invalid argument
#define EIO 29 // Input/output error
#define EISCONN 30 // Socket is connected
#define EISDIR 31 // Is a directory
#define ELOOP 32 // Too many levels for symbolic links
#define EMFILE 33 // File descriptor value too large
#define EMLINK 34 // Too many links
#define EMSGSIZE 35 // Message too big
#define EMULTIHOP 36 // Reserved
#define ENAMETOOLONG 37 // Filename too long
#define ENETDOWN 38 // Network is down
#define ENETRESET 39 // Connection aborted by network
#define ENETUNREACH 40 // Network unreachable
#define ENFILE 41 // Too many files open in a system
#define ENOBUFS 42 // No buffer space available
#define ENODATA 43 // No message is available
#define ENODEV 44 // No such device
#define ENOENT 45 // No such file or directory
#define ENOEXEC 46 // Executable file format error
#define ENOLCK 47 // No locks available
#define ENOLINK 48 // Reserved
#define ENOMEM 49 // Not enough memory
#define ENOMSG 50 // No message of the desired kind
#define ENOPROTOOPT 51 // Protocol not available
#define ENOSPC 52 // No space left on device
#define ENOSR 53 // No stream resources
#define ENOSTR 54 // Not a stream
#define ENOSYS 55 // Functionality not supported
#define ENOTCONN 56 // Socket is not connected
#define ENOTDIR 57 // Not a directory nor symbolic link to directory
#define ENOTEMPTY 58 // Directory not available
#define ENOTRECOVERABLE 59 // State not recoverable
#define ENOTSOCK 60 // Not a socket
#define ENOTSUP 61 // Not supported
#define ENOTTY 62 // Invalid input/output control operation
#define ENXIO 63 // No such device or address
#define EOPNOTSUPP 64 // Operation not supported on socket
#define EOVERFLOW 65 // Value too big to be stored in a data type
#define EOWNERDEAD 66 // Previous owner died
#define EPERM 67 // Operation not permitted
#define EPIPE 68 // Broken pipe
#define EPROTO 69 // Protocol error
#define EPROTONOSUPPORT 70 // Protocol not supported
#define EPROTOTYPE 71 // Protocol wrong type for socket
#define ERANGE 72 // Result too large
#define EROFS 73 // Read-only filesystem
#define ESPIPE 74 // Invalid seek
#define ESRCH 75 // No such process
#define ESTALE 76 // Reserved
#define ETIME 77
#define ETIMEDOUT 78 // Connection timed out
#define ETXTBSY 79 // Text file busy
#define EWOULDBLOCK 80 // Operation would block
#define EXDEV 81 // Cross-device link

#endif
