/*
Copyright Aurélien Chappuis (2014)

aurelienchappuis@ymail.com

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#ifndef ERRNO_H
#define ERRNO_H

extern enum errno_e errno;

enum errno_e {
    EPERM,	/* Not super-user */
    ENOENT,	/* No such file or directory */
    ESRCH,	/* No such process */
    EINTR,	/* Interrupted system call */
    EIO,	/* I/O error */
    ENXIO,	/* No such device or address */
    E2BIG,	/* Arg list too long */
    ENOEXEC,	/* Exec format error */
    EBADF,	/* Bad file number */
    ECHILD,	/* No children */
    EAGAIN,	/* No more processes */
    ENOMEM,	/* Not enough core */
    EACCES,	/* Permission denied */
    EFAULT,	/* Bad address */
    EBUSY,	/* Mount device busy */
    EEXIST,	/* File exists */
    EXDEV,	/* Cross-device link */
    ENODEV,	/* No such device */
    ENOTDIR,	/* Not a directory */
    EISDIR,	/* Is a directory */
    EINVAL,	/* Invalid argument */
    ENFILE,	/* Too many open files in system */
    EMFILE,	/* Too many open files */
    ENOTTY,	/* Not a typewriter */
    ETXTBSY,	/* Text file busy */
    EFBIG,	/* File too large */
    ENOSPC,	/* No space left on device */
    ESPIPE,	/* Illegal seek */
    EROFS,	/* Read only file system */
    EMLINK,	/* Too many links */
    EPIPE,	/* Broken pipe */
    EDOM,	/* Math arg out of domain of func */
    ERANGE,	/* Math result not representable */
    ENOMSG,	/* No message of desired type */
    EIDRM,	/* Identifier removed */
    EDEADLK,	/* Deadlock condition */
    ENOLCK,	/* No record locks available */
};
#endif